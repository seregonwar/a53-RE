#include "a53_abi.h"
#include "a53_context.h"

extern void aarch64_BRK(int code);

void A53_SECTION(".text.el3.loader") el3_assert(char *file, char *func,
                                                a53_u32 line, int c, char *cstr)
{
    if (c != 0) {
        return;
    }
    printf_low("Assertion failed at <%s:%s:%d:%s> on %d\n",
               file, func, (a53_u64)line, cstr, (a53_u64)mp4_get_cpu());
    aarch64_BRK(0);
    for (;;) {
    }
}
