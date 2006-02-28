/* decode.c
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 *
 * $Id$
 * $Log$
 * Revision 1.4  2006/02/28 18:07:13  vivek5797
 * More bugs eliminated.
 *
 * Revision 1.3  2006/02/20 19:44:29  vivek5797
 * Fixed bug in decode.c, opcmap.c
 *
 * Revision 1.2  2006/02/20 17:48:19  vivek5797
 * Fixed bug in opcmap.c
 *
 * Revision 1.1.1.1  2006/02/04 09:14:10  vivek5797
 * Initial import of beta, after a major overhaul.
 *
 * Revision 1.1  2004/12/25 14:41:31  vivek
 * Initial revision
 *
 */

#include <stdlib.h>
#include "types.h"
#include "macros.h"
#include "extern.h"
#include "mnemonics.h"
#include "opcmap.h"
#include "source.h"

/* the max nr of prefixes */
#define MAX_PREFIXES	15

/* register types */
#define T_NONE	0
#define T_GPR	1
#define T_MMX	2
#define T_CRG	3
#define T_DBG	4
#define T_SEG	5
#define T_XMM	6

/* resolve_operand_size()- Resolves the size of operand depending on
 * the current disassembly mode and the prefixes.
 */
static uint8_t resolve_operand_size(register ud_t *ud, ud_operand_size_t size)
{
	if (size == SZ_vw && ud->dis_mode == MODE64)
		return (ud->prefix.opr) ? SZ_w : SZ_q;
	else if (size == SZ_v  && ud->dis_mode == MODE64) {
		if (P_REX_W(ud->prefix.rex))
			return SZ_q;
		else return (ud->prefix.opr) ? SZ_w : SZ_d;
 	}
	else if (size == SZ_v || size == SZ_vw || size == SZ_z)
		return (ud->opr_mode == MODE16) ? SZ_w : SZ_d;
	else if (size == SZ_p)
		return (ud->opr_mode == MODE16) ? SZ_wp : SZ_dp;
	else return size;
}

/* decode_a()- Decodes operands of the type seg:offset
 *
 */
static void decode_a(register ud_t* ud, ud_operand_size_t size, ud_operand_t *op)
{
	if (ud->opr_mode == MODE16) { /* seg16:off16 */
		op->type = P_32;
		op->size = SZ_w;
		op->lval.ptr.seg = src_uint16(ud);
		op->lval.ptr.off = src_uint16(ud);
  	} else { 
		/* seg16:off32 */
		op->type = P_48;
		op->size = SZ_d;
		op->lval.ptr.seg = src_uint16(ud);
		op->lval.ptr.off = src_uint32(ud);
	}
}

/* returns decoded GPR */
static ud_operand_type_t decode_gpr(register ud_t* ud, ud_operand_size_t size, uint8_t rm)
{
	if (size == SZ_m) {
		/* Mode dependent, operand prefix independent, size specifier: SZ_m. In
		 * 64-bits mode, the GPR is 64bits in all other cases its 32 bits.
		 */
		return (ud->dis_mode == MODE64) ? R_RAX + rm : R_EAX + rm;
	}
	if (size == SZ_b && ud->dis_mode == MODE64) {
		/* byte sized operand, in 64 bit mode. In this mode, 16 different 8-bit
		 * registers can be chosen. is it > bl ? if so, then the next register
		 * will be sil instead of ah (in 64 bits)
		 */
		if (rm >= 4)
			return R_SPL + (rm-4);
		else return R_AL + rm;
	} else if (size == SZ_b) {
		return R_AL + rm;
	} else if ((size == SZ_x || size == SZ_v) && ud->dis_mode == MODE64) {
		/* word/dword/qword sized operands, in 64 bit mode. In this mode,
 		 * 16 different 16 bit registers can be chosen. if REX.W is specified
		 * then select a quad register
		 */
		if (P_REX_W(ud->prefix.rex))
			return R_RAX + rm;
		else if (ud->prefix.opr)
			/* in 64 bit mode, 0x66 means a 16 bit register */
			return R_AX  + rm;
		else	/* default, 32 bit register */ {
			return R_EAX + rm;
		}
	} else if ((size == SZ_d || size == SZ_x) || 
			(size == SZ_v && ud->opr_mode == MODE32)) {
		/* if mode is 32its then select 32bit registers */
		return R_EAX + rm;
	} else if ((size == SZ_w) || (size == SZ_v && ud->opr_mode == MODE16)) {
		/* if mode is 16bits then select 16bit registers */
		return R_AX + rm;
	} else	return 0;
}

/* resolve_gpr64() - Resolves any ambiguity in register(including 64bit) selection. */
static ud_operand_type_t resolve_gpr64(register ud_t* ud, ud_opcmap_operand_type_t gpr_op)
{
	ud_operand_type_t delta = R_RAX;

	if (gpr_op >= OP_rAXr8 && gpr_op <= OP_rDIr15)
		gpr_op = (gpr_op - OP_rAXr8) | (P_REX_B(ud->prefix.rex) << 3);			
	else	gpr_op = (gpr_op - OP_rAX);

	if ((ud->dis_mode == MODE64 && ud->prefix.opr) || 
			ud->opr_mode == MODE16)
		delta = R_AX;
	else if (ud->dis_mode == MODE32 || 
		  (ud->opr_mode == MODE32 && 
		    ! (P_REX_W(ud->prefix.rex) || ud->default64)))
		delta = R_EAX;

	return gpr_op + delta;

}

/* resolve_gpr32 () - Resolves any ambiguity in register(excluding 64bit) selection. */
static ud_operand_type_t resolve_gpr32(register ud_t* ud, ud_opcmap_operand_type_t gpr_op)
{
	ud_operand_type_t delta= 0;

	gpr_op = gpr_op - OP_eAX;

	if ((ud->dis_mode == MODE64 && ud->prefix.opr) || 
			ud->opr_mode == MODE16)
		delta = R_AX;
	else	delta = R_EAX;

	return gpr_op + delta;	
}

/* resolve_reg() -  Auxiliary function for Resolving the register type and returning
 * the corresponding reg value.
 */
static ud_operand_type_t resolve_reg(register ud_t* ud, unsigned int type, uint8_t rm)
{
	switch (type) {
		case T_MMX :	/* mmx register */
			return R_MM0 + rm;
		case T_XMM :	/*xmm register */
			return R_XMM0 + rm;
		case T_CRG :	/* control register */
			return R_CR0 + rm;
		case T_DBG :	/* debug register */
			return R_DR0 + rm;
		case T_SEG :	/* debug register */
			return R_ES + rm;
		case T_NONE:
		default:
			return 0;
  	}
}

/* decode_imm() - Decodes Immediate values
 */
static void decode_imm(register ud_t* ud, ud_operand_size_t size, ud_operand_t *op)
{
	/* byte sized immediate */
	if (size == SZ_b || size == SZ_sb) {
		op->type = I_BYTE;
		op->lval.bits8 = src_uint8(ud);
	}
	/* if disassembler mode is 64 bits, then a 0x66 prefix means
	 * a 16 bit immediate value for SZ_v
	 */
	if(size == SZ_z && ud->dis_mode == MODE64) {
		if (ud->prefix.opr) {
			/* if prefixed by 0x66, then a 16bit immediate */
			op->type = I_WORD;
			op->lval.bits16 = src_uint16(ud);
		} else {
			/* else its dword */ 
			op->type = I_DWRD;
			op->lval.bits32 = src_uint32(ud);
		}
	} else if (size == SZ_w || (size == SZ_z && ud->opr_mode == MODE16)) {
		/* if SZ_w or in 16bits operand mode */
		op->type = I_WORD;
		op->lval.bits16 = src_uint16(ud);
	} else if (size == SZ_d || (size == SZ_z && ud->opr_mode == MODE32)) {
		/* if SZ_d or in 32bits operand mode */
		op->type = I_DWRD;
		op->lval.bits32 = src_uint32(ud);
	}
	/* get size of operand */
	op->size = resolve_operand_size(ud, size);
}

/* decode_sib32() - decodes 32 bit SIB byte */
static void decode_sib32(register ud_t* ud, ud_operand_t* op, uint8_t mod) 
{
	/* move to next byte in source */
	src_next(ud);

	/* resolve scale-index-base */
	op->sib.scale = SIB_S(src_curr(ud));
	op->sib.index = SIB_I(src_curr(ud));
	op->sib.base  = SIB_B(src_curr(ud));

	/* if SIB INDEX is 100b(4), BASE+DISP only. */
	if (op->sib.index == 4) {
		/* a special case for ESP */
		if (op->sib.base == 4) {
			op->type = mod + M_ESP;
		} else {
			op->type = (mod * 8) + M_EAX + op->sib.base;
		}
	}
	/* if lower 3 bits of base = 5, then its SCALED-INDEX only */
	else if (op->sib.base == 5 && op->type == M_SIB) {
		op->type = M_SID;
	}
}

/* decode_sib64() - decodes 64 bit SIB byte */
static void decode_sib64(register ud_t* ud, ud_operand_t* op, uint8_t mod) 
{
	/* move to next byte in source */
	src_next(ud);

	/* resolve scale-index-base */
	op->sib.scale = SIB_S(src_curr(ud));
	op->sib.index = SIB_I(src_curr(ud)) + P_REX_X(ud->prefix.rex) * 8;
	op->sib.base  = SIB_B(src_curr(ud)) + P_REX_B(ud->prefix.rex) * 8;

	/* if SIB INDEX is 100b(4), BASE+DISP only. */
	if (op->sib.index == 4) {
		/* a special case for RSP */
		if (op->sib.base == 4) {
			op->type = mod + M_RSP;
		} else {
			op->type = (mod * 8) + M_RAX + op->sib.base;
		}
	}
	/* if lower 3 bits of base = 5, then its SCALED-INDEX only */
	else if (op->sib.base == 5 && op->type == M_RSIB) {
		op->type = M_RSID;
	}
}

/* decode_modrm(...) - Decodes ModRM */
static void decode_modrm (
	register ud_t* ud,
	ud_operand_t *op,	/* ptr to instruction operand struct */
	ud_operand_size_t size,	/* operand size */
	uint8_t rm_type,	/* the type of operand specified by rm, if mod = 11b */
	ud_operand_t *opreg,	/* ptr to instruction operand struct for reg operand */
	uint8_t reg_size,	/* the size of operand specified by reg */
	uint8_t reg_type	/* the type of operand specified by reg */
	)
{
	/* load byte from source */
	src_next(ud);

	/* get mod, r/m and reg fields */
	uint8_t mod = MODRM_MOD(src_curr(ud));
	uint8_t rm  = (P_REX_B(ud->prefix.rex) << 3) | MODRM_RM(src_curr(ud));
	uint8_t reg = (P_REX_R(ud->prefix.rex) << 3) | MODRM_REG(src_curr(ud));

	/* if mod is 11b, then the r_m field specifies a general
	 * purpose register or mmx/sse/control/debug register
	 */
	if (mod == 3) {

		/* determine the operand sizes */
	  	op->size = resolve_operand_size(ud, size);

		if (rm_type == 	T_GPR)	/* general purpose register */
			op->type = decode_gpr(ud, op->size, rm);
		else	op->type = resolve_reg(ud, rm_type, rm);
	} else {
		/* if in 64 bit mode, only 32/64 bit reg addressing is 
		 * allowed. 
		 */
		if (ud->dis_mode == MODE64 && ! ud->prefix.adr) {
			/* if the lower 3 bits of rm = 4 then the next byte is an
		 	 * SIB byte
		 	 */
			if ((rm & 7) == 4) {				
				op->type = (mod * 16) + M_RAX + (rm & 7);
				decode_sib64(ud, op, mod);
			} else
				op->type = (mod * 16) + M_RAX + (rm);
		} else {
			/* 16 bit addressing */
			if (ud->adr_mode == MODE16)				
				op->type = (mod * 8) + M_BX_SI + rm;
			else {
				/* 32 bit addressing or MODE64 with adr prefix */
				op->type = (mod * 8) + M_EAX + rm;

				/* if the lower 3 bits of rm = 4 then the next 
				 * byte is an SIB byte.
				 */
				if (rm == 4)
					decode_sib32(ud, op, mod);
			}
		}
  	}
	
  	/* 8 bit displacements */
  	if (	(M_BX_SI_OFF8 <= op->type && op->type <= M_BX_OFF8  ) ||
      		(M_EAX_OFF8   <= op->type && op->type <= M_EDI_OFF8 ) ||
      		(M_RAX_OFF8   <= op->type && op->type <= M_R15_OFF8 ) ||
		(M_ESP_OFF8   <= op->type && op->type <= M_ESP_OFF8 ) ||
		(M_RSP_OFF8   <= op->type && op->type <= M_RSP_OFF8 ) ) {

		op->lval.bits8 = src_uint8(ud) & 0xff;

	} 
  	/* 16 bit displacements */
	else 
	if (	(M_BX_SI_OFF16 <= op->type && op->type <= M_BX_OFF16) ||
      		(M_OFF16 == op->type) ) {

		op->lval.bits16 = src_uint16(ud) & 0xffff;
	} 
  	/* 32 bit displacements */
	else 
	if (	(M_EAX_OFF32 <= op->type && op->type <= M_EDI_OFF32) ||
		(M_RAX_OFF32 <= op->type && op->type <= M_R15_OFF32) ||
		(M_ESP_OFF32 <= op->type && op->type <= M_ESP_OFF32) ||
		(M_RSP_OFF32 <= op->type && op->type <= M_RSP_OFF32) ||
		(M_OFF32 == op->type) ||
		(M_ROFF32== op->type) ||
		(M_SID   == op->type) ||
		(M_RSID  == op->type) ) {

		op->lval.bits32 = src_uint32(ud) & 0xffffffffL;
	}

  	if (opreg) {
		/* resolve register encoded in reg field */
		if (reg_type == T_GPR) /* general purpose register */
			opreg->type = decode_gpr(ud, reg_size, reg);
		else opreg->type = resolve_reg(ud, reg_type, reg);

		opreg->size = resolve_operand_size(ud, reg_size);
	}

	/* determine the operand sizes */
  	op->size = resolve_operand_size(ud, size);
}

/* decode_o() - Decodes offset */
static void decode_o(register ud_t* ud, ud_operand_size_t size, ud_operand_t *op)
{
	/* In 64bits mode */
	if (ud->dis_mode == MODE64) {
		if (ud->prefix.adr) {
			/* if 0x67, the offest is 32bits long */
			op->lval.bits32 = src_uint32(ud);
			op->type = OFFSET32;
		} else {
			/* else its 64bits long */
			op->lval.bits64 = src_uint64(ud);
			op->type = OFFSET64;
		}
  	} else if (ud->adr_mode == MODE16) {
		/* if the addressing mode is 16bits, so is offset */
		op->lval.bits16 = src_uint16(ud);
		op->type = OFFSET16;
  	} else {
		/* if the addressing mode is 32bits, so is offset */
		op->lval.bits32 = src_uint32(ud);
		op->type = OFFSET32;
	}
  	op->size = resolve_operand_size(ud, size);
}


/* disasm_operands() */
static void disasm_operands(register ud_t* ud)
{
  /* mopXt = map entry, operand X, type; mopXs = map entry, operand X, size */
  ud_operand_type_t mop1t = ud->opcmap_entry->operand1.type;
  ud_operand_type_t mop2t = ud->opcmap_entry->operand2.type;
  ud_operand_type_t mop3t = ud->opcmap_entry->operand3.type;
  ud_operand_size_t mop1s = ud->opcmap_entry->operand1.size;
  ud_operand_size_t mop2s = ud->opcmap_entry->operand2.size;
  ud_operand_size_t mop3s = ud->opcmap_entry->operand3.size;

  /* iop = instruction operand */
  ud_operand_t* iop = ud->operand;


  switch(mop1t) {
	case OP_A :
		decode_a(ud, mop1s, &(iop[0]));
		break;
	/* M[b] */
	case OP_M :
		if (MODRM_MOD(src_peek(ud)) == 3)
			ud->error= 1;

	/* E, G/P/V/I/CL/1/S */
	case OP_E :
		if (mop2t == OP_G)	/* E, G */ {
			decode_modrm(ud, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_GPR);
		}
		else if (mop2t == OP_P)	/* E, P */
			decode_modrm(ud, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_MMX);
		else if (mop2t == OP_V)	/* E, V */
			decode_modrm(ud, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_XMM);
		else if (mop2t == OP_S) /* E, S */
			decode_modrm(ud, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_SEG);
		else {	/* E */
			decode_modrm(ud, &(iop[0]), mop1s, T_GPR, NULL, 0, T_NONE);
			if (mop2t == OP_CL) {	/* E, CL */
				iop[1].type = R_CL;
				iop[1].size = SZ_b;
			} else if (mop2t == OP_I1) {	/* E, 1 */
				iop[1].type = PRINT_I;
				ud->operand[1].lval.bits32 = 1;
			} else if (mop2t == OP_I) {	/* E, I */
				decode_imm(ud, mop2s, &(iop[1]));
			}
		}
		break;

	/* G, E/PR[,I]/VR */
	case OP_G :
		if (mop2t == OP_E) {	/* G, E */
			decode_modrm(ud, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_GPR);
			if (mop3t == OP_I) /* G, E, I */
				decode_imm(ud, mop3s, &(iop[2]));
		}
		else if (mop2t == OP_PR) {	/* G, PR */
			/* the mod field ought to be zero */
			if (MODRM_RM(src_peek(ud)) != 3)
				ud->error = 1;
			decode_modrm(ud, &(iop[1]), mop2s, T_MMX, &(iop[0]), mop1s, T_GPR);
			if (mop3t == OP_I) /* G, PR, I */
				decode_imm(ud, mop3s, &(iop[2]));
		} else if (mop2t == OP_VR) {	/* G, VR */
			/* the mod field ought to be zero */
			if (MODRM_RM(src_peek(ud)) != 3)
				ud->error= 1;
			decode_modrm(ud, &(iop[1]), mop2s, T_XMM, &(iop[0]), mop1s, T_GPR);
		}
		break;

	/* AL..BH, I/O/DX */
	case OP_AL : case OP_CL : case OP_DL : case OP_BL :
	case OP_AH : case OP_CH : case OP_DH : case OP_BH :
		iop[0].type = R_AL + (mop1t - OP_AL);
		iop[0].size = SZ_b;
		if (mop2t == OP_I)	/* AL..BH, Ib */
			decode_imm(ud, mop2s, &(iop[1]));
		else if (mop2t == OP_DX)	/* AL..BH, DX */
			iop[1].type = R_DX;
		else if (mop2t == OP_O)	/* AL..BH, O */
			decode_o(ud, mop2s, &(iop[1]));
		break;

	/* rAX[r8]..rDI[r15], I/rAX..rDI/O */
	case OP_rAXr8 : case OP_rCXr9 : case OP_rDXr10 : case OP_rBXr11 :
	case OP_rSPr12: case OP_rBPr13: case OP_rSIr14 : case OP_rDIr15 :
	case OP_rAX : case OP_rCX : case OP_rDX : case OP_rBX :
	case OP_rSP : case OP_rBP : case OP_rSI : case OP_rDI :
		iop[0].type = resolve_gpr64(ud, mop1t);
		if (mop2t == OP_I)	/* rAX[r8]..rDI[r15], I */ {
			decode_imm(ud, mop2s, &(iop[1]));
		}
		else if (mop2t >= OP_rAX && mop2t <= OP_rDI)
			/* rAX[r8]..rDI[r15], rAX..rDI */
			iop[1].type = resolve_gpr64(ud, mop2t);
		else if (mop2t == OP_O)
			/* rAX[r8]..rDI[r15], O */
			decode_o(ud, ud->opcmap_entry->operand2.size, &(iop[1]));
		break;

	/* AL[r8b]..BH[r15b], I */
	case OP_ALr8b : case OP_CLr9b : case OP_DLr10b : case OP_BLr11b :
	case OP_AHr12b: case OP_CHr13b: case OP_DHr14b : case OP_BHr15b :
	{
		ud_operand_type_t gpr = (mop1t - OP_ALr8b) + R_AL + 
						(P_REX_B(ud->prefix.rex) << 3);
		if (R_AH <= gpr && ud->prefix.rex)
			gpr = gpr + 4;		
		iop[0].type = gpr;
		if (mop2t == OP_I)
			decode_imm(ud, mop2s, &(iop[1]));
		break;
	}

	/* eAX..eDX, DX/I */
	case OP_eAX : case OP_eCX : case OP_eDX : case OP_eBX :
	case OP_eSP : case OP_eBP : case OP_eSI : case OP_eDI :
		iop[0].type = resolve_gpr32(ud, mop1t);
		if (mop2t == OP_DX)	/* eAX..eDX, DX */
			iop[1].type = R_DX;
		else
		if (mop2t == OP_I)	/* eAX..eDX, I */
			decode_imm(ud, mop2s, &(iop[1]));
		break;

	/* ES..GS */
	case OP_ES : case OP_CS : case OP_DS :
	case OP_SS : case OP_FS : case OP_GS :
		/* in 64bits mode, only fs and gs are allowed */
		if (ud->dis_mode == MODE64)
			if (mop1t != OP_FS && mop1t != OP_GS)
				ud->error= 1;
		iop[0].type = (mop1t - OP_ES) + R_ES;
		break;

	/* J */
	case OP_J :
		decode_imm(ud, mop1s, &(iop[0]));
		iop[0].type = J_IMM;		
		switch(iop[0].size) {
			case SZ_b : iop[0].size = SZ_jb; break;
			case SZ_w : iop[0].size = SZ_jw; break;
			case SZ_d : iop[0].size = SZ_jd; break;
			default: break;
		}
		break ;

	/* P, Q/W/E[,I] */
	case OP_P :
		if (mop2t == OP_Q)	/* P, Q */
			decode_modrm(ud, &(iop[1]), mop2s, T_MMX, &(iop[0]), mop1s, T_MMX);
		else if (mop2t == OP_W)	/* P, W */
			decode_modrm(ud, &(iop[1]), mop2s, T_XMM, &(iop[0]), mop1s, T_MMX);
		else if (mop2t == OP_E) {	/* P, E */
			decode_modrm(ud, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_MMX);
			if (mop3t == OP_I)	/* P, E, I */
				decode_imm(ud, mop3s, &(iop[2]));
		}
		break;

	/* R, C/D */
	case OP_R :
		if (mop2t == OP_C)	/* R, C */
			decode_modrm(ud, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_CRG);
		else
		if (mop2t == OP_D)	/* R, D */
			decode_modrm(ud, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_DBG);
		break;

	/* C, R */
	case OP_C :
		decode_modrm(ud, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_CRG);
		break;

	/* D, R */
	case OP_D :
		decode_modrm(ud, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_DBG);
		break;

	/* Q, P */
	case OP_Q :
		decode_modrm(ud, &(iop[0]), mop1s, T_MMX, &(iop[1]), mop2s, T_MMX);
		break;

	/* S, E */
	case OP_S :
		decode_modrm(ud, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_SEG);
		break;

	/* W, V */
	case OP_W :
		decode_modrm(ud, &(iop[0]), mop1s, T_XMM, &(iop[1]), mop2s, T_XMM);
		break;

	/* V, W[,I]/Q */
	case OP_V :
		if (mop2t == OP_W) {	/* V, W */
			decode_modrm(ud, &(iop[1]), mop2s, T_XMM, &(iop[0]), mop1s, T_XMM);
			if (mop3t == OP_I)	/* V, W, I */
				decode_imm(ud, mop3s, &(iop[2]));
		} else if (mop2t == OP_Q)	/* V, Q */
			decode_modrm(ud, &(iop[1]), mop2s, T_MMX, &(iop[0]), mop1s, T_XMM);
		break;

	/* DX, eAX/AL */
	case OP_DX :
		iop[0].type = R_DX;
		if (mop2t == OP_eAX)	/* DX, eAX */
			iop[1].type = resolve_gpr32(ud, mop2t);
		else if (mop2t == OP_AL)	/* DX, AL */
			iop[1].type = R_AL;
		break;

	/* I, I/AL/eAX */
	case OP_I :
		decode_imm(ud, mop1s, &(iop[0]));
		if (mop2t == OP_I)	/* I, I */
			decode_imm(ud, mop2s, &(iop[1]));
		else if (mop2t == OP_AL)	/* I, AL */
			iop[1].type = R_AL;
		else if (mop2t == OP_eAX)	/* I, eAX */
			iop[1].type = resolve_gpr32(ud, mop2t);
		break;

	/* O, AL/eAX */
	case OP_O :
		decode_o(ud, mop1s, &(iop[0]));
		if (mop2t == OP_AL)	/* O, AL */
			iop[1].type = R_AL;
		else if (mop2t == OP_eAX)	/* O, eAX */
			iop[1].type = resolve_gpr32(ud, mop2t);
		break;

	/* 3 */
	case OP_I3 :
		iop[1].type = PRINT_I;
		iop[1].lval.bits8 = 3;
		break;
		
	case OP_ST0 : case OP_ST1 : case OP_ST2 : case OP_ST3 :
	case OP_ST4 : case OP_ST5 : case OP_ST6 : case OP_ST7 :

		iop[0].type = (mop1t-OP_ST0)  + R_ST0;
		if (mop2t >= OP_ST0 && mop2t <= OP_ST7)
			iop[1].type = (mop2t-OP_ST0) + R_ST0;
		break;

	/* none */
	default :
		iop[0].type = iop[1].type = iop[2].type = -1;
  }
}

/* clear instruction pointer */
extern void ud_clear_insn(ud_t *ud)
{
	ud->error= 0;
	ud->prefix.seg = 0;
	ud->prefix.opr = 0;
	ud->prefix.adr = 0;
	ud->prefix.lock= 0;
	ud->prefix.repne=0;
	ud->prefix.rep = 0;
	ud->prefix.seg = 0;
	ud->prefix.rex = 0;
 	ud->mnemonic = 0;
	ud->operand[0].size  = 0;
	ud->operand[1].size  = 0;
	ud->operand[2].size  = 0;
	ud->operand[0].type  = (-1);
	ud->operand[1].type  = (-1);
	ud->operand[2].type  = (-1);
	ud->opcmap_entry = NULL;
}

extern int ud_extract_prefixes(register ud_t* ud) 
{
	/* load byte from input stream */
	src_next(ud);

	int prefixflag = 0;	/* (bool) denotes end of prefixes */
	int i;		/* prefix counter */
	
	/* search for prefixes */
	for (i = 0; prefixflag == 0; ++i) {	
		/* check for rex bit in 64 bits mode */		
		if (0x40 <= src_curr(ud) && src_curr(ud) <= 0x4F) {
			if (ud->dis_mode == MODE64) {
				ud->prefix.rex = src_curr(ud);
				/* move to next byte */
				src_next(ud); 
			} else prefixflag = 1;
		} else switch(src_curr(ud)) {
			case 0x2E : ud->prefix.seg = R_CS; src_next(ud); break;
			case 0x36 : ud->prefix.seg = R_SS; src_next(ud); break;
			case 0x3E : ud->prefix.seg = R_DS; src_next(ud); break;
			case 0x26 : ud->prefix.seg = R_ES; src_next(ud); break;
			case 0x64 : ud->prefix.seg = R_FS; src_next(ud); break;
			case 0x65 : ud->prefix.seg = R_GS; src_next(ud); break;
			case 0x66 : ud->prefix.opr = 0x66; src_next(ud); break;
			case 0x67 : ud->prefix.adr = 0x67; src_next(ud); break;
			case 0xF0 : ud->prefix.lock= 0xF0; src_next(ud); break;
			case 0xF2 : ud->prefix.repne=0xF2; src_next(ud); break;
			case 0xF3 : ud->prefix.rep = 0xF3; src_next(ud); break;
			default   : prefixflag = 1;
		}

		/* if > MAX_PREFIXES, disintegrate */
		if (i >= MAX_PREFIXES) {
			ud->error= 1;
			break;	/* break from loop */
		}
  	}

	/* rewind stream */
	src_back(ud);

	/* return prefix count */
	return i;
}

extern void ud_decode(register ud_t* ud)
{
	/* init modes */
	ud->opr_mode = (ud->adr_mode = 
		(ud->dis_mode == MODE64 || ud->dis_mode == MODE32) ? 
			MODE32 : MODE16);

	/* extract prefixes and opcode from opcode map */
	ud_clear_insn(ud);
	ud_extract_prefixes(ud);
	ud_search_opcmap(ud);

	/* assign instruction mnemonic code */
	ud->mnemonic = ud->opcmap_entry->mnemonic;

	/* if invalid set error */
	if (ud->mnemonic == Iinvalid) 
		ud->error= 1;

	/* process the prefixes */
	/* rex */
	ud->prefix.rex = ud->prefix.rex & P_REX_MASK(ud->opcmap_entry->prefix); 
	/* set the default 64 flag */
	ud->default64 = P_DEF64(ud->opcmap_entry->prefix); 
	/* set the suffix flag */
	ud->suffix = P_SUFF(ud->opcmap_entry->prefix);

	/* check if the instruction is invalid in 64 bits mode */
	if (ud->dis_mode == MODE64 && P_INV64(ud->opcmap_entry->prefix))
		ud->error= 1;

	/* set operand mode */
	if (ud->prefix.opr) {
		if (ud->dis_mode == MODE64)
			ud->opr_mode = MODE16;
		else ud->opr_mode = (ud->opr_mode == MODE32) ? MODE16 : MODE32 ;
	}

	/* set address mode */
	if (ud->prefix.adr) {
		if (ud->dis_mode == MODE64)
			ud->adr_mode = MODE32;
		else	ud->adr_mode = (ud->adr_mode == MODE32) ? MODE16 : MODE32 ;
	}

	/* check if the instruction if dependent on the mode */
	if (P_DEP32(ud->opcmap_entry->prefix)) {
		if (ud->opr_mode == MODE16)
			ud->error = 1;
	}

	/* disassemble operands */
	disasm_operands(ud);

	/* get suffix size */
	if (ud->suffix)
		ud->operand[0].size = resolve_operand_size(ud, SZ_v);

	/* if the operand should be cast */
	if (P_CAST(ud->opcmap_entry->prefix))
		ud->cast = 1;
	else ud->cast = 0;

	/* check if P_O32 prefix was used */
	if (P_O32(ud->opcmap_entry->prefix) && ud->prefix.opr)
  		ud->prefix.opr = 0;

	/* check if P_A32 prefix was used */
	if (P_A32(ud->opcmap_entry->prefix) && ud->prefix.adr)
		ud->prefix.adr = 0;

	/* if its a 3dnow! instruction, get mnemonic code based on the suffix */
	if (ud->mnemonic == I3dnow)
		ud->mnemonic = ud_get_3dnow_insn_mnemonic(src_curr(ud));

	/* check for ud->error*/
	if (ud->error) {
		ud_clear_insn(ud);
		/* init as define byte */
		ud->mnemonic = Idb;
		ud->operand[0].type = I_BYTE;
		ud->operand[0].size = 0;
		/* reset stream to move only 1 byte forward */
		src_reset(ud); 
		/* the first byte as operand */
		ud->operand[0].lval.bits8 = src_uint8(ud);

	} 
}
