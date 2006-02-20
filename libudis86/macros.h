/* macros.h
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#ifndef _UD_MACROS_H_
#define _UD_MACROS_H_

#define SIB_S(b)	((b) >> 6)
#define SIB_I(b)	(((b) >> 3) & 7)
#define SIB_B(b)	((b) & 7)
#define MODRM_REG(b)	(((b) >> 3) & 7)
#define MODRM_NNN(b)	(((b) >> 3) & 7)
#define MODRM_MOD(b)	(((b) >> 6) & 3)
#define MODRM_RM(b)	((b) & 7)

/* Keep this in sync with opcmap.h */
#define P_A32(n)	((n) & 0x01)
#define P_O32(n)	((n) & 0x02)
#define P_REX(n)	((n) & 0x04)
#define P_DEF64(n)	((n) & 0x08)
#define P_INV64(n)	((n) & 0x10)
#define P_SUFF(n)	((n) & 0x20)
#define P_DEP32(n)	((n) & 0x40)
#define P_CAST(n)	((n) & 0x10000000)
#define P_PRFX(n)	((n) & ~0x20)
#define P_REX_W(r) 	((0xF & (r))  >> 3)
#define P_REX_R(r) 	((0x7 & (r))  >> 2)
#define P_REX_X(r) 	((0x3 & (r))  >> 1)
#define P_REX_B(r) 	((0x1 & (r))  >> 0)
#define P_REX_MASK(n)	(0x40 | (0xF & ((n) >> 16)))

#endif
