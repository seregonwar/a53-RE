#include "a53_context.h"

extern void pericom_putchar(a53_u8 *base, int c);

int A53_SECTION(".text.dev.loader") putchar(int c)
{
    dev_context_t *dc;

    dc = get_dev_context();
    (*dc->dc_putchar_low_hook)(c);
    return c;
}

sttyp_putchar_context_t *A53_SECTION(".text.dev.loader") sttyp_putchar_context_get(void)
{
    dev_context_t *dc;

    dc = get_dev_context();
    return dc->dc_sttyp_putchar_context;
}

void A53_SECTION(".text.dev.loader") set_sttyp_putchar_context(sttyp_putchar_context_t *spc)
{
    dev_context_t *dc;

    dc = get_dev_context();
    dc->dc_sttyp_putchar_context = spc;
}

int A53_SECTION(".text.dev.loader") putchar_sttyp_end(void)
{
    return 0;
}

int A53_SECTION(".text.dev.loader") putchar_el0_direct(int c)
{
    dev_context_t *dc;
    mp4_debug_status_t *ds;
    a53_u8 *base;

    dc = get_dev_context();
    base = dc->dc_sttyp_putchar_context->spc_pericom;
    if (base != (a53_u8 *)0) {
        pericom_putchar(base, c);
        return (int)(a53_u64)base;
    }
    ds = dc->dc_sttyp_putchar_context->spc_mds_el0;
    {
        a53_u8 *buf;

        buf = (a53_u8 *)((a53_u64)ds + ds->mds_ttyp_buffer_offset);
        buf[ds->mds_ttyp_buffer_last] = (a53_u8)c;
        ++ds->mds_ttyp_buffer_last;
        if (ds->mds_ttyp_buffer_last == ds->mds_ttyp_buffer_size) {
            ds->mds_ttyp_buffer_last = 0;
            ++ds->mds_ttyp_buffer_count;
        }
    }
    return 0;
}

int A53_SECTION(".text.dev.loader") putchar_sttyp(int c)
{
    get_dev_context();
    putchar_el0_direct(c);
    return c;
}

int A53_SECTION(".text.dev.loader") putchar_sttyp_begin(void)
{
    sttyp_putchar_context_t *spc;
    a53_u64 timer;
    int i;

    spc = sttyp_putchar_context_get();
    putchar_el0_direct('E');
    putchar_el0_direct('L');
    putchar_el0_direct('0');
    putchar_el0_direct(':');
    putchar_el0_direct(spc->spc_cpu + '0');
    putchar_el0_direct(':');
    putchar_el0_direct(':');

    __asm__("mrs %0, cntpct_el0" : "=r"(timer));
    for (i = 0; i < 16; ++i) {
        a53_u64 nibble;

        nibble = (timer >> ((15 - i) * 4)) & 0xfUL;
        if (nibble < 10) {
            putchar_el0_direct((int)(nibble | 0x30));
        } else {
            putchar_el0_direct((int)(nibble + 0x57));
        }
    }
    putchar_el0_direct(':');
    putchar_el0_direct(' ');
    return 0;
}

int A53_SECTION(".text.dev.loader") putchar_titania_uart_el0(int c)
{
    while ((*(volatile a53_u32 *)0x0382010cUL >> 11) & 1) {
        /* wait for TX FIFO not full */
    }
    *(volatile a53_u32 *)0x03820104UL = (a53_u32)(c & 0xff);
    return c;
}
