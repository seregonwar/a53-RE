#include "a53_context.h"

int A53_SECTION(".text.dev.loader") puts(char *s)
{
    __asm__ volatile("svc #0x111");
    return (int)(a53_u64)s;
}
