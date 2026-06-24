#include "a53_abi.h"
#include "a53_context.h"

int A53_SECTION(".text.el3.loader") putchar_pericom(int c)
{
    a53_u8 *base;

    base = (a53_u8 *)(mp4_get_cpu() ? 0xbd110200UL : 0xbd110000UL);
    pericom_putchar(base, c);
    return c;
}
