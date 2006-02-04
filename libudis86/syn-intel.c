/* syn-intel.c
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#include "types.h"
#include "macros.h"
#include "extern.h"
#include "opcmap.h"
#include "syn.h"

static uint8_t n_scale[] = { 1, 2, 4, 8 };

/* INTEL Syntax Operand Decoder Map
 * ================================
 * WARNING -- Make sure the order of declaration is the same as
 * that of the enumeration of operand codes in x86.h
 */
ud_syntax_map_t intel_smap[] = {
  { M_BX_SI,		"\60\1[\1\6\1bx+si]\1" },
  { M_BX_DI,		"\60\1[\1\6\1bx+di]\1" },
  { M_BP_SI,		"\60\1[\1\6\1bp+si]\1" },
  { M_BP_DI,		"\60\1[\1\6\1bp+di]\1" },
  { M_SI,		"\60\1[\1\6\1si]\1" },
  { M_DI,		"\60\1[\1\6\1di]\1" },
  { M_OFF16,		"\60\1[\1\6\1\1\3\1]\1" },
  { M_BX,		"\60\1[\1\6\1bx]\1" },
  { M_BX_SI_OFF8,	"\60\1[\1\6\1bx+si\1\2\1]\1" },
  { M_BX_DI_OFF8, 	"\60\1[\1\6\1bx+di\1\2\1]\1" },
  { M_BP_SI_OFF8,	"\60\1[\1\6\1bp+si\1\2\1]\1" },
  { M_BP_DI_OFF8,	"\60\1[\1\6\1bp+di\1\2\1]\1" },
  { M_SI_OFF8, 		"\60\1[\1\6\1si\1\2\1]\1" },
  { M_DI_OFF8, 		"\60\1[\1\6\1di\1\2\1]\1" },
  { M_BP_OFF8, 		"\60\1[\1\6\1bp\1\2\1]\1" },
  { M_BX_OFF8,		"\60\1[\1\6\1bx\1\2\1]\1" },
  { M_BX_SI_OFF16, 	"\60\1[\1\6\1bx+si+\1\3\1]\1" },
  { M_BX_DI_OFF16, 	"\60\1[\1\6\1bx+di+\1\3\1]\1" },
  { M_BP_SI_OFF16, 	"\60\1[\1\6\1bp+si+\1\3\1]\1" },
  { M_BP_DI_OFF16,	"\60\1[\1\6\1bp+di+\1\3\1]\1" },
  { M_SI_OFF16, 	"\60\1[\1\6\1si+\1\3\1]\1" },
  { M_DI_OFF16, 	"\60\1[\1\6\1di+\1\3\1]\1" },
  { M_BP_OFF16, 	"\60\1[\1\6\1bp+\1\3\1]\1" },
  { M_BX_OFF16,		"\60\1[\1\6\1bx+\1\3\1]\1" },
  { M_EAX,		"\60\1[\1\6\1eax]\1" },
  { M_ECX,		"\60\1[\1\6\1ecx]\1" },
  { M_EDX,		"\60\1[\1\6\1edx]\1" },
  { M_EBX,		"\60\1[\1\6\1ebx]\1" },
  { M_SIB,		"\60\1[\1\6\020\1]\1" },
  { M_OFF32,		"\60\1[\1\6\4\1]\1" },
  { M_ESI, 		"\60\1[\1\6\1esi]\1" },
  { M_EDI,		"\60\1[\1\6\1edi]\1" },
  { M_EAX_OFF8, 	"\60\1[\1\6\1eax\1\2\1]\1" },
  { M_ECX_OFF8, 	"\60\1[\1\6\1ecx\1\2\1]\1" },
  { M_EDX_OFF8, 	"\60\1[\1\6\1edx\1\2\1]\1" },
  { M_EBX_OFF8, 	"\60\1[\1\6\1ebx\1\2\1]\1" },
  { M_SIB_OFF8,		"\60\1[\1\6\020\2\1]\1" },
  { M_EBP_OFF8, 	"\60\1[\1\6\1ebp\1\2\1]\1" },
  { M_ESI_OFF8, 	"\60\1[\1\6\1esi\1\2\1]\1" },
  { M_EDI_OFF8, 	"\60\1[\1\6\1edi\1\2\1]\1" },
  { M_EAX_OFF32, 	"\60\1[\1\6\1eax+\1\4\1]\1" },
  { M_ECX_OFF32, 	"\60\1[\1\6\1ecx+\1\4\1]\1" },
  { M_EDX_OFF32, 	"\60\1[\1\6\1edx+\1\4\1]\1" },
  { M_EBX_OFF32, 	"\60\1[\1\6\1ebx+\1\4\1]\1" },
  { M_SIB_OFF32,	"\60\1[\1\6\020\1+\1\4\1]\1" },
  { M_EBP_OFF32, 	"\60\1[\1\6\1ebp+\1\4\1]\1" },
  { M_ESI_OFF32, 	"\60\1[\1\6\1esi+\1\4\1]\1" },
  { M_EDI_OFF32, 	"\60\1[\1\6\1edi+\1\4\1]\1" },
  { M_SID,		"\60\1[\1\6\021\1+\1\4\1]\1" },
  { M_RAX,		"\60\1[\1\6\1rax]\1" },
  { M_RCX,		"\60\1[\1\6\1rcx]\1" },
  { M_RDX,		"\60\1[\1\6\1rdx]\1" },
  { M_RBX,		"\60\1[\1\6\1rbx]\1" },
  { M_RSIB,		"\60\1[\1\6\022\1]\1" },
  { M_ROFF32,		"\60\1[\1\6\4\1]\1" },
  { M_RSI, 		"\60\1[\1\6\1rsi]\1" },
  { M_RDI,		"\60\1[\1\6\1rdi]\1" },
  { M_R8,		"\60\1[\1\6\1r8]\1" },
  { M_R9,		"\60\1[\1\6\1r9]\1" },
  { M_R10,		"\60\1[\1\6\1r10]\1" },
  { M_R11,		"\60\1[\1\6\1r11]\1" },
  { M_R12,		"\60\1[\1\6\1r12]\1" },
  { M_R13,		"\60\1[\1\6\1r13]\1" },
  { M_R14, 		"\60\1[\1\6\1r14]\1" },
  { M_R15,		"\60\1[\1\6\1r15]\1" },
  { M_RAX_OFF8, 	"\60\1[\1\6\1rax\1\2\1]\1" },
  { M_RCX_OFF8, 	"\60\1[\1\6\1rcx\1\2\1]\1" },
  { M_RDX_OFF8, 	"\60\1[\1\6\1rdx\1\2\1]\1" },
  { M_RBX_OFF8, 	"\60\1[\1\6\1rbx\1\2\1]\1" },
  { M_RSIB_OFF8,	"\60\1[\1\6\022\2\1]\1" },
  { M_RBP_OFF8, 	"\60\1[\1\6\1rbp\1\2\1]\1" },
  { M_RSI_OFF8, 	"\60\1[\1\6\1rsi\1\2\1]\1" },
  { M_RDI_OFF8, 	"\60\1[\1\6\1rdi\1\2\1]\1" },
  { M_R8_OFF8,		"\60\1[\1\6\1r8\1\2\1]\1" },
  { M_R9_OFF8,		"\60\1[\1\6\1r9\1\2\1]\1" },
  { M_R10_OFF8, 	"\60\1[\1\6\1r10\1\2\1]\1" },
  { M_R11_OFF8, 	"\60\1[\1\6\1r11\1\2\1]\1" },
  { M_R12_OFF8,		"\60\1[\1\6\1r12\1\2\1]\1" },
  { M_R13_OFF8, 	"\60\1[\1\6\1r13\1\2\1]\1" },
  { M_R14_OFF8, 	"\60\1[\1\6\1r14\1\2\1]\1" },
  { M_R15_OFF8, 	"\60\1[\1\6\1r15\1\2\1]\1" },
  { M_RAX_OFF32, 	"\60\1[\1\6\1rax+\1\4\1]\1" },
  { M_RCX_OFF32, 	"\60\1[\1\6\1rcx+\1\4\1]\1" },
  { M_RDX_OFF32, 	"\60\1[\1\6\1rdx+\1\4\1]\1" },
  { M_RBX_OFF32, 	"\60\1[\1\6\1rbx+\1\4\1]\1" },
  { M_SIB_OFF32,	"\60\1[\1\6\022\1+\1\4\1]\1" },
  { M_RBP_OFF32, 	"\60\1[\1\6\1rbp+\1\4\1]\1" },
  { M_RSI_OFF32, 	"\60\1[\1\6\1rsi+\1\4\1]\1" },
  { M_RDI_OFF32, 	"\60\1[\1\6\1rdi+\1\4\1]\1" },
  { M_R8_OFF32, 	"\60\1[\1\6\1r8+\1\4\1]\1" },
  { M_R9_OFF32, 	"\60\1[\1\6\1r9+\1\4\1]\1" },
  { M_R10_OFF32, 	"\60\1[\1\6\1r10+\1\4\1]\1" },
  { M_R11_OFF32, 	"\60\1[\1\6\1r11+\1\4\1]\1" },
  { M_R12_OFF32,	"\60\1[\1\6\1r12+\1\4\1]\1" },
  { M_R13_OFF32, 	"\60\1[\1\6\1r13+\1\4\1]\1" },
  { M_R14_OFF32, 	"\60\1[\1\6\1r14+\1\4\1]\1" },
  { M_R15_OFF32, 	"\60\1[\1\6\1r15+\1\4\1]\1" },
  { M_RSID,		"\60\1[\1\6\023\1+\1\4\1]\1" },
  { OFFSET16,		"\1[\1\40\1]\1" },
  { OFFSET32,		"\1[\1\41\1]\1" },
  { OFFSET64,		"\1[\1\42\1]\1" },
  { R_AL, 		"\1al\1" },
  { R_CL, 		"\1cl\1" },
  { R_DL, 		"\1dl\1" },
  { R_BL, 		"\1bl\1" },
  { R_AH, 		"\1ah\1" },
  { R_CH, 		"\1ch\1" },
  { R_DH, 		"\1dh\1" },
  { R_BH, 		"\1bh\1" },
  { R_SPL,		"\1spl\1"},
  { R_BPL,		"\1bpl\1"},
  { R_SIL,		"\1sil\1"},
  { R_DIL,		"\1dil\1"},
  { R_R8B,		"\1r8b\1"},
  { R_R9B,		"\1r9b\1"},
  { R_R10B,		"\1r10b\1"},
  { R_R11B,		"\1r11b\1"},
  { R_R12B,		"\1r12b\1"},
  { R_R13B,		"\1r13b\1"},
  { R_R14B,		"\1r14b\1"},
  { R_R15B,		"\1r15b\1"},
  { R_AX, 		"\1ax\1" },
  { R_CX, 		"\1cx\1" },
  { R_DX, 		"\1dx\1" },
  { R_BX, 		"\1bx\1" },
  { R_SP, 		"\1sp\1" },
  { R_BP, 		"\1bp\1" },
  { R_SI, 		"\1si\1" },
  { R_DI, 		"\1di\1" },
  { R_R8W,		"\1r8w\1"},
  { R_R9W,		"\1r9w\1"},
  { R_R10W,		"\1r10w\1"},
  { R_R11W,		"\1r11w\1"},
  { R_R12W,		"\1r12w\1"},
  { R_R13W,		"\1r13w\1"},
  { R_R14W,		"\1r14w\1"},
  { R_R15W,		"\1r15w\1"},
  { R_EAX, 		"\1eax\1"},
  { R_ECX, 		"\1ecx\1"},
  { R_EDX, 		"\1edx\1"},
  { R_EBX, 		"\1ebx\1"},
  { R_ESP, 		"\1esp\1"},
  { R_EBP, 		"\1ebp\1"},
  { R_ESI, 		"\1esi\1"},
  { R_EDI, 		"\1edi\1"},
  { R_R8D,		"\1r8d\1"},
  { R_R9D,		"\1r9d\1"},
  { R_R10D,		"\1r10d\1"},
  { R_R11D,		"\1r11d\1"},
  { R_R12D,		"\1r12d\1"},
  { R_R13D,		"\1r13d\1"},
  { R_R14D,		"\1r14d\1"},
  { R_R15D,		"\1r15d\1"},
  { R_RAX, 		"\1rax\1"},
  { R_RCX, 		"\1rcx\1"},
  { R_RDX, 		"\1rdx\1"},
  { R_RBX, 		"\1rbx\1"},
  { R_RSP, 		"\1rsp\1"},
  { R_RBP, 		"\1rbp\1"},
  { R_RSI, 		"\1rsi\1"},
  { R_RDI, 		"\1rdi\1"},
  { R_R8,		"\1r8\1"},
  { R_R9,		"\1r9\1"},
  { R_R10,		"\1r10\1"},
  { R_R11,		"\1r11\1"},
  { R_R12,		"\1r12\1"},
  { R_R13,		"\1r13\1"},
  { R_R14,		"\1r14\1"},
  { R_R15,		"\1r15\1"},
  { R_ES,		"\1es\1" },
  { R_CS,		"\1cs\1" },
  { R_SS,		"\1ss\1" },
  { R_DS,		"\1ds\1" },
  { R_FS,		"\1fs\1" },
  { R_GS,		"\1gs\1" },
  { R_CR0,		"\1cr0\1" },
  { R_CR1,		"\1cr1\1" },
  { R_CR2,		"\1cr2\1" },
  { R_CR3,		"\1cr3\1" },
  { R_CR4,		"\1cr4\1" },
  { R_CR5_INV,		"\0" },
  { R_CR6_INV,		"\0" },
  { R_CR7_INV,		"\0" },
  { R_DR0,		"\1dr0\1" },
  { R_DR1,		"\1dr1\1" },
  { R_DR2,		"\1dr2\1" },
  { R_DR3,		"\1dr3\1" },
  { R_DR4_INV,		"\0" },
  { R_DR5_INV,		"\0" },
  { R_DR6,		"\1dr6\1" },
  { R_DR7,		"\1dr7\1" },
  { R_MM0,		"\1mm0\1" },
  { R_MM1,		"\1mm1\1" },
  { R_MM2,		"\1mm2\1" },
  { R_MM3,		"\1mm3\1" },
  { R_MM4,		"\1mm4\1" },
  { R_MM5,		"\1mm5\1" },
  { R_MM6,		"\1mm6\1" },
  { R_MM7,		"\1mm7\1" },
  { R_ST0,		"\1st0\1" },
  { R_ST1,		"\1st1\1" },
  { R_ST2,		"\1st2\1" },
  { R_ST3,		"\1st3\1" },
  { R_ST4,		"\1st4\1" },
  { R_ST5,		"\1st5\1" },
  { R_ST6,		"\1st6\1" },
  { R_ST7,		"\1st7\1" },
  { R_XMM0,		"\1xmm0\1" },
  { R_XMM1,		"\1xmm1\1" },
  { R_XMM2,		"\1xmm2\1" },
  { R_XMM3,		"\1xmm3\1" },
  { R_XMM4,		"\1xmm4\1" },
  { R_XMM5,		"\1xmm5\1" },
  { R_XMM6,		"\1xmm6\1" },
  { R_XMM7,		"\1xmm7\1" },
  { R_XMM8,		"\1xmm8\1" },
  { R_XMM9,		"\1xmm9\1" },
  { R_XMM10,		"\1xmm10\1" },
  { R_XMM11,		"\1xmm11\1" },
  { R_XMM12,		"\1xmm12\1" },
  { R_XMM13,		"\1xmm13\1" },
  { R_XMM14,		"\1xmm14\1" },
  { R_XMM15,		"\1xmm15\1" },
  { I_BYTE, 		"\60\12"  },
  { I_WORD,		"\60\13"  },
  { I_DWRD,		"\60\14"  },
  { J_IMM, 		"\60\5"	  },
  { P_32,		"\60\30\1:\1\31" },
  { P_48,		"\60\32\1:\1\33" },
  { PRINT_I,		"\50"},
  { M_ESP,		"\60\1[\1\6\1esp]\1" },
  { M_ESP_OFF8, 	"\60\1[\1\6\1esp\1\2\1]\1" },
  { M_ESP_OFF32, 	"\60\1[\1\6\1esp+\1\4\1]\1" },
  { M_RSP,		"\60\1[\1\6\1rsp]\1" },
  { M_RSP_OFF8, 	"\60\1[\1\6\1rsp\1\2\1]\1" },
  { M_RSP_OFF32, 	"\60\1[\1\6\1rsp+\1\4\1]\1" }
};

/* 64bit regs */
static char* intel_q_regs[] = {
	"rax",  "rcx",  "rdx",  "rbx",  "rsp",  "rbp",  "rsi",  "rdi",
	"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};

/* 32bit regs */
static char* intel_d_regs[] = {
	"eax",  "ecx",  "edx",  "ebx",  "esp",  "ebp",  "esi",  "edi",
	"r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"
};

/* segment registers */
static char* intel_s_regs[] = {
	"es", "cs",  "ss",  "ds",  "fs",  "gs"
};

/* Operand Cast */
static void intel_opr_cast(ud_t* ud, ud_operand_size_t size, int* syn_cast)
{
	if(*syn_cast) {
		switch(size) {
			case SZ_sb:
			case SZ_b : 
				mkasm(ud, "byte " ); 
				break;
			case SZ_w : 
				mkasm(ud, "word " ); 
				break;
			case SZ_d : 
				mkasm(ud, "dword "); 
				break;
			case SZ_wp: 
				mkasm(ud, "word far "); 
				break;
			case SZ_dp: 
				mkasm(ud, "dword far "); 
				break;
			case SZ_q : 
				mkasm(ud, "qword "); 
				break;
			default:
				break;
		}
		(*syn_cast)--;
  	}
}

/* Relative offset cast */
static void intel_rel_cast(ud_t* ud, ud_operand_size_t size, int* syn_cast)
{
	switch(size) {
		case SZ_b: 
			mkasm(ud, "short "); 
			break;
		case SZ_w: 
			mkasm(ud, "near " ); 
			break;
		case SZ_d: 
			mkasm(ud, "dword "); 
			break;
		default:
			break;
	}
}

/* SIB */
static void intel_sib(ud_t* ud, unsigned int n)
{
	mkasm(ud, "%s+%s", 
		intel_d_regs[(ud->operand[n].sib.base)],
		intel_d_regs[(ud->operand[n].sib.index)]);
	if (n_scale[ud->operand[n].sib.scale] > 1)
		mkasm(ud, "*%d", n_scale[ud->operand[n].sib.scale]);
}

/* SIB(64) */
static void intel_rsib(ud_t* ud, unsigned int n)
{
	mkasm(ud, "%s+%s", 
		intel_q_regs[(ud->operand[n].sib.base)],
		intel_q_regs[(ud->operand[n].sib.index)]);
	if (n_scale[ud->operand[n].sib.scale] > 1)
		mkasm(ud, "*%d", n_scale[ud->operand[n].sib.scale]);
}

/* SI - scaleed Index*/
static void intel_si(ud_t *ud, unsigned int n)
{
	mkasm(ud, "%s*%d",
		intel_d_regs[(ud->operand[n].sib.index)],
  		n_scale[ud->operand[n].sib.scale]);
}

/* SI(64) */
static void intel_rsi(ud_t *ud, unsigned int n)
{
	mkasm(ud, "%s*%d", 
		intel_q_regs[(ud->operand[n].sib.index)],
  		n_scale[ud->operand[n].sib.scale]);
}

/* read only static plugin wrapper */
ud_syntax_plugin_t plugin_intel = {
	intel_opr_cast,
	intel_rel_cast,
	intel_sib,
	intel_si,
	intel_rsib,
	intel_rsi,
	intel_s_regs,
	intel_d_regs,
	intel_q_regs,
	intel_smap
};


/* translates to intel syntax */
extern void ud_translate_intel(ud_t *ud)
{
	int syn_cast = 1;

	if (ud->operand[0].size != ud->operand[1].size && 
	    	ud->operand[2].type == -1)
		syn_cast = 2;
	else
	if (ud->operand[2].type != -1 && 
		((ud->operand[0].size != ud->operand[1].size) ||
		 (ud->operand[1].size != ud->operand[2].size) || 
		 (ud->operand[0].size != ud->operand[2].size)))
		syn_cast = 2;

	/* address mode override */
	if (ud->prefix.opr)
		mkasm(ud, "o32 ");
	/* operand mode override */
	if (ud->prefix.adr)
		mkasm(ud, "a32 ");
	/* lock prefix */
	if (ud->prefix.lock)
		mkasm(ud, "lock ");
	/* rep prefix */
	if (ud->prefix.rep)
		mkasm(ud, "rep ");
	/* repne prefix */
	if (ud->prefix.repne)
		mkasm(ud, "repne ");

	/* print the instruction mnemonic */
	mkasm(ud, "%s", ud_mnemonics[ud->mnemonic]);

	/* string operation suffix */
	if (ud->suffix) {
		switch(ud->operand[0].size) {
			case SZ_w : mkasm(ud, "w"); break;
			case SZ_d : mkasm(ud, "d"); break;
			case SZ_q : mkasm(ud, "q"); break;
			default:
				break;
		}
	}

	/* space between insn menmonic and operands */
	mkasm(ud, " ");

	/* operand 1 */
	if (ud->operand[0].type != -1) {
		ud_syntax_map_script(ud, &plugin_intel, &syn_cast, 0, intel_smap[ud->operand[0].type].script);
		/* operand 2 */
		if (ud->operand[1].type != -1) {
			mkasm(ud, ", ");
			ud_syntax_map_script(ud, &plugin_intel, &syn_cast, 1, intel_smap[ud->operand[1].type].script);
			/* operand 3 */
			if (ud->operand[2].type != -1) {
				mkasm(ud, ", ");
				ud_syntax_map_script(ud, &plugin_intel, &syn_cast, 2, intel_smap[ud->operand[2].type].script);
			}
		}
	}
}
