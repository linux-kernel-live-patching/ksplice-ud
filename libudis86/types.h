/* types.h
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#ifndef _UD_TYPES_H_
#define _UD_TYPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "mnemonics.h"

/* ud_opcmap_operand_type_t - operand types enumerated
 *
 * OP_XXX, define the type of the operand in the opcode map. The naming was 
 * inspired by the AMD manuals. To understand the specifics, read the manuals.
 */
typedef enum ud_opcmap_operand_type_t {
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
} ud_opcmap_operand_type_t;

/* ud_operand_size_t - operand sizes enumerated
 *
 * SZ_XX - define the size of the operand in the opcode map. Again, the naming
 * is related to the AMD Manuals, check them out for specifics.
 */
typedef enum ud_operand_size_t {
	SZ_b = 1,
	SZ_w,
	SZ_z,
	SZ_v,
	SZ_vw,
	SZ_d,
	SZ_q,
	SZ_p,
	SZ_wp, 
	SZ_dp,
	SZ_sb,
	SZ_jb,
	SZ_jw,
	SZ_jd,
	SZ_x,
	SZ_m
} ud_operand_size_t;


/* WARNING - Make sure the order of enumeration is same as order
 * of operand script declarations in syntax.c
 */
typedef enum ud_operand_type_t {

	/* 16 bit addressing modes */
	M_BX_SI = 0, 
	M_BX_DI, 
	M_BP_SI,
	M_BP_DI,
	M_SI,
	M_DI,
	M_OFF16,
	M_BX,
	M_BX_SI_OFF8,
	M_BX_DI_OFF8,
	M_BP_SI_OFF8,
	M_BP_DI_OFF8,
	M_SI_OFF8,
	M_DI_OFF8,
	M_BP_OFF8,
	M_BX_OFF8,
	M_BX_SI_OFF16,
	M_BX_DI_OFF16,
	M_BP_SI_OFF16,
	M_BP_DI_OFF16,
	M_SI_OFF16,
	M_DI_OFF16,
	M_BP_OFF16,
	M_BX_OFF16,

	/* 32 bit addressing modes */
	M_EAX,
	M_ECX,
	M_EDX,
	M_EBX,
	M_SIB,
	M_OFF32,
	M_ESI,
	M_EDI,
	M_EAX_OFF8,
	M_ECX_OFF8,
	M_EDX_OFF8,
	M_EBX_OFF8,
	M_SIB_OFF8,
	M_EBP_OFF8,
	M_ESI_OFF8,
	M_EDI_OFF8,
	M_EAX_OFF32,
	M_ECX_OFF32,
	M_EDX_OFF32,
	M_EBX_OFF32,
	M_SIB_OFF32,
	M_EBP_OFF32,
	M_ESI_OFF32,
	M_EDI_OFF32,
	M_SID,

	/* (amd)64 bit addressing modes */
	M_RAX,
	M_RCX,
	M_RDX,
	M_RBX,
	M_RSIB,
	M_ROFF32,
	M_RSI,
	M_RDI,
	M_R8,
	M_R9,
	M_R10,
	M_R11,
	M_R12,
	M_R13,
	M_R14,
	M_R15,
	M_RAX_OFF8,
	M_RCX_OFF8,
	M_RDX_OFF8,
	M_RBX_OFF8,
	M_RSIB_OFF8,
	M_RBP_OFF8,
	M_RSI_OFF8,
	M_RDI_OFF8,
	M_R8_OFF8,
	M_R9_OFF8,
	M_R10_OFF8,
	M_R11_OFF8,
	M_R12_OFF8,
	M_R13_OFF8,
	M_R14_OFF8,
	M_R15_OFF8,
	M_RAX_OFF32,
	M_RCX_OFF32,
	M_RDX_OFF32,
	M_RBX_OFF32,
	M_RSIB_OFF32,
	M_RBP_OFF32,
	M_RSI_OFF32,
	M_RDI_OFF32,
	M_R8_OFF32,
	M_R9_OFF32,
	M_R10_OFF32,
	M_R11_OFF32,
	M_R12_OFF32,
	M_R13_OFF32,
	M_R14_OFF32,
	M_R15_OFF32,
	M_RSID,

	/* mem OFFSET */
	OFFSET16,
	OFFSET32,
	OFFSET64,

	/* 8 bit GPRs */
	R_AL,
	R_CL,
	R_DL,
	R_BL,
	R_AH,
	R_CH,
	R_DH,
	R_BH,
	R_SPL,
	R_BPL,
	R_SIL,
	R_DIL,
	R_R8B,
	R_R9B,
	R_R10B,
	R_R11B,
	R_R12B,
	R_R13B,
	R_R14B,
	R_R15B,

	/* 16 bit GPRs */
	R_AX,
	R_CX,
	R_DX,
	R_BX,
	R_SP,
	R_BP,
	R_SI,
	R_DI,
	R_R8W,
	R_R9W,
	R_R10W,
	R_R11W,
	R_R12W,
	R_R13W,
	R_R14W,
	R_R15W,
	
	/* 32 bit GPRs */
	R_EAX,
	R_ECX,
	R_EDX,
	R_EBX,
	R_ESP,
	R_EBP,
	R_ESI,
	R_EDI,
	R_R8D,
	R_R9D,
	R_R10D,
	R_R11D,
	R_R12D,
	R_R13D,
	R_R14D,
	R_R15D,
	
	/* 64 bit GPRs */
	R_RAX,
	R_RCX,
	R_RDX,
	R_RBX,
	R_RSP,
	R_RBP,
	R_RSI,
	R_RDI,
	R_R8,
	R_R9,
	R_R10,
	R_R11,
	R_R12,
	R_R13,
	R_R14,
	R_R15,

	/* segment registers */
	R_ES,
	R_CS,
	R_SS,
	R_DS,
	R_FS,
	R_GS,
	
	/* control registers*/
	R_CR0,
	R_CR1,
	R_CR2,
	R_CR3,
	R_CR4,
	R_CR5_INV,
	R_CR6_INV,
	R_CR7_INV,
	
	/* debug registers */
	R_DR0,
	R_DR1,
	R_DR2,
	R_DR3,
	R_DR4_INV,
	R_DR5_INV,
	R_DR6,
	R_DR7,

	/* mmx registers */
	R_MM0,
	R_MM1,
	R_MM2,
	R_MM3,
	R_MM4,
	R_MM5,
	R_MM6,
	R_MM7,

	/* x87 registers */
	R_ST0,
	R_ST1,
	R_ST2,
	R_ST3,
	R_ST4,
	R_ST5,
	R_ST6,
	R_ST7,

	/* extended multimedia registers */
	R_XMM0,
	R_XMM1,
	R_XMM2,
	R_XMM3,
	R_XMM4,
	R_XMM5,
	R_XMM6,
	R_XMM7,
	R_XMM8,
	R_XMM9,
	R_XMM10,
	R_XMM11,
	R_XMM12,
	R_XMM13,
	R_XMM14,
	R_XMM15,

	/* miscellany */
	I_BYTE,
	I_WORD,
	I_DWRD,
	J_IMM, 

	/* pointers */
	P_32, 
	P_48, 

	/* special */
	PRINT_I, 

	/* Extras */
	M_ESP,
	M_ESP_OFF8,
	M_ESP_OFF32,
	M_RSP,
	M_RSP_OFF8,
	M_RSP_OFF32
} ud_operand_type_t;

/* disassembly/operand/addressing modes */
typedef enum ud_mode_t {
	MODE16,
	MODE32,
	MODE64
} ud_mode_t;

/* input sources */
typedef enum ud_input_t {
	INPUT_NONE,
	INPUT_HOOK,
	INPUT_BUFFERED,
	INPUT_FILE
} ud_input_t;

/* input sources */
typedef enum ud_syntax_t {
	SYNTAX_INTEL,
	SYNTAX_ATT
} ud_syntax_t;

/* ud_opcmap_operand_t - Operand in the opcode map. 
 * .type = the type of the operand (enum ud_opcmap_operand_type_t).
 * .size = the size of the operand (enum ud_operand_size_t).
 */
typedef struct ud_opcmap_operand_t {
	ud_opcmap_operand_type_t type;
	ud_operand_size_t        size;
} ud_opcmap_operand_t;

/* ud_opcmap_entry_t - Opcode Table Entry structure - Level 1 output.
 * .mnemonic = instruction mnemonic code 
 * .operand1 = operand 1
 * .operand2 = operand 2
 * .operand3 = operand 3
 * .prefix   = allowed prefixes/suffixes 
 * 
 */
typedef struct ud_opcmap_entry_t {
	ud_mnemonic_code_t	mnemonic;	
	ud_opcmap_operand_t	operand1;	
	ud_opcmap_operand_t	operand2;	
	ud_opcmap_operand_t	operand3;	
	uint32_t		prefix;		
} ud_opcmap_entry_t;

/* ud_operand_t - Instruction Operand Structure
 *
 * This structure is associated with each operand of the decoded instruction. 
 */
typedef struct ud_operand_t {
	ud_operand_size_t size;  /* operand size */
	ud_operand_type_t type;  /* operand type */

	/* lval part of the instruction, the size being dependent on the
	 * instruction.
	 */
	union {
		int8_t  bits8;
		int16_t bits16;
		int32_t bits32;
		int64_t bits64;
		uint8_t bits8u;
		uint16_t bits16u;
		uint32_t bits32u;
		uint64_t bits64u;
		struct {
			int16_t seg;
			int32_t off;
		} ptr;
	} lval;

	/* decoded scale-index-base (SIB) */
	struct {		
		int8_t base;	/* - Base  */
		int8_t index;	/* - Index */
		int8_t scale;	/* - Scale */
	} sib;
} ud_operand_t;


/* flag type */
typedef uint8_t ud_flag_t;

/* source object */
typedef struct ud_src_t {
	ud_input_t input_type;
	uint8_t buffer[64];
	uint8_t* sess_ptr;
	uint8_t* fill_ptr;
	uint8_t* curr_ptr;
	size_t counter;
	ud_flag_t end;
	union {
		int (*hook)();
		struct {
			char* start;
			char* end;
		} buffered;
		FILE* file;		
	} input;
} ud_src_t;

/* instruction prefixes */
typedef struct ud_prefix_t {
	uint8_t rex;
	uint8_t seg;	/* segment override */
	uint8_t opr;	/* 0x66 prefix flag */
	uint8_t adr;	/* 0x67 prefix flag */
	uint8_t lock;	/* 0xF0 (lock) prefix flag */
	uint8_t rep;	/* 0xF3 (rep) prefix flag */
	uint8_t repe;	/* 0xF3 (repe) prefix flag */
	uint8_t repne;	/* 0xF2 (repne) prefix flag */
} ud_prefix_t;

/* final output */
typedef struct ud_asmout_t {
	int32_t offset;
	char hexcode[32];
	char buffer[64];
	unsigned int fill_ptr;
} ud_asmout_t;


/* ud_t - The current state of the disassembler library. These variables were, earlier, 
 * global. The encapsulation makes the code more thread safe and re-entrant.
 */

typedef struct ud_t {
	ud_src_t source;
	uint32_t pc;

	/* flags */
	ud_mode_t dis_mode;
	ud_mode_t opr_mode;
	ud_mode_t adr_mode;
	ud_flag_t error;	
	ud_flag_t suffix;
	ud_flag_t default64;
	ud_flag_t cast;

	/* Level-1: opcode map entry */
	ud_opcmap_entry_t* opcmap_entry;

	/* Level-2: decoded instruction */
	ud_prefix_t  prefix;
	ud_mnemonic_code_t mnemonic;
	ud_operand_t operand[3];

	/* Level-3: output */
	ud_asmout_t asmout;
} ud_t;

/* syntax generator plugin structure */
typedef struct ud_syntax_plugin_t {
	/* procedure hooks */
	void (*opr_cast)(ud_t*, ud_operand_size_t, int*);
	void (*rel_cast)(ud_t*, ud_operand_size_t, int*);
	void (*sib)(ud_t*, unsigned int);
	void (*si)(ud_t*, unsigned int);
	void (*rsib)(ud_t*, unsigned int);
	void (*rsi)(ud_t*, unsigned int);

	/* data hooks */
	char** s_regs;
	char** d_regs;
	char** q_regs;

	/* operand decoder script */
	struct ud_syntax_map_t* smap;
} ud_syntax_plugin_t;

/* This structure defines the decoder script for a particular 
 * ouput syntax. 
 */
typedef struct ud_syntax_map_t {
	ud_operand_type_t type;		/* operand type */
	char* script;			/* translation script */
} ud_syntax_map_t;

#endif
