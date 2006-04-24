/* -----------------------------------------------------------------------------
 * types.h
 *
 * Copyright (c) 2006, Vivek Mohan <vivek@sig9.com>
 * All rights reserved. See LICENSE
 * -----------------------------------------------------------------------------
 */

#ifndef UD_TYPES_H
#define UD_TYPES_H

#include <stdio.h>
#include <inttypes.h>
#include "mnemonics.h"

/* -----------------------------------------------------------------------------
 * All possible "types" of objects in udis86. Order is Important!
 * -----------------------------------------------------------------------------
 */
enum ud_type
{
  UD_NONE,

  /* 8 bit GPRs */
  UD_R_AL,	UD_R_CL,	UD_R_DL,	UD_R_BL,
  UD_R_AH,	UD_R_CH,	UD_R_DH,	UD_R_BH,
  UD_R_SPL,	UD_R_BPL,	UD_R_SIL,	UD_R_DIL,
  UD_R_R8B,	UD_R_R9B,	UD_R_R10B,	UD_R_R11B,
  UD_R_R12B,	UD_R_R13B,	UD_R_R14B,	UD_R_R15B,

  /* 16 bit GPRs */
  UD_R_AX,	UD_R_CX,	UD_R_DX,	UD_R_BX,
  UD_R_SP,	UD_R_BP,	UD_R_SI,	UD_R_DI,
  UD_R_R8W,	UD_R_R9W,	UD_R_R10W,	UD_R_R11W,
  UD_R_R12W,	UD_R_R13W,	UD_R_R14W,	UD_R_R15W,
	
  /* 32 bit GPRs */
  UD_R_EAX,	UD_R_ECX,	UD_R_EDX,	UD_R_EBX,
  UD_R_ESP,	UD_R_EBP,	UD_R_ESI,	UD_R_EDI,
  UD_R_R8D,	UD_R_R9D,	UD_R_R10D,	UD_R_R11D,
  UD_R_R12D,	UD_R_R13D,	UD_R_R14D,	UD_R_R15D,
	
  /* 64 bit GPRs */
  UD_R_RAX,	UD_R_RCX,	UD_R_RDX,	UD_R_RBX,
  UD_R_RSP,	UD_R_RBP,	UD_R_RSI,	UD_R_RDI,
  UD_R_R8,	UD_R_R9,	UD_R_R10,	UD_R_R11,
  UD_R_R12,	UD_R_R13,	UD_R_R14,	UD_R_R15,

  /* segment registers */
  UD_R_ES,	UD_R_CS,	UD_R_SS,	UD_R_DS,
  UD_R_FS,	UD_R_GS,	

  /* control registers*/
  UD_R_CR0,	UD_R_CR1,	UD_R_CR2,	UD_R_CR3,
  UD_R_CR4,	UD_R_CR5_INV,	UD_R_CR6_INV,	UD_R_CR7_INV,
	
  /* debug registers */
  UD_R_DR0,	UD_R_DR1,	UD_R_DR2,	UD_R_DR3,
  UD_R_DR4_INV,	UD_R_DR5_INV,	UD_R_DR6,	UD_R_DR7,

  /* mmx registers */
  UD_R_MM0,	UD_R_MM1,	UD_R_MM2,	UD_R_MM3,
  UD_R_MM4,	UD_R_MM5,	UD_R_MM6,	UD_R_MM7,

  /* x87 registers */
  UD_R_ST0,	UD_R_ST1,	UD_R_ST2,	UD_R_ST3,
  UD_R_ST4,	UD_R_ST5,	UD_R_ST6,	UD_R_ST7, 

  /* extended multimedia registers */
  UD_R_XMM0,	UD_R_XMM1,	UD_R_XMM2,	UD_R_XMM3,
  UD_R_XMM4,	UD_R_XMM5,	UD_R_XMM6,	UD_R_XMM7,
  UD_R_XMM8,	UD_R_XMM9,	UD_R_XMM10,	UD_R_XMM11,
  UD_R_XMM12,	UD_R_XMM13,	UD_R_XMM14,	UD_R_XMM15,

  UD_R_RIP,

  /* Operand Types */
  UD_OP_IMM8,	UD_OP_IMM16,	UD_OP_IMM32,	UD_OP_IMM64,
  UD_OP_JIMM, 	UD_OP_PTR32, 	UD_OP_PTR48,	UD_OP_CONST, 	
  UD_OP_MEM,	UD_OP_REG,	UD_OP_OFF8,	UD_OP_OFF16,
  UD_OP_OFF32,	UD_OP_OFF64,	UD_OP_SIB,	UD_OP_RSIB,
  UD_OP_SID,	UD_OP_RSID,

  /* Mode */
  UD_MODE16,	UD_MODE32,	UD_MODE64,

  /* Input Type */
  UD_INP_HOOK,	UD_INP_BUFF,
	
  /* Syntax Type */
  UD_SYN_INTEL,	UD_SYN_ATT,

  /* Operand Size Types */
  UD_SZ_BYTE,	UD_SZ_SBYTE,	UD_SZ_WORD,	UD_SZ_DWORD,
  UD_SZ_QWORD,	UD_SZ_Z,	UD_SZ_V,	UD_SZ_VW,
  UD_SZ_P,	UD_SZ_WP, 	UD_SZ_DP,	UD_SZ_JBYTE,
  UD_SZ_JWORD,	UD_SZ_JDWORD,	UD_SZ_X,	UD_SZ_M

};

/* -----------------------------------------------------------------------------
 * struct ud_operand - Disassembled instruction Operand.
 * -----------------------------------------------------------------------------
 */
struct ud_operand 
{
  enum ud_type		size;
  enum ud_type		type;

  union {
	int8_t		sbyte;
	uint8_t		ubyte;
	int16_t		sword;
	uint16_t	uword;
	int32_t		sdword;
	uint32_t	udword;
	int64_t		sqword;
	uint64_t	uqword;

	struct {
		uint16_t seg;
		uint32_t off;
	} ptr;
  } lval;

  enum ud_type		base;
  enum ud_type		index;
  uint8_t		offset;
  uint8_t		scale;	
};

/* -----------------------------------------------------------------------------
 * struct ud - The udis86 object.
 * -----------------------------------------------------------------------------
 */
struct ud
{
  struct map_entry*	opcmap_entry;
  struct ud_operand	operand[3];

  unsigned long		pc;
  int			error;

  int 			(*inp_hook) (struct ud*);
  int 			inp_ctr;
  unsigned char 	inp_cache[64];
  unsigned char*	inp_curr;
  unsigned char*	inp_fill;
  unsigned char*	inp_buff;
  unsigned char*	inp_buff_end;
  unsigned char*	inp_sess;
  int			inp_end;
  FILE*			inp_file;

  unsigned char 	pfx_rex;
  unsigned char 	pfx_seg;
  unsigned char 	pfx_opr;
  unsigned char 	pfx_adr;
  unsigned char 	pfx_lock;
  unsigned char 	pfx_rep;
  unsigned char 	pfx_repe;
  unsigned char 	pfx_repne;

  unsigned char		sfx;
  unsigned char		default64;
  unsigned char		cast;
  enum ud_type		dis_mode;
  enum ud_type		opr_mode;
  enum ud_type		adr_mode;

  unsigned int		asm_offset;
  char			asm_hexcode[32];
  char			asm_buffer[64];
  unsigned int		asm_fill;

  enum ud_mnemonic_code	mnemonic;
};

/* -----------------------------------------------------------------------------
 * Type-definitions
 * -----------------------------------------------------------------------------
 */
typedef enum ud_type 		ud_type_t;
typedef enum ud_mnemonic_code	ud_mnemonic_code_t;

typedef struct ud 		ud_t;
typedef struct ud_operand 	ud_operand_t;

#endif
