/* -----------------------------------------------------------------------------
 * udis86.c
 *
 * Copyright (c) 2004, 2005, 2006, Vivek Mohan <vivek@sig9.com>
 * All rights reserved. See LICENSE
 * -----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <udis86.h>

#include "input.h"
#include "extern.h"

/* =============================================================================
 * ud_init() - Initializes ud_t object.
 * =============================================================================
 */
extern void ud_init(struct ud* u)
{
  memset((void*)u, 0, sizeof(struct ud));
  u->dis_mode = UD_MODE16;
  u->mnemonic = UD_Iinvalid;
  
  ud_set_input_file(u, stdin);
}

/* =============================================================================
 * ud_disassemble() - disassembles one instruction and returns the number of 
 * bytes disassembled. A zero means end of disassembly.
 * =============================================================================
 */
extern unsigned int ud_disassemble(struct ud* u, ud_type_t syn)
{
  if (ud_input_end(u))
	return 0;

  if (ud_decode(u) == 0)
	return 0;
 
  if (syn == UD_SYN_ATT)
	ud_translate_att(u);
  else  ud_translate_intel(u);

  return ud_asm_count(u);
}

/* =============================================================================
 * ud_set_dis_mod() - Set Disassemly Mode.
 * =============================================================================
 */
extern void ud_set_dis_mode(struct ud* u, ud_type_t mode)
{
  switch(mode) {
	case UD_MODE16:
	case UD_MODE32:
	case UD_MODE64:
		u->dis_mode = mode;
		break;
	default:
		u->dis_mode = UD_MODE16;
  }
}


/* =============================================================================
 * ud_set_origin() - Sets code origin. 
 * =============================================================================
 */
extern void ud_set_origin(struct ud* u, unsigned int origin)
{
  u->pc = origin;
}


/* =============================================================================
 * ud_asm() - returns the disassembled instruction
 * =============================================================================
 */
extern char* ud_asm(struct ud* u) 
{
  return u->asm_buffer;
}

/* =============================================================================
 * ud_asm_offset() - Returns the offset.
 * =============================================================================
 */
extern unsigned int ud_asm_offset(struct ud* u) 
{
  return u->asm_offset;
}


/* =============================================================================
 * ud_asm_next() - Returns hex form of disassembled instruction.
 * =============================================================================
 */
extern char* ud_asm_hex(struct ud* u) 
{
  return u->asm_hexcode;
}


/* =============================================================================
 * ud_asm_code() - Returns code disassembled.
 * =============================================================================
 */
extern unsigned char* ud_asm_code(struct ud* u) 
{
  return u->inp_sess;
}


/* =============================================================================
 * ud_asm_count() - Returns the count of bytes disassembled.
 * =============================================================================
 */
extern unsigned int ud_asm_count(struct ud* u) 
{
  return u->inp_ctr;
}
