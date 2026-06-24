#include "a53_context.h"

int A53_SECTION(".text.el3.loader") dev_context_init_for_el3(dev_context_t *context)
{
    register a53_u64 register_context __asm__("x8") = (a53_u64)(uintptr_t)context;
    register int result __asm__("w0") = 0;

    __asm__ volatile(
        "msr tpidr_el3, %x0\n\t"
        "msr tpidrro_el0, %x0"
        : "+r"(register_context), "+r"(result)
        :
        : "memory");
    return result;
}
