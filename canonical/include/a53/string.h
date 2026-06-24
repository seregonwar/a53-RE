#ifndef A53_STRING_H
#define A53_STRING_H

#include "a53_abi.h"

/* =========================================================================
 * Standard string/memory functions (A53 bare-metal implementations)
 * ========================================================================= */

void *memcpy(void *dest, const void *src, a53_u64 n);
void bzero(void *s, a53_u64 n);
char *strncpy(char *dest, const char *src, a53_u64 n);
a53_u64 strnlen(const char *s, a53_u64 maxlen);
int puts(char *s);
int toupper(int c);
int tolower(int c);

#endif
