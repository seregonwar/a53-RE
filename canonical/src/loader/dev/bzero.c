#include <stddef.h>

#include "a53_abi.h"

void A53_SECTION(".text.dev.loader") bzero(void *s, size_t n)
{
    unsigned char *p = s;

    while (n != 0) {
        --n;
        *p = 0;
    }
}
