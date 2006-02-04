/* syn.h
 *
 * Copyright (c) 2002, 2003, 2004 Vivek Mohan <vivek@sig9.com>
 * All rights reserved.
 * See (LICENSE)
 */

#ifndef _SYN_H_
#define _SYN_H_

#include <udis86.h>

extern void mkasm(ud_t* ud, const char* fmt, ...);
extern void ud_syntax_map_script(ud_t*, ud_syntax_plugin_t*, int*, 
					unsigned int n, char*);

#endif
