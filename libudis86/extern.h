/* extern.h
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#ifndef _UD_EXTERN_H_
#define _UD_EXTERN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <inttypes.h>
#include "types.h"

/*-----------------------------------------------------------------------------
 * Level-1 Wrappers
 *-----------------------------------------------------------------------------
 */

/* ud_init(ud_t* ud, origin, dis_mode, input_type, start, end)
 *
 * ud_t object initializer.
 */
extern void ud_init(ud_t *ud);

/* ud_set_input(ud_t, type, input, end)
 * ud_set_input_hook(ud_t* ud, int (*hook)());
 * ud_set_input_buffered(ud_t* ud, char* buffer, size_t size);
 * ud_set_input_file(ud_t* ud, FILE* filep);
 *
 * Sets the input source for the  disassembler. Possible types are -
 * INPUT_HOOK, INPUT_BUFFERED, INPUT_FILE.
 * The last three function are wrappers for the first one.
 * 
 */
extern void ud_set_input(ud_t* ud, ud_input_t input_type, 
				void *input, void *end);
extern void ud_set_input_hook(ud_t* ud, int (*hook)());
extern void ud_set_input_buffered(ud_t* ud, char* buffer, size_t size);
extern void ud_set_input_file(ud_t* ud, FILE* filep);

/* ud_set_dis_mode(ud, mode) 
 *
 * Sets the disassembly mode. Possible values are -
 * MODE16, MODE32, MODE64.
 */
extern void ud_set_dis_mode(ud_t* ud, ud_mode_t mode);

/* ud_set_origin(ud, origin) - 
 *
 * Sets code origin. Can be any positive integer (32 bits).
 */
extern void ud_set_origin(ud_t* ud, unsigned int origin);

/* ud_disassemble(ud_t* ud)
 *
 * Disassembles one instruction and returns the number of bytes disassembled.
 * The object must be initialized to a source. This wraps ud_decode and
 * other translator functions.
 */
extern unsigned int ud_disassemble(ud_t* ud, ud_syntax_t s);

/* ud_asmout_offset(ud)
 * ud_asmout_hex(ud)
 * ud_asmout_insn(ud)
 * 
 * Ouput access functions. As the names suggest, they return the offset,
 * the instruction, and the hex form of the codes respectively.
 */
extern unsigned int ud_asmout_offset(ud_t* ud);
extern const char* ud_asmout_insn(ud_t* ud);
extern const char* ud_asmout_hex(ud_t* ud);

/*-----------------------------------------------------------------------------
 * Level-2 Wrappers
 *-----------------------------------------------------------------------------
 */

/* ud_src_init() - 
 *
 * Initializes the source buffer pointers. fill_ptr and curr_ptr 
 * are both set to -1, indicating an  uninitiated state. This function must be 
 * called before the disassembly session.
 */
extern void ud_src_init(register ud_t *ud);


/* ud_src_start(ud) - 
 *
 * Initializes the source buffering unit for a single instruction disassembly. 
 * This must be called every time before calling the decoding functions.
 */
extern void ud_src_start(ud_t* ud);

/* ud_decode(ud_t* ud)
 *
 * This wraps ud_search_opcmap, by searching the map first and then proceeds
 * to decode the prefixes and the instruction operands. It generates an
 * intermediate form (see ud-types.h) which can be translated to different
 * syntax forms.
 */
extern void ud_decode(ud_t* ud);

/* ud_translate_intel(ud_t* ud)
 * ud_translate_att(ud_t* ud)
 *
 * Translates decoded instruction to the intel/att syntax. Once ud_decode 
 * generates the intermediate form, the object can be translated to generate an 
 * assembly output (ud_t.asmout).
 */
extern void ud_translate_intel(ud_t *ud);
extern void ud_translate_att(ud_t *ud);

/* ud_src_nomore(ud)
 *
 * Returns a non zero if there are no more bytes to be disassembled.
 */
extern int ud_src_nomore(ud_t* ud);

/*-----------------------------------------------------------------------------
 * Level-3 Wrappers
 *-----------------------------------------------------------------------------
 */

/* ud_clear_insn(ud_t* ud)
 *
 * Clears the attributes/flags and data fields of ud associated with decoding
 * a map entry. This function is called internally by the decoder before
 * decoding.
 */
extern void ud_clear_insn(ud_t *ud);

/* ud_extract_prefixes(ud_t* ud)
 *
 * Reads the source for possible prefixes and sets flags associated with "ud".
 * This function is called internally by the decoder before decoding.
 */
extern int ud_extract_prefixes(register ud_t* ud);

/* ud_search_opcmap(ud_t* ud)
 *
 * Finds the entry in the opcode map which corresponds to the bytes in source.
 * The functions pulls 1 or 2 bytes from source to search the map and fills
 * ud->opcmap_entry.
 */
extern void ud_search_opcmap(ud_t *ud);

/* ud_get_3dnow_insn_mnemonic(uint8_T suffix)
 *
 * 3dnow! instructions mnemonics can be resolved only based on the 
 * instruction suffix. This function returns the mneomnic code based on 
 * that.
 * 
 */
extern ud_mnemonic_code_t ud_get_3dnow_insn_mnemonic(uint8_t suffix);

/* ud_lookup_mnemonic()
 *
 * lookup the string form of a mnemonic code.
 */
extern const char* ud_lookup_mnemonic(ud_mnemonic_code_t c);

#ifdef __cplusplus
}
#endif
#endif
