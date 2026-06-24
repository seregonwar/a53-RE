#include <stddef.h>

#include "a53_abi.h"

size_t A53_SECTION(".text.dev.loader") strnlen(char *string, size_t maxlen)
{
    size_t length = 0;

    while (length < maxlen && string[length] != '\0') {
        ++length;
    }
    return length;
}
