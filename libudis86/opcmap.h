/* opcmap.h
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#ifndef _UD_OPCMAP_H_
#define _UD_OPCMAP_H_

/* Instruction Prefixes - */
/* Keep this in sync with ud-macros.h */
#if _UD_BINARY_
/* definitions only for internal use */
#  define Pnone		0x00
#  define Pa32		0x01
#  define Po32		0x02
#  define Pdef64	0x08
#  define Pinv64	0x10
#  define Psuff		0x20
#  define Pdep32	0x40
#  define Pcast		0x10000000
#  define _W		8
#  define _R		4
#  define _X		2
#  define _B		1
#  define REX(c)	((40 | c) << 16)
#endif

#endif
