/* syn-intel.c
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#include "types.h"
#include "extern.h"
#include "opcmap.h"
#include "syn.h"

/* -----------------------------------------------------------------------------
 * put_offset() - Prints an operand cast.
 * -----------------------------------------------------------------------------
 */
static void put_offset(struct ud* u, struct ud_operand* op)
{
  if (op->offset == 8) {
	if (op->lval.sbyte < 0)
		mkasm(u, "-0x%x", op->lval.sbyte);
	else	mkasm(u, "0x%x", op->lval.sbyte);
  } 
  else if (op->offset == 16) 
	mkasm(u, "0x%x", op->lval.uword);
  else if (op->offset == 32) 
	mkasm(u, "0x%lx", op->lval.udword);
  else if (op->offset == 64) 
	mkasm(u, "0x%llx", op->lval.udword);
}

/* -----------------------------------------------------------------------------
 * opr_cast() - Prints an operand cast.
 * -----------------------------------------------------------------------------
 */
static void opr_cast(struct ud* u, struct ud_operand* op, int* syn_cast)
{
  if(*syn_cast) {
	switch(op->size) {
		case UD_SZ_WORD : case UD_SZ_DWORD :
		case UD_SZ_WP:    case UD_SZ_DP:
			mkasm(u, "*");   break;
		default: break;
	}
  	(*syn_cast)--;
  }
}


/* -----------------------------------------------------------------------------
 * gen_operand() - Generates assembly output for each operand.
 * -----------------------------------------------------------------------------
 */
static void gen_operand(struct ud* u, struct ud_operand* op, int* syn_cast)
{
  switch(op->type) {
	case UD_OP_REG:
		mkasm(u, "%%%s", ud_reg_tab[op->base - UD_R_AL]);
		break;

	case UD_OP_MEM:
		opr_cast(u, op, syn_cast);
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_ES]);
		put_offset(u, op);
		mkasm(u, "(");
		mkasm(u, "%%%s", ud_reg_tab[op->base - UD_R_AL]);
		if (op->index)
			mkasm(u, ",%%%s", ud_reg_tab[op->index - UD_R_AL]);
		mkasm(u, ")");
		break;

	case UD_OP_SIB:
	case UD_OP_RSIB:
		opr_cast(u, op, syn_cast);
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_ES]);
		put_offset(u, op);
		mkasm(u, "(");
		mkasm(u, "%%%s,%%%s", ud_reg_tab[op->base - UD_R_AL], 
				ud_reg_tab[op->index - UD_R_AL]);		
		if (op->scale)
			mkasm(u, ",%d", op->scale);
		mkasm(u, ")");
		break;

	case UD_OP_SID:
	case UD_OP_RSID:
		opr_cast(u, op, syn_cast);
		if (u->pfx_seg)
			mkasm(u, "%%%s:", ud_reg_tab[u->pfx_seg - UD_R_ES]);
		put_offset(u, op);
		mkasm(u, "(,");
		mkasm(u, "%%%s", ud_reg_tab[op->index - UD_R_AL]);
		if (op->scale)
			mkasm(u, ",%d", op->scale);
		mkasm(u, ")");
		break;
			
	case UD_OP_IMM8:
		mkasm(u, "$0x%x", op->lval.ubyte);
		break;
	case UD_OP_IMM16:
		mkasm(u, "$0x%x", op->lval.uword);
		break;
	case UD_OP_IMM32:
		mkasm(u, "$0x%lx", op->lval.udword);
		break;
	case UD_OP_IMM64:
		mkasm(u, "$0x%llx", op->lval.uqword);
		break;

	case UD_OP_OFF8:
		opr_cast(u, op, syn_cast);
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_ES]);
		mkasm(u, "0x%x", op->lval.ubyte);
		break;
	case UD_OP_OFF16:
		opr_cast(u, op, syn_cast);
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_ES]);
		mkasm(u, "0x%x", op->lval.uword);
		break;
	case UD_OP_OFF32:
		opr_cast(u, op, syn_cast);
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_ES]);
		mkasm(u, "0x%lx", op->lval.udword);
		break;
	case UD_OP_OFF64:
		opr_cast(u, op, syn_cast);
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_ES]);
		mkasm(u, "0x%llx", op->lval.uqword);
		break;

	case UD_OP_JIMM:
		switch(op->size) {
			case UD_SZ_JBYTE:
				mkasm(u, "0x%lx", u->pc + op->lval.sbyte);
				break;
			case UD_SZ_JWORD:
				mkasm(u, "0x%lx", u->pc + op->lval.sword);
				break;
			case UD_SZ_JDWORD:
				mkasm(u, "0x%lx", u->pc + op->lval.sdword);
				break;
			default: break;
		}
		break;

	case UD_OP_PTR32:
		mkasm(u, "$0x%x, $0x%x", op->lval.ptr.seg, 
			op->lval.ptr.off & 0xFFFF);
		break;
	case UD_OP_PTR48:
		mkasm(u, "$0x%x, $0x%lx", op->lval.ptr.seg, op->lval.ptr.off);
		break;

	case UD_OP_CONST:
		mkasm(u, "$%d", op->lval.udword);
		break;

	default: return;
  }
}

/* =============================================================================
 * translates to AT&T syntax 
 * =============================================================================
 */
extern void ud_translate_att(struct ud *u)
{
	int syn_cast = 0;

	/* all instructions specify an operand cast */
	u->cast = 0;

	if (u->pfx_opr)
		mkasm(u,  "o32 ");
	if (u->pfx_adr)
		mkasm(u,  "a32 ");
	if (u->pfx_lock)
		mkasm(u,  "lock ");
	if (u->pfx_rep)
		mkasm(u,  "rep ");
	if (u->pfx_repne)
		mkasm(u,  "repne ");

	/* special instructions */
	if (u->mnemonic == UD_Iretf)
		mkasm(u, "lret ");
	if (u->mnemonic == UD_Idb) {
		mkasm(u, ".byte 0x%x", u->operand[0].lval.ubyte);
		return;
	 }

	if (u->mnemonic == UD_Ijmp || u->mnemonic == UD_Icall ) {
		if ( u->operand[0].size == UD_SZ_DP || u->operand[0].size == UD_SZ_WP )
			mkasm(u,  "l");
		syn_cast = 1;
		u->cast = 1;

		mkasm(u, "%s", ud_lookup_mnemonic(u->mnemonic));
	} else {
		mkasm(u, "%s", ud_lookup_mnemonic(u->mnemonic));

		/* att instruction suffix */
		if (u->operand[0].size == UD_SZ_BYTE || u->operand[1].size == UD_SZ_BYTE)
			mkasm(u, "b");
		else
		if (u->operand[0].size == UD_SZ_WORD || u->operand[1].size == UD_SZ_WORD)
			mkasm(u, "w");
		else
		if (u->operand[0].size == UD_SZ_DWORD  || u->operand[1].size == UD_SZ_DWORD ||
			u->operand[1].size == UD_SZ_DP || u->operand[0].size == UD_SZ_DP)
			mkasm(u, "l");
		else
		if (u->operand[0].size == UD_SZ_QWORD  || u->operand[1].size == UD_SZ_QWORD )
			mkasm(u, "q");
	}

	mkasm(u, " ");

	if ( u->mnemonic == UD_Ibound || u->mnemonic == UD_Ienter) {
		if (u->operand[0].type != UD_NONE)
			gen_operand(u, &u->operand[0], &syn_cast);
		if (u->operand[1].type != UD_NONE) {
			mkasm(u, ",");
			gen_operand(u, &u->operand[1], &syn_cast);
		}
	} else {
		/* Print operands, in reverse sequence */
		if (u->operand[2].type != UD_NONE) {
			gen_operand(u, &u->operand[2], &syn_cast);
  			mkasm(u, ", ");
        	}

		if (u->operand[1].type != UD_NONE) {
			gen_operand(u, &u->operand[1], &syn_cast);
			mkasm(u, ", ");
		}

		if (u->operand[0].type != UD_NONE)
			gen_operand(u, &u->operand[0], &syn_cast);
	}
}
