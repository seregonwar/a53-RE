#include <stddef.h>

#include "a53_abi.h"

void A53_SECTION(".text.el3.loader") aarch64_DC_CVAC_range(void *base, a53_u64 vsize)
{
    while (vsize != 0) {
        __asm__ volatile("dc cvac, %0" : : "r"(base) : "memory");
        base = (void *)((a53_u64)base + 64);
        vsize -= 64;
    }
}
