/* source.h
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#ifndef _SOURCE_H_
#define _SOURCE_H_

#include <inttypes.h>
#include <udis86.h>

extern void src_init(ud_t *ud);
extern void src_reset(ud_t *ud);
extern uint8_t* src_buffptr(ud_t *ud);
extern uint8_t* src_currptr(ud_t *ud);
extern uint8_t src_curr(ud_t *ud);
extern int src_currindex(ud_t *ud);
extern void src_back(ud_t *ud);
extern void src_flush(ud_t *ud);
extern uint8_t src_next(ud_t *ud);
extern uint8_t src_peek(ud_t *ud);
extern uint8_t src_uint8(register ud_t *ud);
extern uint16_t src_uint16(register ud_t *ud);
extern uint32_t src_uint32(register ud_t *ud);
extern uint64_t src_uint64(register ud_t *ud);

#endif
