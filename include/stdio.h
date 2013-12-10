#ifndef _STDIO_H
#define _STDIO_H

#include <defs.h>


int printf(const char *format, ...);

void scanf(char* str, void* buf);

int printf_color(const char *fmt, ...);
#endif
