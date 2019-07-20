#ifndef STDIO_STRING_H
#define STDIO_STRING_H

#include <sys/types.h>

char* itoa(int num, char* str, int base);
size_t strlength( const char *s );

#endif
