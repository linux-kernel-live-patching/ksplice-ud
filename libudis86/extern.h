/* -----------------------------------------------------------------------------
 * extern.h
 *
 * Copyright (c) 2004, 2005, 2006, Vivek Mohan <vivek@sig9.com>
 * All rights reserved. See LICENSE
 * -----------------------------------------------------------------------------
 */
#ifndef UD_EXTERN_H
#define UD_EXTERN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <inttypes.h>
#include "types.h"

/* ============================= PUBLIC API ================================= */


extern void ud_init(struct ud*);

extern void ud_search_map(struct ud*);

extern void ud_set_dis_mode(struct ud*, ud_type_t);

extern void ud_set_origin(struct ud*, unsigned int);

extern void ud_set_input_hook(struct ud*, int (*)(struct ud*));

extern void ud_set_input_buffer(struct ud*, unsigned char*, size_t);

extern void ud_set_input_file(struct ud*, FILE*);

extern void ud_input_skip(struct ud*, size_t);

extern int ud_input_end(struct ud*);

extern unsigned int ud_decode(struct ud*);

extern unsigned int ud_disassemble(struct ud*, ud_type_t);

extern void ud_translate_intel(struct ud*);

extern void ud_translate_att(struct ud*);

extern char* ud_asm(struct ud* u);

extern unsigned int ud_asm_offset(struct ud*);

extern char* ud_asm_hex(struct ud*);

extern unsigned char* ud_asm_code(struct ud* u);

extern unsigned int ud_asm_count(struct ud* u);

extern const char* ud_lookup_mnemonic(ud_mnemonic_code_t c);

/* ========================================================================== */

#ifdef __cplusplus
}
#endif
#endif
