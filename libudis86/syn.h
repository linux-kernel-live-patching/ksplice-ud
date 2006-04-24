#ifndef UD_SYN_H
#define UD_SYN_H

#include <stdio.h>
#include <stdarg.h>

extern const char* ud_reg_tab[];

static void mkasm(struct ud* u, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  u->asm_fill += vsprintf((char*) u->asm_buffer + u->asm_fill, fmt, ap);

  va_end(ap);
}

#endif
