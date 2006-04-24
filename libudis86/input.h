/* -----------------------------------------------------------------------------
 * input.h
 *
 * Copyright (c) 2006, Vivek Mohan <vivek@sig9.com>
 * All rights reserved. See LICENSE
 * -----------------------------------------------------------------------------
 */
#ifndef UD_INPUT_H
#define UD_INPUT_H

#include <inttypes.h>
#include <udis86.h>

extern unsigned char inp_next(struct ud*);
extern unsigned char inp_peek(struct ud*);
extern uint8_t inp_uint8(struct ud*);
extern uint16_t inp_uint16(struct ud*);
extern uint32_t inp_uint32(struct ud*);
extern uint64_t inp_uint64(struct ud*);

/* inp_init() - Initializes the input system. */
static inline void inp_init(struct ud* u)
{
  u->inp_curr = NULL;
  u->inp_fill = NULL;
  u->inp_sess = NULL;
  u->inp_ctr  = 0;
}

/* inp_start() - Should be called before each de-code operation. */
static inline void inp_start(struct ud* u)
{
  u->inp_ctr = 0;
 
  if (u->inp_curr == u->inp_fill) {
	u->inp_curr = NULL;
	u->inp_fill = NULL;
	u->inp_sess = u->inp_cache;
  } else 
	u->inp_sess = u->inp_curr + 1;
}

/* inp_back() - Move back a byte. */
static inline void inp_back(struct ud* u)
{
  if (u->inp_ctr > 0) {
	--u->inp_curr;
	--u->inp_ctr; 
  }
}

/* inp_back() - Resets the current pointer to its position before the current
 * instruction disassembly was started.
 */
static inline void inp_reset(struct ud* u)
{
  u->inp_curr -= u->inp_ctr;
  u->inp_ctr = 0;
}

/* inp_cur() - Returns the current input byte. */
static inline unsigned char inp_curr(struct ud* u)
{
	if (u->inp_curr == NULL) 
		return(0);
	return *(u->inp_curr);
}

#endif
