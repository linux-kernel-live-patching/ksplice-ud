/* -----------------------------------------------------------------------------
 * opcmap.h
 *
 * Copyright (c) 2005, 2006, Vivek Mohan <vivek@sig9.com>
 * All rights reserved. See LICENSE
 * -----------------------------------------------------------------------------
 */
#ifndef UD_OPCMAP_H
#define UD_OPCMAP_H

#include "types.h"

#define Pnone		0x00
#define Pa32		0x01
#define P_A32(n)	((n) & 0x01)
#define Po32		0x02
#define P_O32(n)	((n) & 0x02)
#define Pdef64		0x08
#define P_DEF64(n)	((n) & 0x08)
#define Pinv64		0x10
#define P_INV64(n)	((n) & 0x10)
#define Psuff		0x20
#define P_SUFF(n)	((n) & 0x20)
#define Pdep32		0x40
#define P_DEP32(n)	((n) & 0x40)
#define Pcast		0x10000000
#define P_CAST(n)	((n) & 0x10000000)
#define P_PRFX(n)	((n) & ~0x20)
#define REX(c)	((40 | c) << 16)
#define P_REX_MASK(n)	(0x40 | (0xF & ((n) >> 16)))
#define _W		8
#define _R		4
#define _X		2
#define _B		1
#define P_REX_W(r) 	((0xF & (r))  >> 3)
#define P_REX_R(r) 	((0x7 & (r))  >> 2)
#define P_REX_X(r) 	((0x3 & (r))  >> 1)
#define P_REX_B(r) 	((0x1 & (r))  >> 0)
#define SIB_S(b)	((b) >> 6)
#define SIB_I(b)	(((b) >> 3) & 7)
#define SIB_B(b)	((b) & 7)
#define MODRM_REG(b)	(((b) >> 3) & 7)
#define MODRM_NNN(b)	(((b) >> 3) & 7)
#define MODRM_MOD(b)	(((b) >> 6) & 3)
#define MODRM_RM(b)	((b) & 7)

/* -----------------------------------------------------------------------------
 * Enumeration of types of the operands in the opcode map. The naming was 
 * inspired by the AMD manuals. To understand the specifics, read the manuals.
 * -----------------------------------------------------------------------------
 */
enum map_operand_type 
{
  OP_NONE = 0,
  OP_A,
  OP_E,
  OP_M,
  OP_G,
  OP_I,
  OP_AL,
  OP_CL,
  OP_DL,
  OP_BL,
  OP_AH,
  OP_CH,
  OP_DH,
  OP_BH,
  OP_ALr8b,
  OP_CLr9b,
  OP_DLr10b,
  OP_BLr11b,
  OP_AHr12b, 
  OP_CHr13b,
  OP_DHr14b,
  OP_BHr15b,
  OP_AX,
  OP_CX,
  OP_DX, 
  OP_BX,
  OP_SI,
  OP_DI,
  OP_SP,
  OP_BP,
  OP_rAX,
  OP_rCX,
  OP_rDX,
  OP_rBX,
  OP_rSP,
  OP_rBP,
  OP_rSI,
  OP_rDI,
  OP_rAXr8,
  OP_rCXr9,
  OP_rDXr10,
  OP_rBXr11,
  OP_rSPr12,
  OP_rBPr13, 
  OP_rSIr14,
  OP_rDIr15,
  OP_eAX,
  OP_eCX,
  OP_eDX,
  OP_eBX,
  OP_eSP, 
  OP_eBP,
  OP_eSI,
  OP_eDI,
  OP_ES,
  OP_CS,
  OP_SS,
  OP_DS,
  OP_FS,
  OP_GS,
  OP_ST0,
  OP_ST1,
  OP_ST2,
  OP_ST3,
  OP_ST4,
  OP_ST5,
  OP_ST6,
  OP_ST7,
  OP_J,
  OP_S,
  OP_O,
  OP_I1,
  OP_I3,
  OP_V,
  OP_W,
  OP_Q,
  OP_P, 
  OP_R,
  OP_C,
  OP_D,
  OP_VR,
  OP_PR
};

struct map_operand 
{
  enum map_operand_type	type;
  enum ud_type			size;
};

struct map_entry 
{
  enum ud_mnemonic_code		mnemonic;	
  struct map_operand		operand1;
  struct map_operand		operand2;	
  struct map_operand		operand3;		
  uint32_t 			prefix;
};

#endif
