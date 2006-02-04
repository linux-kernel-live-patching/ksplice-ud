/* syn-att.c
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#include <stdlib.h>
#include "types.h"
#include "macros.h"
#include "extern.h"
#include "opcmap.h"
#include "syn.h"

static uint8_t n_scale[] = { 1, 2, 4, 8 };

/* AT&T Syntax Operand Decoder Map
 * ================================
 * WARNING -- Make sure the order of declaration is the same as
 * that of the enumeration of operand codes in x86.h
 */
ud_syntax_map_t att_smap[] = {
  { M_BX_SI,		"\60\6\1(%bx,%si)\1" },
  { M_BX_DI,		"\60\6\1(%bx,%di)\1" },
  { M_BP_SI,		"\60\6\1(%bp,%si)\1" },
  { M_BP_DI,		"\60\6\1(%bp,%di)\1" },
  { M_SI,		"\60\6\1(%si)\1" },
  { M_DI,		"\60\6\1(%di)\1" },
  { M_OFF16,		"\60\6\3" },
  { M_BX,		"\60\6\1(%bx)\1" },
  { M_BX_SI_OFF8,	"\60\6\2\1(%bx,%si)\1" },
  { M_BX_DI_OFF8, 	"\60\6\2\1(%bx,%di)\1" },
  { M_BP_SI_OFF8,	"\60\6\2\1(%bp,%si)\1" },
  { M_BP_DI_OFF8,	"\60\6\2\1(%bp,%di)\1" },
  { M_SI_OFF8, 		"\60\6\2\1(%si)\1" },
  { M_DI_OFF8, 		"\60\6\2\1(%di)\1" },
  { M_BP_OFF8, 		"\60\6\2\1(%bp)\1" },
  { M_BX_OFF8,		"\60\6\2\1(%bx)\1" },
  { M_BX_SI_OFF16, 	"\60\6\3\1(%bx,%si)\1" },
  { M_BX_DI_OFF16, 	"\60\6\3\1(%bx,%di)\1" },
  { M_BP_SI_OFF16, 	"\60\6\3\1(%bp,%si)\1" },
  { M_BP_DI_OFF16,	"\60\6\3\1(%bp,%di)\1" },
  { M_SI_OFF16, 	"\60\6\3\1(%si)\1" },
  { M_DI_OFF16, 	"\60\6\3\1(%di)\1" },
  { M_BP_OFF16, 	"\60\6\3\1(%bp)\1" },
  { M_BX_OFF16,		"\60\6\3\1(%bx)\1" },
  { M_EAX,		"\60\6\1(%eax)\1" },
  { M_ECX,		"\60\6\1(%ecx)\1" },
  { M_EDX,		"\60\6\1(%edx)\1" },
  { M_EBX,		"\60\6\1(%ebx)\1" },
  { M_SIB,		"\60\6\1(\1\020\1)\1" },
  { M_OFF32,		"\60\6\4" },
  { M_ESI, 		"\60\6\1(%esi)\1" },
  { M_EDI,		"\60\6\1(%edi)\1" },
  { M_EAX_OFF8, 	"\60\6\2\1(%eax)\1" },
  { M_ECX_OFF8, 	"\60\6\2\1(%ecx)\1" },
  { M_EDX_OFF8, 	"\60\6\2\1(%edx)\1" },
  { M_EBX_OFF8, 	"\60\6\2\1(%ebx)\1" },
  { M_SIB_OFF8,		"\60\6\2\1(\1\020\1)\1" },
  { M_EBP_OFF8, 	"\60\6\2\1(%ebp)\1" },
  { M_ESI_OFF8, 	"\60\6\2\1(%esi)\1" },
  { M_EDI_OFF8, 	"\60\6\2\1(%edi)\1" },
  { M_EAX_OFF32, 	"\60\6\4\1(%eax)\1" },
  { M_ECX_OFF32, 	"\60\6\4\1(%ecx)\1" },
  { M_EDX_OFF32, 	"\60\6\4\1(%edx)\1" },
  { M_EBX_OFF32, 	"\60\6\4\1(%ebx)\1" },
  { M_SIB_OFF32,	"\60\6\4\1(\1\020\1)\1" },
  { M_EBP_OFF32, 	"\60\6\4\1(%ebp)\1" },
  { M_ESI_OFF32, 	"\60\6\4\1(%esi)\1" },
  { M_EDI_OFF32, 	"\60\6\4\1(%edi)\1" },
  { M_SID,		"\60\6\4\1(\1\021\1)\1" },
  { M_RAX,		"\60\6\1(%rax)\1" },
  { M_RCX,		"\60\6\1(%rcx)\1" },
  { M_RDX,		"\60\6\1(%rdx)\1" },
  { M_RBX,		"\60\6\1(%rbx)\1" },
  { M_RSIB,		"\60\6\1(\1\022\1)\1" },
  { M_ROFF32,		"\60\6\4" },
  { M_RSI, 		"\60\6\1(%rsi)\1" },
  { M_RDI,		"\60\6\1(%rdi)\1" },
  { M_R8,		"\60\6\1(%r8)\1" },
  { M_R9,		"\60\6\1(%r9)\1" },
  { M_R10,		"\60\6\1(%r10)\1" },
  { M_R11,		"\60\6\1(%r11)\1" },
  { M_R12,		"\60\6\1(%r12)\1" },
  { M_R13,		"\60\6\1(%r13)\1" },
  { M_R14, 		"\60\6\1(%r14)\1" },
  { M_R15,		"\60\6\1(%r15)\1" },
  { M_RAX_OFF8, 	"\60\6\2\1(%rax)\1" },
  { M_RCX_OFF8, 	"\60\6\2\1(%rcx)\1" },
  { M_RDX_OFF8, 	"\60\6\2\1(%rdx)\1" },
  { M_RBX_OFF8, 	"\60\6\2\1(%rbx)\1" },
  { M_RSIB_OFF8,	"\60\6\2\1(\1\022\1)\1" },
  { M_RBP_OFF8, 	"\60\6\2\1(%rbp)\1" },
  { M_RSI_OFF8, 	"\60\6\2\1(%rsi)\1" },
  { M_RDI_OFF8, 	"\60\6\2\1(%rdi)\1" },
  { M_R8_OFF8,		"\60\6\2\1(%r8)\1" },
  { M_R9_OFF8,		"\60\6\2\1(%r9)\1" },
  { M_R10_OFF8, 	"\60\6\2\1(%r10)\1" },
  { M_R11_OFF8, 	"\60\6\2\1(%r11)\1" },
  { M_R12_OFF8,		"\60\6\2\1(%r12)\1" },
  { M_R13_OFF8, 	"\60\6\2\1(%r13)\1" },
  { M_R14_OFF8, 	"\60\6\2\1(%r14)\1" },
  { M_R15_OFF8, 	"\60\6\2\1(%r15)\1" },
  { M_RAX_OFF32, 	"\60\6\4\1(%rax)\1" },
  { M_RCX_OFF32, 	"\60\6\4\1(%rcx)\1" },
  { M_RDX_OFF32, 	"\60\6\4\1(%rdx)\1" },
  { M_RBX_OFF32, 	"\60\6\4\1(%rbx)\1" },
  { M_RSIB_OFF32,	"\60\6\4\1(\1\022\1)\1" },
  { M_RBP_OFF32, 	"\60\6\4\1(%rbp)\1" },
  { M_RSI_OFF32, 	"\60\6\4\1(%rsi)\1" },
  { M_RDI_OFF32, 	"\60\6\4\1(%rdi)\1" },
  { M_R8_OFF32,		"\60\6\4\1(%r8)\1" },
  { M_R9_OFF32,		"\60\6\4\1(%r9)\1" },
  { M_R10_OFF32, 	"\60\6\4\1(%r10)\1" },
  { M_R11_OFF32, 	"\60\6\4\1(%r11)\1" },
  { M_R12_OFF32,	"\60\6\4\1(%r12)\1" },
  { M_R13_OFF32, 	"\60\6\4\1(%r13)\1" },
  { M_R14_OFF32, 	"\60\6\4\1(%r14)\1" },
  { M_R15_OFF32, 	"\60\6\4\1(%r15)\1" },
  { M_RSID,		"\60\1(\1\6\023\1+\1\4\1)\1" },
  { OFFSET16,		"\40" },
  { OFFSET32,		"\41" },
  { OFFSET64,		"\42" },
  { R_AL, 		"\1%al\1" },
  { R_CL, 		"\1%cl\1" },
  { R_DL, 		"\1%dl\1" },
  { R_BL, 		"\1%bl\1" },
  { R_AH, 		"\1%ah\1" },
  { R_CH, 		"\1%ch\1" },
  { R_DH, 		"\1%dh\1" },
  { R_BH, 		"\1%bh\1" },
  { R_SPL,		"\1%spl\1"},
  { R_BPL,		"\1%bpl\1"},
  { R_SIL,		"\1%sil\1"},
  { R_DIL,		"\1%dil\1"},
  { R_R8B,		"\1%r8b\1"},
  { R_R9B,		"\1%r9b\1"},
  { R_R10B,		"\1%r10b\1"},
  { R_R11B,		"\1%r11b\1"},
  { R_R12B,		"\1%r12b\1"},
  { R_R13B,		"\1%r13b\1"},
  { R_R14B,		"\1%r14b\1"},
  { R_R15B,		"\1%r15b\1"},
  { R_AX, 		"\1%ax\1" },
  { R_CX, 		"\1%cx\1" },
  { R_DX, 		"\1%dx\1" },
  { R_BX, 		"\1%bx\1" },
  { R_SP, 		"\1%sp\1" },
  { R_BP, 		"\1%bp\1" },
  { R_SI, 		"\1%si\1" },
  { R_DI, 		"\1%di\1" },
  { R_R8W,		"\1%r8w\1"},
  { R_R9W,		"\1%r9w\1"},
  { R_R10W,		"\1%r10w\1"},
  { R_R11W,		"\1%r11w\1"},
  { R_R12W,		"\1%r12w\1"},
  { R_R13W,		"\1%r13w\1"},
  { R_R14W,		"\1%r14w\1"},
  { R_R15W,		"\1%r15w\1"},
  { R_EAX, 		"\1%eax\1"},
  { R_ECX, 		"\1%ecx\1"},
  { R_EDX, 		"\1%edx\1"},
  { R_EBX, 		"\1%ebx\1"},
  { R_ESP, 		"\1%esp\1"},
  { R_EBP, 		"\1%ebp\1"},
  { R_ESI, 		"\1%esi\1"},
  { R_EDI, 		"\1%edi\1"},
  { R_R8D,		"\1%r8d\1"},
  { R_R9D,		"\1%r9d\1"},
  { R_R10D,		"\1%r10d\1"},
  { R_R11D,		"\1%r11d\1"},
  { R_R12D,		"\1%r12d\1"},
  { R_R13D,		"\1%r13d\1"},
  { R_R14D,		"\1%r14d\1"},
  { R_R15D,		"\1%r15d\1"},
  { R_RAX, 		"\1%rax\1"},
  { R_RCX, 		"\1%rcx\1"},
  { R_RDX, 		"\1%rdx\1"},
  { R_RBX, 		"\15rbx\1"},
  { R_RSP, 		"\1%rsp\1"},
  { R_RBP, 		"\1%rbp\1"},
  { R_RSI, 		"\1%rsi\1"},
  { R_RDI, 		"\1%rdi\1"},
  { R_R8,		"\1%r8\1"},
  { R_R9,		"\1%r9\1"},
  { R_R10,		"\1%r10\1"},
  { R_R11,		"\1%r11\1"},
  { R_R12,		"\1%r12\1"},
  { R_R13,		"\1%r13\1"},
  { R_R14,		"\1%r14\1"},
  { R_R15,		"\1%r15\1"},
  { R_ES,		"\1%es\1" },
  { R_CS,		"\1%cs\1" },
  { R_SS,		"\1%ss\1" },
  { R_DS,		"\1%ds\1" },
  { R_FS,		"\1%fs\1" },
  { R_GS,		"\1%gs\1" },
  { R_CR0,		"\1%cr0\1" },
  { R_CR1,		"\1%cr1\1" },
  { R_CR2,		"\1%cr2\1" },
  { R_CR3,		"\1%cr3\1" },
  { R_CR4,		"\1%cr4\1" },
  { R_CR5_INV,		"\0" },
  { R_CR6_INV,		"\0" },
  { R_CR7_INV,		"\0" },
  { R_DR0,		"\1%dr0\1" },
  { R_DR1,		"\1%dr1\1" },
  { R_DR2,		"\1%dr2\1" },
  { R_DR3,		"\1%dr3\1" },
  { R_DR4_INV,		"\0" },
  { R_DR5_INV,		"\0" },
  { R_DR6,		"\1%dr6\1" },
  { R_DR7,		"\1%dr7\1" },
  { R_MM0,		"\1%mm0\1" },
  { R_MM1,		"\1%mm1\1" },
  { R_MM2,		"\1%mm2\1" },
  { R_MM3,		"\1%mm3\1" },
  { R_MM4,		"\1%mm4\1" },
  { R_MM5,		"\1%mm5\1" },
  { R_MM6,		"\1%mm6\1" },
  { R_MM7,		"\1%mm7\1" },
  { R_ST0,		"\1%st(0)\1" },
  { R_ST1,		"\1%st(1)\1" },
  { R_ST2,		"\1%st(2)\1" },
  { R_ST3,		"\1%st(3)\1" },
  { R_ST4,		"\1%st(4)\1" },
  { R_ST5,		"\1%st(5)\1" },
  { R_ST6,		"\1%st(6)\1" },
  { R_ST7,		"\1%st(7)\1" },
  { R_XMM0,		"\1%xmm0\1" },
  { R_XMM1,		"\1%xmm1\1" },
  { R_XMM2,		"\1%xmm2\1" },
  { R_XMM3,		"\1%xmm3\1" },
  { R_XMM4,		"\1%xmm4\1" },
  { R_XMM5,		"\1%xmm5\1" },
  { R_XMM6,		"\1%xmm6\1" },
  { R_XMM7,		"\1%xmm7\1" },
  { R_XMM8,		"\1%xmm8\1" },
  { R_XMM9,		"\1%xmm9\1" },
  { R_XMM10,		"\1%xmm10\1" },
  { R_XMM11,		"\1%xmm11\1" },
  { R_XMM12,		"\1%xmm12\1" },
  { R_XMM13,		"\1%xmm13\1" },
  { R_XMM14,		"\1%xmm14\1" },
  { R_XMM15,		"\1%xmm15\1" },
  { I_BYTE, 		"\1$\1\12"  },
  { I_WORD,		"\1$\1\13"  },
  { I_DWRD,		"\1$\1\14"  },
  { J_IMM, 		"\1$\1\5"   },
  { P_32,		"\1$\1\30\1, $\1\31" },
  { P_48,		"\1$\1\32\1, $\1\33" },
  { PRINT_I,		"\1$\1\50" }
};


/* 64bit regs */
static char* att_q_regs[] = {
	"%rax",  "%rcx",  "%rdx",  "%rbx",  "%rsp",  "%rbp",  "%rsi",  "%rdi",
	"%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
};

/* 32bit regs */
static char* att_d_regs[] = {
	"%eax",  "%ecx",  "%edx",  "%ebx", "%esp",  "%ebp",  "%esi",  "%edi"
};

/* segment regs */
static char* att_s_regs[] = {
	"%es", "%cs",  "%ss",  "%ds",  "%fs",  "%gs"
};

/* operand cast */
static void att_opr_cast(ud_t* ud, ud_operand_size_t size, int* syn_cast)
{
	if(*syn_cast) {
		switch(size) {
			case SZ_w : case SZ_d :
			case SZ_wp: case SZ_dp:
				mkasm(ud, "*");   break;
			default: break;
	}
	(*syn_cast)--;
  }
}

/* SIB */
static void att_sib(ud_t* ud, unsigned int n)
{
	mkasm(ud, "%s, %s", 
		att_d_regs[(ud->operand[n].sib.base)],
		att_d_regs[(ud->operand[n].sib.index)]);
	if (n_scale[ud->operand[n].sib.scale] > 1)
		mkasm(ud,  ", %d", n_scale[ud->operand[n].sib.scale]);
}

/* SIB(64) */
static void att_rsib(ud_t* ud, unsigned int n)
{
	mkasm(ud, "%s, %s", 
		att_q_regs[(ud->operand[n].sib.base)],
		att_q_regs[(ud->operand[n].sib.index)]);
  if (n_scale[ud->operand[n].sib.scale] > 1)
	mkasm(ud, ", %d", 
		n_scale[ud->operand[n].sib.scale]);
}

/* SI */
static void att_si(ud_t* ud, unsigned int n)
{
	mkasm(ud, ", %s, %d", 
		att_d_regs[(ud->operand[n].sib.index)],
  		n_scale[ud->operand[n].sib.scale]);
}

/* SI(64) */
static void att_rsi(ud_t* ud, unsigned int n)
{
	mkasm(ud, ", %s, %d", 
		att_q_regs[(ud->operand[n].sib.index)],
  		n_scale[ud->operand[n].sib.scale]);
}

/* read only static plugin wrapper */
ud_syntax_plugin_t plugin_att = {
	att_opr_cast,
	NULL,
	att_sib,
	att_si,
	att_rsib,
	att_rsi,
	att_s_regs,
	att_d_regs,
	att_q_regs,
	att_smap
};

/* translates to at&t syntax */
extern void ud_translate_att(ud_t *ud)
{
	int syn_cast;

	/* address mode override */
	if (ud->prefix.opr)
		mkasm(ud,  "o32 ");
	/* operand mode override */
	if (ud->prefix.adr)
		mkasm(ud,  "a32 ");
	/* lock prefix */
	if (ud->prefix.lock)
		mkasm(ud,  "lock ");
	/* rep prefix */
	if (ud->prefix.rep)
		mkasm(ud,  "rep ");
	/* repne prefix */
	if (ud->prefix.repne)
		mkasm(ud,  "repne ");
	/* special instructions */
	if (ud->mnemonic == Iretf)
		mkasm(ud, "lret ");
	/* defined byte */
	if (ud->mnemonic == Idb) {
		mkasm(ud, ".byte 0x%x", ud->operand[0].lval.bits8u);
		return;
	 }  else {
		if (ud->mnemonic == Ijmp || ud->mnemonic == Icall ) {
			if ( ud->operand[0].size == SZ_dp || ud->operand[0].size == SZ_wp )
				mkasm(ud,  "l");
			syn_cast = 1;
		}
		/* print the instruction mnemonic */
		mkasm(ud, "%s", ud_mnemonics[ud->mnemonic]);
	}

	/* att instruction suffix */
	if ( ud->operand[0].size == SZ_b || ud->operand[1].size == SZ_b)
		mkasm(ud, "b");
	else
	if (ud->operand[0].size == SZ_w || ud->operand[1].size == SZ_w)
		mkasm(ud, "w");
	else
	if (ud->operand[0].size == SZ_d  || ud->operand[1].size == SZ_d ||
		ud->operand[1].size == SZ_dp || ud->operand[0].size == SZ_dp)
		mkasm(ud, "l");
	else
	if (ud->operand[0].size == SZ_q  || ud->operand[1].size == SZ_q )
		mkasm(ud, "q");

	mkasm(ud, " ");

	if ( ud->mnemonic == Ibound || ud->mnemonic == Ienter) {
		if (ud->operand[0].type != -1)
			ud_syntax_map_script(ud, &plugin_att, &syn_cast, 0, att_smap[ud->operand[0].type].script);
		if (ud->operand[1].type != -1) {
			mkasm(ud, ",");
			ud_syntax_map_script(ud, &plugin_att, &syn_cast, 1, att_smap[ud->operand[1].type].script);
		}
	} else {

		/* operand 1 */
		if (ud->operand[2].type != -1) {
			ud_syntax_map_script(ud, &plugin_att, &syn_cast, 2, att_smap[ud->operand[2].type].script);
  			mkasm(ud, ", ");
        	}
		/* operand 2 */
		if (ud->operand[1].type!=-1) {
			ud_syntax_map_script(ud, &plugin_att, &syn_cast, 1, att_smap[ud->operand[1].type].script);
			mkasm(ud, ", ");
		}
		/* operand 3 */
		if (ud->operand[0].type!=-1)
			ud_syntax_map_script(ud, &plugin_att, &syn_cast, 0, att_smap[ud->operand[0].type].script);
	}
}
