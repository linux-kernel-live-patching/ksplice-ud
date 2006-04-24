/* -----------------------------------------------------------------------------
 * decode.c
 *
 * Copyright (c) 2005, 2006, Vivek Mohan <vivek@sig9.com>
 * All rights reserved. See LICENSE
 * -----------------------------------------------------------------------------
 */

#include <string.h>

#include "types.h"
#include "input.h"
#include "opcmap.h"
#include "mnemonics.h"

/* The max number of prefixes to an instruction */
#define MAX_PREFIXES	15

/* register types */
#define T_NONE	0
#define T_GPR	1
#define T_MMX	2
#define T_CRG	3
#define T_DBG	4
#define T_SEG	5
#define T_XMM	6

/* -----------------------------------------------------------------------------
 * resolve_operand_size()- Resolves the size of operand depending on the current
 * disassembly mode and instruction prefixes. See AMD Manual for details.
 * -----------------------------------------------------------------------------
 */
static inline enum ud_type 
resolve_operand_size( register struct ud* u, enum ud_type size )
{
  switch (size) {
	case UD_SZ_V:
		if (u->opr_mode == UD_MODE32)
			return UD_SZ_DWORD;
		if (u->opr_mode == UD_MODE16)
			return UD_SZ_WORD;
		if (u->opr_mode == UD_MODE64 && P_REX_W(u->pfx_rex)) {
			return UD_SZ_QWORD;
		} else return UD_SZ_DWORD;
	case UD_SZ_VW:
		if (u->dis_mode == UD_MODE64)
			return (u->pfx_opr) ? UD_SZ_WORD : UD_SZ_QWORD;
	case UD_SZ_Z:
		return (u->opr_mode == UD_MODE16) ? UD_SZ_WORD : UD_SZ_DWORD;
	case UD_SZ_P:
		return (u->opr_mode == UD_MODE16) ? UD_SZ_WP : UD_SZ_DP;
	case UD_SZ_X:
		return (u->dis_mode == UD_MODE64 && P_REX_W(u->pfx_rex)) ?
			UD_SZ_QWORD : UD_SZ_DWORD;
	case UD_SZ_M:
		return (u->opr_mode == UD_MODE64) ? UD_SZ_QWORD : UD_SZ_DWORD;
	default:
		return size;
  }
}

/* -----------------------------------------------------------------------------
 * decode_a()- Decodes operands of the type seg:offset
 * -----------------------------------------------------------------------------
 */
static inline void 
decode_a( register struct ud* u, enum ud_type size, struct ud_operand *op )
{
  /* seg16:off16 */
  if (u->opr_mode == UD_MODE16) {	
	op->type = UD_OP_PTR32;
	op->size = UD_SZ_WORD;
	op->lval.ptr.seg = inp_uint16(u);
	op->lval.ptr.off = inp_uint16(u);
  } 
  /* seg16:off32 */
  else {
	op->type = UD_OP_PTR48;
	op->size = UD_SZ_DWORD;
	op->lval.ptr.seg = inp_uint16(u);
	op->lval.ptr.off = inp_uint32(u);
  }
}

/* -----------------------------------------------------------------------------
 * decode_gpr() - Returns decoded General Purpose Register 
 * -----------------------------------------------------------------------------
 */
static inline ud_type_t
decode_gpr(register struct ud* u, enum ud_type size, unsigned char rm)
{
  size = resolve_operand_size(u, size);
		
  switch (size) {
	case UD_SZ_QWORD:  return UD_R_RAX + rm;
	case UD_SZ_DWORD:  return UD_R_EAX + rm;
	case UD_SZ_WORD:   return UD_R_AX  + rm;
	case UD_SZ_BYTE:
		if (u->dis_mode == UD_MODE64) {
			if (rm >= 4)
				return UD_R_SPL + (rm-4);
			return UD_R_AL + rm;
		}
		return UD_R_AL + rm;
	default:
		return 0;
  }
}

/* -----------------------------------------------------------------------------
 * resolve_gpr64() - Resolves any ambiguity in register (including 64bit) 
 * selection. 
 * -----------------------------------------------------------------------------
 */
static inline ud_type_t
resolve_gpr64( register struct ud* u, enum map_operand_type gpr_op )
{
  if (gpr_op >= OP_rAXr8 && gpr_op <= OP_rDIr15)
	gpr_op = (gpr_op - OP_rAXr8) | (P_REX_B(u->pfx_rex) << 3);			
  else	gpr_op = (gpr_op - OP_rAX);

  if (u->opr_mode == UD_MODE16)
	return gpr_op + UD_R_AX;
  if (u->dis_mode == UD_MODE32 || 
	(u->opr_mode == UD_MODE32 && ! (P_REX_W(u->pfx_rex) || u->default64)))
	return gpr_op + UD_R_EAX;

  return gpr_op + UD_R_RAX;
}

/* -----------------------------------------------------------------------------
 * resolve_gpr32 () - Resolves any ambiguity in register (excluding 64bit) 
 * selection. 
 * -----------------------------------------------------------------------------
 */
static ud_type_t
resolve_gpr32( register struct ud* u, enum map_operand_type gpr_op )
{
  gpr_op = gpr_op - OP_eAX;

  if (u->opr_mode == UD_MODE16) 
	return gpr_op + UD_R_AX;

  return gpr_op +  UD_R_EAX;
}

/* -----------------------------------------------------------------------------
 * resolve_reg() - Auxiliary function for Resolving the register type and 
 * returning the corresponding reg value.
 * -----------------------------------------------------------------------------
 */
static inline ud_type_t
resolve_reg( register struct ud* u, unsigned int type, unsigned char rm )
{
  switch (type) {
	case T_MMX :	return UD_R_MM0  + (rm & 7);
	case T_XMM :	return UD_R_XMM0 + rm;
	case T_CRG :	return UD_R_CR0  + rm;
	case T_DBG :	return UD_R_DR0  + rm;
	case T_SEG :	return UD_R_ES   + rm;
	case T_NONE:
	default:	return UD_NONE;
  }
}

/* -----------------------------------------------------------------------------
 * decode_imm() - Decodes Immediate values.
 * -----------------------------------------------------------------------------
 */
static void 
decode_imm( register struct ud* u, enum ud_type size, struct ud_operand *op )
{
  op->size = resolve_operand_size(u, size);

  switch (op->size) {
	case UD_SZ_BYTE:
	case UD_SZ_SBYTE:
		op->type = UD_OP_IMM8;
		op->lval.sbyte = inp_uint8(u);
		break;
	case UD_SZ_WORD:
		op->type = UD_OP_IMM16;
		op->lval.uword = inp_uint16(u);
		break;
	case UD_SZ_DWORD:
		op->type = UD_OP_IMM32;
		op->lval.udword = inp_uint32(u);
		break;
	case UD_SZ_QWORD:
		op->type = UD_OP_IMM64;
		op->lval.uqword = inp_uint64(u);
		break;
	default:
		return;
  }
}

/* -----------------------------------------------------------------------------
 * decode_modrm() - Decodes ModRM Byte
 * -----------------------------------------------------------------------------
 */
static void 
decode_modrm (
	register struct ud* u,
	struct ud_operand *op,	  /* ptr to instruction operand struct */
	enum ud_type size,	  /* operand size */
	unsigned char rm_type,	  /* the type of operand specified by rm, if mod = 11b */
	struct ud_operand *opreg, /* ptr to instruction operand struct for reg operand */
	unsigned char reg_size,	  /* the size of operand specified by reg */
	unsigned char reg_type	  /* the type of operand specified by reg */
	)
{
  unsigned char mod, rm, reg;

  inp_next(u);

  /* get mod, r/m and reg fields */
  mod = MODRM_MOD(inp_curr(u));
  rm  = (P_REX_B(u->pfx_rex) << 3) | MODRM_RM(inp_curr(u));
  reg = (P_REX_R(u->pfx_rex) << 3) | MODRM_REG(inp_curr(u));

  op->size = resolve_operand_size(u, size);

  /* if mod is 11b, then the UD_R_m specifies a gpr/mmx/sse/control/debug */
  if (mod == 3) {
	op->type = UD_OP_REG;
	if (rm_type == 	T_GPR)
		op->base = decode_gpr(u, op->size, rm);
	else op->base = resolve_reg(u, rm_type, rm);	
  } 
  /* else its memory addressing */  
  else { 

	op->type = UD_OP_MEM;

	if (u->adr_mode == UD_MODE64) {

		/* get base */
		op->base = UD_R_RAX + rm;

		/* get offset type */
		if (mod == 1)
			op->offset = 8;
		else if (mod == 2)
			op->offset = 32;
		else if (mod == 0 && rm == 5) {			
			op->base = UD_R_RIP;
			op->offset = 32;
		}

		/* Scale-Index-Base (SIB) */
		if ((rm & 7) == 4) {
			inp_next(u);

			op->type  = UD_OP_RSIB;
			op->scale = (1 << SIB_S(inp_curr(u))) & ~1;
			op->index = UD_R_RAX + (SIB_I(inp_curr(u)) | (P_REX_X(u->pfx_rex) << 3));
			op->base  = UD_R_RAX + (SIB_B(inp_curr(u)) | (P_REX_B(u->pfx_rex) << 3));

			/* special conditions for base reference */
			if (op->index == UD_R_RSP) {
				op->index = UD_NONE;
				op->scale = UD_NONE;
				op->type  = UD_OP_MEM;
			}

			if (op->base == UD_R_RBP || op->base == UD_R_R13) {
				if (mod == 0) {	
					op->base = UD_NONE;
					if (! op->index) {
						op->type = (mod == 1) ?
							UD_OP_OFF8 : UD_OP_OFF32;
					} else  op->type  = UD_OP_RSID;
				}
				if (mod == 1)
					op->offset = 8;
				else op->offset = 32;
			}
		}
	} 
	/* 32-Bit addressing mode */
	else if (u->adr_mode == UD_MODE32) {

		/* get base */
		op->base = UD_R_EAX + rm;

		/* get offset type */
		if (mod == 1)
			op->offset = 8;
		else if (mod == 2)
			op->offset = 32;
		else if (mod == 0 && rm == 5) {
			op->type = UD_OP_OFF32;
			op->offset = 32;
		}

		/* Scale-Index-Base (SIB) */
		if (rm == 4) {
			inp_next(u);

			op->type  = UD_OP_SIB;
			op->scale = (1 << SIB_S(inp_curr(u))) & ~1;
			op->index = UD_R_EAX + SIB_I(inp_curr(u));
			op->base  = UD_R_EAX + SIB_B(inp_curr(u));

			/* special condition for base reference */
			if (op->index == UD_R_ESP) {
				op->index = UD_NONE;
				op->scale = UD_NONE;
				op->type  = UD_OP_MEM;
			}

			if (op->base == UD_R_EBP) {
				if (mod == 0) {
					op->base = UD_NONE;
					if (! op->index) {
						op->type = (mod == 1) ?
							UD_OP_OFF8 : UD_OP_OFF32;
					} else op->type  = UD_OP_SID;
				}
				if (mod == 1)
					op->offset = 8;
				else op->offset = 32;
			}
		}
	} 
	else  { /* 16bit addressing mode */
		op->type = UD_OP_MEM;

		switch (rm) {
			case 0:	op->base = UD_R_BX; op->index = UD_R_SI; break;
			case 1: op->base = UD_R_BX; op->index = UD_R_DI; break;
			case 2: op->base = UD_R_BP; op->index = UD_R_SI; break;
			case 3: op->base = UD_R_BP; op->index = UD_R_DI; break;
			case 4:	op->base = UD_R_SI; break;
			case 5: op->base = UD_R_DI; break;
			case 6: op->base = UD_R_BP; break;
			case 7: op->base = UD_R_BX; break;
		}

		if (mod == 0 && rm == 6) {
			op->type  = UD_OP_OFF16;
			op->offset= 16;
		} else if (mod == 1)
			op->offset = 8;
		else if (mod == 2) 
			op->offset = 16;
	}
  }  

  /* extract offset, if any */
  switch(op->offset) {
	case 8 : op->lval.ubyte  = inp_uint8(u);  break;
	case 16: op->lval.uword  = inp_uint16(u); break;
	case 32: op->lval.udword = inp_uint32(u); break;
	case 64: op->lval.uqword = inp_uint64(u); break;
	default: break;
  }

  /* resolve register encoded in reg field */
  if (opreg) {
	opreg->size = resolve_operand_size(u, reg_size);
	opreg->type = UD_OP_REG;
	if (reg_type == T_GPR)
		opreg->base = decode_gpr(u, opreg->size, reg);
	else opreg->base = resolve_reg(u, reg_type, reg);
  }
}

/* -----------------------------------------------------------------------------
 * decode_o() - Decodes offset
 * -----------------------------------------------------------------------------
 */
static void 
decode_o( register struct ud* u, enum ud_type size, struct ud_operand *op )
{
  switch (u->adr_mode) {
	case UD_MODE64:
		op->lval.uqword = inp_uint64(u);
		op->type = UD_OP_OFF64;
		break;
	case UD_MODE32:
		op->lval.udword = inp_uint32(u);
		op->type = UD_OP_OFF32;
		break;
	case UD_MODE16:
		op->lval.uword = inp_uint16(u);
		op->type = UD_OP_OFF16;
		break;
	default:
		return;
  }

  op->size = resolve_operand_size(u, size);
}

/* -----------------------------------------------------------------------------
 * disasm_operands() - Disassembles Operands.
 * -----------------------------------------------------------------------------
 */
static void 
disasm_operands(register struct ud* u)
{
  /* mopXt = map entry, operand X, type; mopXs = map entry, operand X, size */
  enum map_operand_type mop1t = u->opcmap_entry->operand1.type;
  enum map_operand_type mop2t = u->opcmap_entry->operand2.type;
  enum map_operand_type mop3t = u->opcmap_entry->operand3.type;

  enum ud_type mop1s = u->opcmap_entry->operand1.size;
  enum ud_type mop2s = u->opcmap_entry->operand2.size;
  enum ud_type mop3s = u->opcmap_entry->operand3.size;

  /* iop = instruction operand */
  struct ud_operand* iop = u->operand;

  switch(mop1t) {
	case OP_A :
		decode_a(u, mop1s, &(iop[0]));
		break;
	/* M[b] */
	case OP_M :
		if (MODRM_MOD(inp_peek(u)) == 3)
			u->error= 1;

	/* E, G/P/V/I/CL/1/S */
	case OP_E :
		if (mop2t == OP_G)	/* E, G */ {
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_GPR);
		}
		else if (mop2t == OP_P)	/* E, P */
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_MMX);
		else if (mop2t == OP_V)	/* E, V */
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_XMM);
		else if (mop2t == OP_S) /* E, S */
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_SEG);
		else {	/* E */
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, NULL, 0, T_NONE);
			if (mop2t == OP_CL) {	/* E, CL */
				iop[1].type = UD_OP_REG;
				iop[1].base = UD_R_CL;
				iop[1].size = UD_SZ_BYTE;
			} else if (mop2t == OP_I1) {	/* E, 1 */
				iop[1].type = UD_OP_CONST;
				u->operand[1].lval.udword = 1;
			} else if (mop2t == OP_I) {	/* E, I */
				decode_imm(u, mop2s, &(iop[1]));
			}
		}
		break;

	/* G, E/PR[,I]/VR */
	case OP_G :
		if (mop2t == OP_E) {	/* G, E */
			decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_GPR);
			if (mop3t == OP_I) /* G, E, I */
				decode_imm(u, mop3s, &(iop[2]));
		}
		else if (mop2t == OP_PR) {	/* G, PR */
			/* the mod field ought to be zero */
			if (MODRM_RM(inp_peek(u)) != 3)
				u->error = 1;
			decode_modrm(u, &(iop[1]), mop2s, T_MMX, &(iop[0]), mop1s, T_GPR);
			if (mop3t == OP_I) /* G, PR, I */
				decode_imm(u, mop3s, &(iop[2]));
		} else if (mop2t == OP_VR) {	/* G, VR */
			/* the mod field ought to be zero */
			if (MODRM_RM(inp_peek(u)) != 3)
				u->error= 1;
			decode_modrm(u, &(iop[1]), mop2s, T_XMM, &(iop[0]), mop1s, T_GPR);
		}
		break;

	/* AL..BH, I/O/DX */
	case OP_AL : case OP_CL : case OP_DL : case OP_BL :
	case OP_AH : case OP_CH : case OP_DH : case OP_BH :
		iop[0].type = UD_OP_REG;
		iop[0].base = UD_R_AL + (mop1t - OP_AL);
		iop[0].size = UD_SZ_BYTE;
		if (mop2t == OP_I)	/* AL..BH, Ib */
			decode_imm(u, mop2s, &(iop[1]));
		else if (mop2t == OP_DX) {	/* AL..BH, DX */
			iop[1].type = UD_OP_REG;
			iop[1].base = UD_R_DX;
		}
		else if (mop2t == OP_O)	/* AL..BH, O */
			decode_o(u, mop2s, &(iop[1]));
		break;

	/* rAX[r8]..rDI[r15], I/rAX..rDI/O */
	case OP_rAXr8 : case OP_rCXr9 : case OP_rDXr10 : case OP_rBXr11 :
	case OP_rSPr12: case OP_rBPr13: case OP_rSIr14 : case OP_rDIr15 :
	case OP_rAX : case OP_rCX : case OP_rDX : case OP_rBX :
	case OP_rSP : case OP_rBP : case OP_rSI : case OP_rDI :

		iop[0].type = UD_OP_REG;
		iop[0].base = resolve_gpr64(u, mop1t);

		if (mop2t == OP_I)
			decode_imm(u, mop2s, &(iop[1]));
		else if (mop2t >= OP_rAX && mop2t <= OP_rDI) {
			iop[1].type = UD_OP_REG;
			iop[1].base = resolve_gpr64(u, mop2t);
		}
		else if (mop2t == OP_O) {
			decode_o(u, mop2s, &(iop[1]));	
			iop[0].size = resolve_operand_size(u, mop2s);
		}
		break;

	/* AL[r8b]..BH[r15b], I */
	case OP_ALr8b : case OP_CLr9b : case OP_DLr10b : case OP_BLr11b :
	case OP_AHr12b: case OP_CHr13b: case OP_DHr14b : case OP_BHr15b :
	{
		ud_type_t gpr = (mop1t - OP_ALr8b) + UD_R_AL + 
						(P_REX_B(u->pfx_rex) << 3);
		if (UD_R_AH <= gpr && u->pfx_rex)
			gpr = gpr + 4;
		iop[0].type = UD_OP_REG;	
		iop[0].base = gpr;
		if (mop2t == OP_I)
			decode_imm(u, mop2s, &(iop[1]));
		break;
	}

	/* eAX..eDX, DX/I */
	case OP_eAX : case OP_eCX : case OP_eDX : case OP_eBX :
	case OP_eSP : case OP_eBP : case OP_eSI : case OP_eDI :
		iop[0].type = UD_OP_REG;
		iop[0].base = resolve_gpr32(u, mop1t);
		if (mop2t == OP_DX) {	/* eAX..eDX, DX */
			iop[1].type = UD_OP_REG;
			iop[1].base = UD_R_DX;
		} else if (mop2t == OP_I)	/* eAX..eDX, I */
			decode_imm(u, mop2s, &(iop[1]));
		break;

	/* ES..GS */
	case OP_ES : case OP_CS : case OP_DS :
	case OP_SS : case OP_FS : case OP_GS :
		/* in 64bits mode, only fs and gs are allowed */
		if (u->dis_mode == UD_MODE64)
			if (mop1t != OP_FS && mop1t != OP_GS)
				u->error= 1;
		iop[0].type = UD_OP_REG;	
		iop[0].base = (mop1t - OP_ES) + UD_R_ES;
		break;

	/* J */
	case OP_J :
		decode_imm(u, mop1s, &(iop[0]));
		iop[0].type = UD_OP_JIMM;		
		switch(iop[0].size) {
			case UD_SZ_BYTE : iop[0].size = UD_SZ_JBYTE; break;
			case UD_SZ_WORD : iop[0].size = UD_SZ_JWORD; break;
			case UD_SZ_DWORD : iop[0].size = UD_SZ_JDWORD; break;
			default: break;
		}
		break ;

	/* P, Q/W/E[,I] */
	case OP_P :
		if (mop2t == OP_Q)	/* P, Q */
			decode_modrm(u, &(iop[1]), mop2s, T_MMX, &(iop[0]), mop1s, T_MMX);
		else if (mop2t == OP_W)	/* P, W */
			decode_modrm(u, &(iop[1]), mop2s, T_XMM, &(iop[0]), mop1s, T_MMX);
		else if (mop2t == OP_E) {	/* P, E */
			decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_MMX);
			if (mop3t == OP_I)	/* P, E, I */
				decode_imm(u, mop3s, &(iop[2]));
		}
		break;

	/* R, C/D */
	case OP_R :
		if (mop2t == OP_C)	/* R, C */
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_CRG);
		else
		if (mop2t == OP_D)	/* R, D */
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_DBG);
		break;

	/* C, R */
	case OP_C :
		decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_CRG);
		break;

	/* D, R */
	case OP_D :
		decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_DBG);
		break;

	/* Q, P */
	case OP_Q :
		decode_modrm(u, &(iop[0]), mop1s, T_MMX, &(iop[1]), mop2s, T_MMX);
		break;

	/* S, E */
	case OP_S :
		decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_SEG);
		break;

	/* W, V */
	case OP_W :
		decode_modrm(u, &(iop[0]), mop1s, T_XMM, &(iop[1]), mop2s, T_XMM);
		break;

	/* V, W[,I]/Q */
	case OP_V :
		if (mop2t == OP_W) {	/* V, W */
			decode_modrm(u, &(iop[1]), mop2s, T_XMM, &(iop[0]), mop1s, T_XMM);
			if (mop3t == OP_I)	/* V, W, I */
				decode_imm(u, mop3s, &(iop[2]));
		} else if (mop2t == OP_Q)	/* V, Q */
			decode_modrm(u, &(iop[1]), mop2s, T_MMX, &(iop[0]), mop1s, T_XMM);
		break;

	/* DX, eAX/AL */
	case OP_DX :
		iop[0].type = UD_OP_REG;
		iop[0].base = UD_R_DX;
		if (mop2t == OP_eAX) {	/* DX, eAX */
			iop[1].type = UD_OP_REG;	
			iop[1].base = resolve_gpr32(u, mop2t);
		} else if (mop2t == OP_AL) {	/* DX, AL */
			iop[1].type = UD_OP_REG;	
			iop[1].base = UD_R_AL;
		}
		break;

	/* I, I/AL/eAX */
	case OP_I :
		decode_imm(u, mop1s, &(iop[0]));
		if (mop2t == OP_I)	/* I, I */
			decode_imm(u, mop2s, &(iop[1]));
		else if (mop2t == OP_AL) {	/* I, AL */
			iop[1].type = UD_OP_REG;	
			iop[1].base = UD_R_AL;
		} else if (mop2t == OP_eAX) {	/* I, eAX */
			iop[1].type = UD_OP_REG;	
			iop[1].base = resolve_gpr32(u, mop2t);
		}
		break;

	/* O, AL/eAX */
	case OP_O :
		decode_o(u, mop1s, &(iop[0]));
		iop[1].type = UD_OP_REG;
		iop[1].size = resolve_operand_size(u, mop1s);
		if (mop2t == OP_AL)
			iop[1].base = UD_R_AL;
		else if (mop2t == OP_eAX)
			iop[1].base = resolve_gpr32(u, mop2t);
		else if (mop2t == OP_rAX)
			iop[1].base = resolve_gpr64(u, mop2t);		
		break;

	/* 3 */
	case OP_I3 :
		iop[1].type = UD_OP_CONST;
		iop[1].lval.sbyte = 3;
		break;
		
	case OP_ST0 : case OP_ST1 : case OP_ST2 : case OP_ST3 :
	case OP_ST4 : case OP_ST5 : case OP_ST6 : case OP_ST7 :

		iop[0].type = UD_OP_REG;
		iop[0].base = (mop1t-OP_ST0)  + UD_R_ST0;
		if (mop2t >= OP_ST0 && mop2t <= OP_ST7) {
			iop[1].type = UD_OP_REG;
			iop[1].base = (mop2t-OP_ST0) + UD_R_ST0;
		}
		break;

	/* none */
	default :
		iop[0].type = iop[1].type = iop[2].type = UD_NONE;
  }
}

/* -----------------------------------------------------------------------------
 * ud_clear_instruction_pointer() - clear instruction pointer 
 * -----------------------------------------------------------------------------
 */
static void inline clear_insn(struct ud* u)
{
  u->error = 0;
  u->pfx_seg = 0;
  u->pfx_opr = 0;
  u->pfx_adr = 0;
  u->pfx_lock= 0;
  u->pfx_repne=0;
  u->pfx_rep = 0;
  u->pfx_seg = 0;
  u->pfx_rex = 0;
  u->mnemonic = 0;

  memset(&u->operand[0], 0, sizeof(u->operand[0]));
  memset(&u->operand[1], 0, sizeof(u->operand[1]));
  memset(&u->operand[2], 0, sizeof(u->operand[2]));

  u->opcmap_entry = NULL;
}

/* -----------------------------------------------------------------------------
 * ud_extract_prefixes() - extract instruction prefixes
 * -----------------------------------------------------------------------------
 */
static int extract_prefixes( register struct ud* u ) 
{
  int prefixflag = 0;	/* (bool) denotes end of prefixes */
  int i;		/* prefix counter */

  inp_next(u);
	
  /* search for prefixes */
  for (i = 0; prefixflag == 0; ++i) {	
	/* check for rex bit in 64 bits mode */		
	if (0x40 <= inp_curr(u) && inp_curr(u) <= 0x4F) {
		if (u->dis_mode == UD_MODE64) {
			u->pfx_rex = inp_curr(u);
			/* move to next byte */
			inp_next(u); 
		} else prefixflag = 1;
	} else switch(inp_curr(u)) {
		case 0x2E : u->pfx_seg = UD_R_CS; inp_next(u); break;
		case 0x36 : u->pfx_seg = UD_R_SS; inp_next(u); break;
		case 0x3E : u->pfx_seg = UD_R_DS; inp_next(u); break;
		case 0x26 : u->pfx_seg = UD_R_ES; inp_next(u); break;
		case 0x64 : u->pfx_seg = UD_R_FS; inp_next(u); break;
		case 0x65 : u->pfx_seg = UD_R_GS; inp_next(u); break;
		case 0x66 : u->pfx_opr = 0x66; inp_next(u); break;
		case 0x67 : u->pfx_adr = 0x67; inp_next(u); break;
		case 0xF0 : u->pfx_lock= 0xF0; inp_next(u); break;
		case 0xF2 : u->pfx_repne=0xF2; inp_next(u); break;
		case 0xF3 : u->pfx_rep = 0xF3; inp_next(u); break;
		default   : prefixflag = 1;
	}

	/* if > MAX_PREFIXES, disintegrate */
	if (i >= MAX_PREFIXES) {
		u->error= 1;
		break;	/* break from loop */
	}
  }

  /* rewind stream */
  inp_back(u);

  /* return prefix count */
  return i;
}

/* =============================================================================
 * ud_decode() - Instruction decoder. Returns the number of bytes decoded.
 * =============================================================================
 */
extern unsigned int ud_decode( register struct ud* u )
{
  char* src_hex, *src_ptr;
  unsigned int i;

  /* init modes */
  u->opr_mode = u->adr_mode = u->dis_mode;

  inp_start(u);
  src_ptr = (char*) u->inp_sess;
  clear_insn(u);
  extract_prefixes(u);
  ud_search_map(u);

  /* assign instruction mnemonic code */
  u->mnemonic = u->opcmap_entry->mnemonic;

  /* if invalid set error */
  u->error = (u->mnemonic == UD_Iinvalid);

  /* rex */
  u->pfx_rex = u->pfx_rex & P_REX_MASK(u->opcmap_entry->prefix); 

  /* set the default 64 flag */
  u->default64 = P_DEF64(u->opcmap_entry->prefix); 

  /* set the suffix flag */
  u->sfx = P_SUFF(u->opcmap_entry->prefix);

  /* check if the instruction is invalid in 64 bits mode */
  u->error = (u->dis_mode == UD_MODE64) && P_INV64(u->opcmap_entry->prefix);

  /* set operand mode */
  if (u->pfx_opr) {
	if (u->dis_mode == UD_MODE64)
		u->opr_mode = UD_MODE16;
	else u->opr_mode = (u->opr_mode == UD_MODE32) ? UD_MODE16 : UD_MODE32 ;
  }

  /* set address mode */
  if (u->pfx_adr) {
	if (u->dis_mode == UD_MODE64)
		u->adr_mode = UD_MODE32;
	else	u->adr_mode = (u->adr_mode == UD_MODE32) ? UD_MODE16 : UD_MODE32 ;
  }

  /* check if the instruction is dependent on the mode */
  u->error = P_DEP32(u->opcmap_entry->prefix) && (u->opr_mode == UD_MODE16);

  disasm_operands(u);

  /* get suffix size */
  if (u->sfx)
	u->operand[0].size = resolve_operand_size(u, UD_SZ_V);

  /* if the operand should be cast */
  if (P_CAST(u->opcmap_entry->prefix))
	u->cast = 1;

  /* check if P_O32 prefix was used */
  if (P_O32(u->opcmap_entry->prefix) && u->pfx_opr)
	u->pfx_opr = 0;

  /* check if P_A32 prefix was used */
  if (P_A32(u->opcmap_entry->prefix) && u->pfx_adr)
	u->pfx_adr = 0;
	
  /* if its a 3dnow! instruction, get mnemonic code based on the suffix */
  if (u->mnemonic == UD_I3dnow)
	u->mnemonic = ud_map_get_3dnow(inp_curr(u));

  /* check for u->error*/
  if (u->error) {
	clear_insn(u);
	inp_reset(u); 

	u->mnemonic = UD_Idb;
	u->operand[0].type = UD_OP_IMM8;
	u->operand[0].size = 0;
	u->operand[0].lval.sbyte = inp_uint8(u);
  } 

  /* update offset */
  u->asm_offset = u->pc;
  u->asm_fill = 0;  

  /* compose hex code */
  src_hex = (char*) u->asm_hexcode;

  for (i = 0; i < u->inp_ctr; ++i, ++src_ptr) {
	sprintf(src_hex, "%02x", *src_ptr & 0xFF);
	src_hex += 2;
  }

  /* Update program counter */
  u->pc += u->inp_ctr;

  return u->inp_ctr;
}
