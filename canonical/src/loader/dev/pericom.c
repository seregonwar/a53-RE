#include "a53_abi.h"

void A53_SECTION(".text.dev.loader") pericom_putchar(a53_u8 *base, int c)
{
    volatile a53_u8 *reg = (volatile a53_u8 *)base;

    /* Wait for TX ready (bit 5 of reg[5]) with two-stage timeout. */
    {
        a53_u32 timeout = 0x10000000;
        do {
            if ((reg[5] >> 5 & 1) != 0) goto write;
            ++timeout;
        } while (timeout != 0);
        timeout = 250000;
        do {
            if ((reg[5] >> 5 & 1) != 0) goto write;
            --timeout;
        } while (timeout != 0);
    }

write:
    reg[0] = (a53_u8)c;

    /* Wait for TX done (bit 6) then TX ready (bit 5). */
    {
        a53_u32 timeout = 250000;
        do {
            if ((reg[5] >> 6 & 1) != 0) return;
            --timeout;
        } while (timeout != 0);
        timeout = 250000;
        do {
            if ((reg[5] >> 5 & 1) != 0) return;
            --timeout;
        } while (timeout != 0);
    }
}
