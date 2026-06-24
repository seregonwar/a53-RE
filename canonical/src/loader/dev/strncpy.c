#include <stddef.h>

#include "a53_abi.h"

char *A53_SECTION(".text.dev.loader") strncpy(char *dest, const char *src, size_t n)
{
    size_t index;

    for (index = 0; index < n && src[index] != '\0'; ++index) {
        dest[index] = src[index];
    }
    for (; index < n; ++index) {
        dest[index] = '\0';
    }
    return dest;
}
