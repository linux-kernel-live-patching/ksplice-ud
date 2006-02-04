/* opcmap.h
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <udis86.h>
#include "extern.h"
#include "types.h"

#define BUFFERMAX 0x16

/* src_load_byte() - loads a byte from the source and returns it. */
static int src_load_byte(ud_t* ud)
{
	/* switch between inputs, depending on the type */
	switch (ud->source.input_type) {
		case INPUT_HOOK:
			return(ud->source.input.hook());
			break;
		case INPUT_BUFFERED:
			if (ud->source.input.buffered.start 
				<= ud->source.input.buffered.end) {
				return(*ud->source.input.buffered.start++);
			} else {
				return(-1);
			}
			break;
		case INPUT_FILE:
			return(fgetc(ud->source.input.file));
			break;
		default:
			return(-1);
	}
}

/* ud_src_init() - Initializes the source buffer pointers. fill_ptr and curr_ptr 
 * are both set to -1, indicating an  uninitiated state. This function must be 
 * called before the disassembly session.
 */
extern void ud_src_init(register ud_t *ud)
{
	ud->source.input_type = INPUT_NONE;
	ud->source.curr_ptr = NULL;
	ud->source.fill_ptr = NULL;
	ud->source.counter  = 0;
	ud->source.sess_ptr = NULL;
}

/* ud_src_start() - Initializes the source buffering unit for a single
 * instruction disassembly. This must be called every time before calling the 
 * decoder.
 */
extern void ud_src_start(ud_t* ud)
{
	/* reset byte counter */
	ud->source.counter = 0;

	/* if both pointers are equal, start afresh */
	if (ud->source.curr_ptr == ud->source.fill_ptr) {
		ud->source.curr_ptr = NULL;
		ud->source.fill_ptr = NULL;
		ud->source.sess_ptr = ud->source.buffer;
	} else {
		/* they are not equal, which means that curr pointer is still
		 * pointing to the last byte of the previous instruction. So
		 * session pointer is made to point to the next byte, keeping
		 * curr_ptr the same way till a byte is actually requested.
		 */
		ud->source.sess_ptr = ud->source.curr_ptr + 1;
	}
}

/* ud_src_nomore() - Returns a non zero if there are no more bytes to be 
 * disassembled.
 */
extern int ud_src_nomore(ud_t* ud)
{
	return((ud->source.curr_ptr == ud->source.fill_ptr) 
			&& ud->source.end);
}

/* ud_src_skip() - skip n source bytes.
 */
extern void ud_src_skip(ud_t* ud, size_t n)
{
	while (n--)
		src_load_byte(ud);
}

/* src_next() - Loads the next byte from source. The byte is actually loaded
 * from the external source only if curr_ptr and fill_ptr are equal, else
 * only curr_ptr moves one byte ahead towards fill_ptr. If the external source
 * indicates an end, the "error" and "end" flags are set.
 */
extern uint8_t src_next(register ud_t *ud) 
{
	int c;
	
	/* if curr_ptr is less than fill_ptr it means that there are bytes
	 * already buffered.
	 */
	if (ud->source.curr_ptr < ud->source.fill_ptr) {
		++ud->source.curr_ptr;
		++ud->source.counter;
		return(*ud->source.curr_ptr);
	}

	/* if the source input has ended, the error flag is set */
	if (ud->source.end) {
		ud->error = 1;
		return(0);
	}

	/* load byte from source */
	c = src_load_byte(ud);
		
	/* a -1 indicates end of source input */
	if (c == -1) {
		ud->error = 1;
		ud->source.end = 1;
		return 0;
	} else {
		if (ud->source.fill_ptr == NULL) {
			ud->source.curr_ptr = ud->source.buffer;
			ud->source.fill_ptr = ud->source.buffer;
		} else {
			++ud->source.curr_ptr;
			++ud->source.fill_ptr;
		}
		++ud->source.counter;
		*(ud->source.fill_ptr) = c;
		return c;
	}
}

/* src_back() - Moves back one byte. (Rewind).
 */
extern void src_back(register ud_t *ud)
{
	/* the count should be atleast 1, to facilitate a push back */
	if (ud->source.counter > 0) {
		/* decrement both curr_ptr and byte counter */
		--ud->source.curr_ptr;
		--ud->source.counter; 
	}
}

/* src_reset() - Resets the current pointer to its position before the current
 * instruction disassembly was started.
 */
extern void src_reset(register ud_t *ud)
{
	ud->source.curr_ptr -= ud->source.counter;
	ud->source.counter = 0;
}

/* src_cur() - Returns the current source byte.
 */
extern uint8_t src_curr(register ud_t *ud)
{
	if (ud->source.curr_ptr == NULL) 
		return(0);
	return *(ud->source.curr_ptr);
}

/* src_currptr() - Returns pointer to current byte.
 */
extern uint8_t* src_currptr(register ud_t *ud)
{
	return ud->source.curr_ptr;
}

/* src_move() - Moves ahead n bytes 
 */
extern void src_move(register ud_t* ud, int8_t n) 
{
	/* check buffer overflow */
	assert((ud->source.fill_ptr + n) < (ud->source.buffer + BUFFERMAX));
	/* move ahead, byte by byte */	
	while (n --)
		src_next(ud);
}

/* src_peek() - Peek into the next byte in source. 
 */
extern uint8_t src_peek(register ud_t *ud) 
{
	int8_t r = src_next(ud);
	src_back(ud);
	return r;
}

/*------------------------------------------------------------------------------
 *  src_uintN() - return uintN from source.
 *------------------------------------------------------------------------------
 */
extern uint8_t src_uint8(register ud_t *ud)
{
	uint8_t *ref;
	return src_next(ud);
	ref = src_currptr(ud);
	return *((uint8_t*)ref);
}

extern uint16_t src_uint16(register ud_t *ud)
{
	uint8_t *ref;

	src_next(ud);
	ref = src_currptr(ud);
	src_move(ud, sizeof(uint16_t) - 1);

	return *((uint8_t*)ref) | (*((uint8_t*)(ref+1)) << 8);
}

extern uint32_t src_uint32(register ud_t *ud)
{
	uint8_t *ref;
	src_next(ud);
	ref = src_currptr(ud);
	src_move(ud, sizeof(uint32_t) - 1);
	return *((uint32_t*)(ref));

	return *((uint8_t*)ref) | 
		(*((uint8_t*)(ref+1)) << 8) |
		(*((uint8_t*)(ref+2)) << 16) |
		(*((uint8_t*)(ref+3)) << 24);
}

extern uint64_t src_uint64(register ud_t *ud)
{
	uint8_t *ref;
	uint64_t ret, r;

	src_next(ud);
	ref = src_currptr(ud);
	src_move(ud, sizeof(uint64_t) - 1);
	return *((uint64_t*)(ref));

	ret = *((uint8_t*)ref) | 
		(*((uint8_t*)(ref+1)) << 8) |
		(*((uint8_t*)(ref+2)) << 16) |
		(*((uint8_t*)(ref+3)) << 24);
	r = *((uint8_t*)(ref+4));
	ret = ret |  (r << 32);
	r = *((uint8_t*)(ref+4));
	ret = ret |  (r << 40);
	r = *((uint8_t*)(ref+4));
	ret = ret |  (r << 48);
	r = *((uint8_t*)(ref+4));
	ret = ret |  (r << 54);

	return ret;
}
