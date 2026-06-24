#include "a53_context.h"

dev_context_t *A53_SECTION(".text.dev.loader") get_dev_context(void)
{
    a53_u64 value;

    __asm__ volatile("mrs %0, tpidrro_el0" : "=r"(value));
    return (dev_context_t *)(uintptr_t)value;
}
