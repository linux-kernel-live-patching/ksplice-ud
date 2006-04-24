/* udcli.c
 *
 * Copyright (c) 2002, 2003, 2004, 2005, 2006 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <udis86.h>

/* help string */
static char help[] = {
 "usage: %s [-option[s]]\n"
 "This udis86 front-end reads bytes from STDIN.\n"
 "options:\n"
 "  -16         : 16-bit code.\n"
 "  -32         : 32-bit code.\n"
 "  -64         : 64-bit code.\n"
 "  -o <origin> : code origin in decimal system.\n"
 "  -intel      : Intel (NASM like) syntax.\n"
 "  -att        : AT&T (GNU as) syntax.\n"
 "\n"
 "Udis86 Disassembler for x86 and x86-64 (AMD64)\n" 
 "http://udis86.sourceforge.net/\n"
 "Author: Vivek Mohan, (http://sig9.com/~vivek/)\n"
};

	

/* main */
int main(int argc, char **argv)
{
	char *prog_path = *argv;
	
	ud_type_t syntax = UD_SYN_INTEL;

	/* udis86 object */
	ud_t ud_obj;

	/* initialize */
	ud_init(&ud_obj);

	argv++;

	/* loop through the args */
	while(--argc > 0) {
		if (strcmp(*argv,"-16") == 0)
			ud_set_dis_mode(&ud_obj, UD_MODE16);
		else if (strcmp(*argv,"-32") == 0)
			ud_set_dis_mode(&ud_obj, UD_MODE32);
		else if (strcmp(*argv,"-64") == 0)
			ud_set_dis_mode(&ud_obj, UD_MODE64);
		else if (strcmp(*argv,"-intel") == 0)
			syntax = UD_SYN_INTEL;
		else if (strcmp(*argv,"-att") == 0)
			syntax = UD_SYN_ATT;
		else if (strcmp(*argv,"-o") == 0) {
			if (--argc)
				ud_set_origin(&ud_obj, atoi(*(++argv)));
			else 	printf("-o <origin>\n");

		} else {
			printf(help, prog_path);
			return 0;
		} 
		argv++;
	}

	/* set input to STDIN */
	ud_set_input_file(&ud_obj, stdin);

	/* disassembly loop */
	while (ud_disassemble(&ud_obj, syntax)) {

		/* get hex code */
		char* hex = ud_asm_hex(&ud_obj);

		/* if the hex code is too long, split it */
		if (strlen(hex) > 16) {
			char part_1[25];
			char part_2[25];
			strncpy(part_1, hex,  16);
			strcpy (part_2, hex + 16);

			/* print part 1 */
			printf("%08x %-16s %-24s\n", 
				ud_asm_offset(&ud_obj),
				part_1,
				ud_asm(&ud_obj)	);
			/* print part 2 */
			printf("%8s %-16s\n", "", part_2);
		} else {
			printf("%08x %-16s %-24s\n", 
				ud_asm_offset(&ud_obj),
				hex,
				ud_asm(&ud_obj));
		}
	}

	printf("\n");
	return 0;
}
