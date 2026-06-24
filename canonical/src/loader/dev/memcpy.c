#include <stddef.h>

#include "a53_abi.h"

void *A53_SECTION(".text.dev.loader") memcpy(void *dest, void *src, size_t n)
{
    unsigned char *out = dest;
    const unsigned char *in = src;
    size_t index;

    for (index = 0; index != n; ++index) {
        out[index] = in[index];
    }
    return dest;
}
