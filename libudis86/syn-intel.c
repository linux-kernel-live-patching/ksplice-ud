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
	else	mkasm(u, "+0x%x", op->lval.sbyte);
  } 
  else if (op->offset == 16) 
	mkasm(u, "+0x%x", op->lval.uword);
  else if (op->offset == 32) 
	mkasm(u, "+0x%lx", op->lval.udword);
  else if (op->offset == 64) 
	mkasm(u, "+0x%llx", op->lval.udword);
}

/* -----------------------------------------------------------------------------
 * opr_cast() - Prints an operand cast.
 * -----------------------------------------------------------------------------
 */
static void opr_cast(struct ud* u, struct ud_operand* op, int* syn_cast)
{
  if(*syn_cast) {
	switch(op->size) {
		case UD_SZ_SBYTE:
		case UD_SZ_BYTE: 
			mkasm(u, "byte " ); 
			break;
		case UD_SZ_WORD: 
			mkasm(u, "word " ); 
			break;
		case UD_SZ_DWORD: 
			mkasm(u, "dword "); 
			break;
		case UD_SZ_WP: 
			mkasm(u, "word far "); 
			break;
		case UD_SZ_DP: 
			mkasm(u, "dword far "); 
			break;
		case UD_SZ_QWORD : 
			mkasm(u, "qword "); 
			break;
		default:
			break;
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
		mkasm(u, ud_reg_tab[op->base - UD_R_AL]);
		break;

	case UD_OP_MEM:
		opr_cast(u, op, syn_cast);
		mkasm(u, "[");
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_AL]);
		mkasm(u, "%s", ud_reg_tab[op->base - UD_R_AL]);
		if (op->index)
			mkasm(u, "+%s", ud_reg_tab[op->index - UD_R_AL]);
		put_offset(u, op);
		mkasm(u, "]");
		break;

	case UD_OP_SIB:
	case UD_OP_RSIB:
		opr_cast(u, op, syn_cast);
		mkasm(u, "[");
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_AL]);
		mkasm(u, "%s+%s", ud_reg_tab[op->base - UD_R_AL], 
				ud_reg_tab[op->index - UD_R_AL]);		
		if (op->scale)
			mkasm(u, "*%d", op->scale);
		put_offset(u, op);
		mkasm(u, "]");
		break;

	case UD_OP_SID:
	case UD_OP_RSID:
		opr_cast(u, op, syn_cast);
		mkasm(u, "[");
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_AL]);
		mkasm(u, "%s", ud_reg_tab[op->index - UD_R_AL]);		
		if (op->scale)
			mkasm(u, "*%d", op->scale);
		put_offset(u, op);
		mkasm(u, "]");
		break;
			
	case UD_OP_IMM8:
		opr_cast(u, op, syn_cast);
		mkasm(u, "0x%x", op->lval.ubyte);
		break;
	case UD_OP_IMM16:
		opr_cast(u, op, syn_cast);
		mkasm(u, "0x%x", op->lval.uword);
		break;
	case UD_OP_IMM32:
		opr_cast(u, op, syn_cast);
		mkasm(u, "0x%lx", op->lval.udword);
		break;
	case UD_OP_IMM64:
		opr_cast(u, op, syn_cast);
		mkasm(u, "0x%llx", op->lval.uqword);
		break;

	case UD_OP_OFF8:
		opr_cast(u, op, syn_cast);
		mkasm(u, "[");
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_AL]);
		mkasm(u, "0x%x]", op->lval.ubyte);
		break;
	case UD_OP_OFF16:
		opr_cast(u, op, syn_cast);
		mkasm(u, "[");
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_AL]);
		mkasm(u, "0x%x]", op->lval.uword);
		break;
	case UD_OP_OFF32:
		opr_cast(u, op, syn_cast);
		mkasm(u, "[");
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_AL]);
		mkasm(u, "0x%lx]", op->lval.udword);
		break;
	case UD_OP_OFF64:
		opr_cast(u, op, syn_cast);
		mkasm(u, "[");
		if (u->pfx_seg)
			mkasm(u, "%s:", ud_reg_tab[u->pfx_seg - UD_R_AL]);
		mkasm(u, "0x%llx]", op->lval.uqword);
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
		mkasm(u, "0x%x:0x%x", op->lval.ptr.seg, 
			op->lval.ptr.off & 0xFFFF);
		break;
	case UD_OP_PTR48:
		mkasm(u, "0x%x:0x%lx", op->lval.ptr.seg, op->lval.ptr.off);
		break;

	case UD_OP_CONST:
		mkasm(u, "%d", op->lval.udword);
		break;

	default: return;
  }
}

/* =============================================================================
 * translates to intel syntax 
 * =============================================================================
 */
extern void ud_translate_intel(struct ud* u)
{
	int syn_cast = 0;

	if (u->operand[0].size != u->operand[1].size && 
	    	u->operand[2].type == UD_NONE)
		syn_cast = 2;
	else
	if (u->operand[2].type != UD_NONE && 
		((u->operand[0].size != u->operand[1].size) ||
		 (u->operand[1].size != u->operand[2].size) || 
		 (u->operand[0].size != u->operand[2].size)))
		syn_cast = 2;

	if (u->operand[0].type != UD_OP_REG && u->operand[1].type != UD_OP_REG)
		syn_cast = 1;

	/* prefixes */
	if (u->pfx_opr)
		mkasm(u, "o32 ");
	if (u->pfx_adr)
		mkasm(u, "a32 ");
	if (u->pfx_lock)
		mkasm(u, "lock ");
	if (u->pfx_rep)
		mkasm(u, "rep ");
	if (u->pfx_repne)
		mkasm(u, "repne ");

	/* print the instruction mnemonic */
	mkasm(u, "%s", ud_lookup_mnemonic(u->mnemonic));

	/* string operation suffix */
	if (u->sfx) {
		switch(u->operand[0].size) {
			case UD_SZ_WORD  : mkasm(u, "w"); break;
			case UD_SZ_DWORD : mkasm(u, "d"); break;
			case UD_SZ_QWORD : mkasm(u, "q"); break;
			default:
				break;
		}
	}

	mkasm(u, " ");

	/* operand 1 */
	if (u->operand[0].type != UD_NONE) {
		gen_operand(u, &u->operand[0], &syn_cast);
	}

	/* operand 2 */
	if (u->operand[1].type != UD_NONE) {
		mkasm(u, ", ");
		gen_operand(u, &u->operand[1], &syn_cast);
	}

	/* operand 3 */
	if (u->operand[2].type != UD_NONE) {
		mkasm(u, ", ");
		gen_operand(u, &u->operand[2], &syn_cast);
	}
}
