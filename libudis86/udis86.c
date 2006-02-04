/* udis86.c
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <udis86.h>
#include <string.h>
#include "source.h"

/* ud_init(ud_t *ud) - Initializes ud_t object.
 */
extern void ud_init(ud_t *ud)
{
	memset((void*)ud, 0, sizeof(ud_t));
	ud->dis_mode = MODE16;
	ud->mnemonic = Iinvalid;
	ud_src_init(ud);
	ud_clear_insn(ud);
}

/* ud_set_intput(ud_t, type, input, end) - Sets the input source for the 
 * disassembler.
 */
extern void ud_set_input(ud_t* ud, ud_input_t input_type, 
				void *input, void *end)
{
	switch(input_type) {
		case INPUT_HOOK:
			ud->source.input.hook = (int (*)())input;
			ud->source.input_type = INPUT_HOOK;
			break;
		case INPUT_BUFFERED:
			ud->source.input.buffered.start = (char*) input;
			ud->source.input.buffered.end = (char*)end;
			ud->source.input_type = INPUT_BUFFERED;
			break;
		case INPUT_FILE:
			ud->source.input.file = (FILE*) input;
			ud->source.input_type = INPUT_FILE;
			break;
		default:
			ud->source.input.file = stdin;
			ud->source.input_type = INPUT_FILE;
			break;
	}	
}

/* wrappers */
extern void ud_set_input_hook(ud_t* ud, int (*hook)()) 
{ 
	ud_set_input(ud, INPUT_HOOK, hook, NULL); 
}

extern void ud_set_input_buffered(ud_t* ud, char* buffer, size_t size) 
{ 
	ud_set_input(ud, INPUT_BUFFERED, buffer, buffer + size); 
}

extern void ud_set_input_file(ud_t* ud, FILE* filep) 
{ 
	ud_set_input(ud, INPUT_FILE, filep, NULL); 
}


/* ud_set_dis_mode(ud, mode) - sets the disassembly mode.
 */
extern void ud_set_dis_mode(ud_t* ud, ud_mode_t mode)
{
	switch(mode) {
		case MODE16:
		case MODE32:
		case MODE64:
			ud->dis_mode = mode;
			break;
		default:
			ud->dis_mode = MODE16;
	}
}

/* ud_set_origin(ud, origin) - sets code origin.
 */
extern void ud_set_origin(ud_t* ud, unsigned int origin)
{
	ud->pc = origin;
}

/* ud_asmout_offset(ud) - returns the offset.
 */
extern unsigned int ud_asmout_offset(ud_t* ud) 
{
	return ud->asmout.offset;
}

/* ud_asmout_hex(ud) - returns the hex form of bytes disassembled.
 */
extern const char* ud_asmout_hex(ud_t* ud) 
{
	return ud->asmout.hexcode;
}

/* ud_asmout_insn(ud) - returns the translated assembly instrution.
 */
extern const char* ud_asmout_insn(ud_t* ud) 
{
	return ud->asmout.buffer;
}

/* disassembles one instruction and returns the number of bytes disassembled */
extern unsigned int ud_disassemble(ud_t* ud, ud_syntax_t s)
{
	char* src_hex;
	uint8_t* src_ptr;
	unsigned int i = 0;
	unsigned int src_bytes;

	/* if no more bytes to disassemble, leave */
	if (ud_src_nomore(ud))
		return 0;

	/* decode */
	ud_src_start(ud);
	ud_decode(ud);

	/* determine the number of bytes disassembled */
	if ((src_bytes = ud->source.counter) == 0)
		return(0);

	/* set offset */
	ud->asmout.fill_ptr = 0;
	ud->asmout.offset = ud->pc;
	ud->pc += src_bytes;

	/* call syntax translator */
	if (s == SYNTAX_ATT)
		ud_translate_att(ud);
	else ud_translate_intel(ud);

	/* compose hex code */
	src_ptr = ud->source.sess_ptr;
	src_hex = ud->asmout.hexcode;
	for (i = 0; i < src_bytes; ++i, ++src_ptr) {
		sprintf(src_hex, "%02x", *src_ptr);
		src_hex += 2;
	}

	/* return number of bytes disassembled */
	return src_bytes;
}
