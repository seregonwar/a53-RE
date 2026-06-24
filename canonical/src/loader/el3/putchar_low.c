#include "a53_abi.h"
#include "a53_context.h"

int A53_SECTION(".text.el3.loader") putchar_low(int c)
{
    dev_context_t *dc;

    dc = get_dev_context();
    if (c == '\n') {
        (*dc->dc_putchar_low_hook)('\r');
    }
    (*dc->dc_putchar_low_hook)(c);
    return c;
}

int A53_SECTION(".text.el3.loader") putchar_cp(int c)
{
    return mp4_debug_status_putchar(c);
}
