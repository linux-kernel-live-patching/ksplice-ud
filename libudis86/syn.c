/* syn.c
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <udis86.h>
#include "syn.h"
#include "opcmap.h"

/* Decoder Script Directives (In Octal Number System)
   ==================================================
   \1  - verbatim print delimiter
   \2  - print  8 bit sign extended offset
   \3  - print 16 bit offset
   \4  - print 32 bit offset
   \5  - print rel offset
   \6  - print seg override
   \7  - print rel cast
   \10 - print 64 bit offset
   \12 - print 8bit imm
   \13 - print 16bit imm
   \14 - print 32bit imm

   \20 - print SIB
   \21 - print SID
   \22 - print RSIB
   \23 - print RSID

   \26 - print ptr cast

   \30 - print 16 bit segment of ptr32
   \31 - print 16 bit offset of ptr32
   \32 - print 16 bit segment of ptr48
   \33 - print 32 bit offset of ptr48

   \40 - print 16b offset
   \41 - print 32b offset
   \42 - print 64b offset

   \50 - print lval

   \60 - print operand cast
   \61 - print jmp cast
*/

extern void mkasm(ud_t* ud, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	ud->asmout.fill_ptr += vsprintf(ud->asmout.buffer + ud->asmout.fill_ptr, fmt, ap);
	va_end(ap);
}

/* syntax map script interpreter */
extern void ud_syntax_map_script(
		register ud_t* ud, 
		ud_syntax_plugin_t* syn, 
		int* syn_cast, 
		unsigned int n, 
		char* script)
{
	register ud_operand_t* op = &ud->operand[n];

	for (;*(script); ++script) {
		switch (*script) {
			case 01: /* verbatim print delimiter */
				for (++script;*(script) != 01; ++script)
					mkasm(ud, "%c", *script);
				break;			
			case 02: /* print 8 bit offset (sign extended) */
				if (op->lval.bits8 < 0 )
					mkasm(ud, "-0x%x", -1 * op->lval.bits8);
				else	mkasm(ud, "+0x%x",  op->lval.bits8);
				break;
			case 03: /* print 16 bit offset */
				mkasm(ud, "0x%x", op->lval.bits16 & 0xFFFF);
				break;
			case 04: /* print 32 bit offset */
				mkasm(ud, "0x%lx", op->lval.bits32 & 0xFFFFFFFF);
				break;
			case 010:/* print 64 bit offset */
				mkasm(ud, "0x%llx", op->lval.bits64);
				break;
			case 05:/* relative offset */
				switch(op->size) {
					case SZ_jb:
						mkasm(ud, "0x%lx", ud->pc + op->lval.bits8);
						break;
					case SZ_jw:
						mkasm(ud, "0x%lx", ud->pc + op->lval.bits16);
						break;
					case SZ_jd:
						mkasm(ud, "0x%lx", ud->pc + op->lval.bits32);
						break;
					default :
						mkasm(ud, ":%d", op->size);
				}
				break;
			case 06: /* print segment override */
				if (ud->prefix.seg)
					mkasm(ud, "%s:", syn->s_regs[(ud->prefix.seg) - R_ES]);
				break;
			case 07: /* relative cast */
				syn->rel_cast(ud, n, syn_cast);
				break;
			case 012: /* print 8bit imm */
				if (op->size == SZ_sb) {
					if (op->lval.bits8 < 0)
						mkasm(ud, "-0x%x", -1 * op->lval.bits8);
					else	mkasm(ud, "+0x%x", op->lval.bits8);
				} else mkasm(ud, "0x%x", op->lval.bits8u);
				break;
			case 013: /* print 16bit imm */
				mkasm(ud, "0x%x", op->lval.bits16u);
				break;
			case 014: /* print 32bit imm */
				mkasm(ud, "0x%lx", op->lval.bits32u);
				break;
			case 020: /* print sib */
				syn->sib(ud, n);
				break;
			case 021: /* print si */
				syn->si(ud, n);
				break;
			case 022: /* print rsib */
				syn->rsib(ud, n);
				break;
			case 023: /* print rsi */
				syn->rsi(ud, n);
				break;
			case 030: /* print 16 bit segment of ptr32 */
				mkasm(ud, "0x%x", op->lval.ptr.seg);
				break;
			case 031: /* print 16 bit offset of ptr32 */
				mkasm(ud, "0x%x", op->lval.ptr.off & 0xFFFF);
				break;
			case 032: /* print 16 bit segment of ptr48 */
				mkasm(ud, "0x%x", op->lval.ptr.seg);
				break;
			case 033: /* print 32 bit offset of ptr48 */
				mkasm(ud, "0x%x", op->lval.ptr.off);
				break;
			case 040: /* print offset 16b */
				mkasm(ud, "0x%x", op->lval.bits16 & 0xFFFF);
				break;
			case 041: /* print offset 32b */
				mkasm(ud, "0x%lx", op->lval.bits32);
				break;
			case 042: /* print offset 64b */
				mkasm(ud, "0x%llx", op->lval.bits64);
				break;
			case 050: /* print lval */
				mkasm(ud, "0x%lx", op->lval.bits32);
				break;
			case 060: /* print operand cast */
				if (ud->cast)
					syn->opr_cast(ud, op->size, syn_cast);
				break;
		}
  	}	
}
