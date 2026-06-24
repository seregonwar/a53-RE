#include <stddef.h>

#include "a53_abi.h"

a53_u32 A53_SECTION(".text.dev.loader") mp4_get_cpu(void)
{
    a53_u64 mpidr;

    __asm__ volatile("mrs %0, mpidr_el1" : "=r"(mpidr));
    return (a53_u32)(mpidr & 0xff);
}

char *A53_SECTION(".text.dev.loader") mp4_basename(char *f)
{
    char *p;
    char *last;

    last = NULL;
    for (p = f; *p != '\0'; ++p) {
        if ((*p == '/' || *p == '\\') && *(p + 1) != '\0') {
            last = p + 1;
        }
    }
    return (last != NULL) ? last : f;
}
