#include "a53_abi.h"
#include "a53_context.h"

extern deci_sig_mp4_t g_deci_sig_mp4_data;
extern deci_sig_mp4_t *g_deci_sig_mp4;

void A53_SECTION(".text.el3.loader")
deci_mp4_sig1_write_int_to_sycorax(a53_u32 bit, a53_u32 *dst, a53_u32 val)
{
    deci_sig_mp4_t *dsim;

    dsim = g_deci_sig_mp4;
    if (*(volatile a53_u32 *)((a53_u64)dsim->dsim_sig2 + 0x14) != 0) {
        a53_u32 cpu;

        cpu = mp4_get_cpu();
        printf_low("%d:%s:SIG2 INT_FROM_CPUY has data 0x%08x\n",
                   (a53_u64)cpu,
                   "deci_mp4_sig1_write_int_to_sycorax",
                   (a53_u64)*(volatile a53_u32 *)((a53_u64)dsim->dsim_sig2 + 0x14));
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_sig_mp4.c",
                   "deci_mp4_sig1_write_int_to_sycorax", 0x29e, 0, "0");
    }
    *dst = val;
    *(volatile a53_u32 *)((a53_u64)dsim->dsim_sig1 + 4) = bit;
    __asm__ volatile("dsb sy");
}

a53_u32 A53_SECTION(".text.el3.loader") deci_mp4_sig2_read_int_from_emc(void)
{
    return *(volatile a53_u32 *)((a53_u64)g_deci_sig_mp4->dsim_sig2 + 0x14);
}

a53_u32 A53_SECTION(".text.el3.loader") deci_mp4_sig3_read_int_from_emc(void)
{
    return *(volatile a53_u32 *)((a53_u64)g_deci_sig_mp4->dsim_sig3 + 0x14);
}

void A53_SECTION(".text.el3.loader")
deci_mp4_sig3_clear_int_from_emc(a53_u32 v)
{
    *(volatile a53_u32 *)((a53_u64)g_deci_sig_mp4->dsim_sig3 + 0x14) = v;
    __asm__ volatile("dsb sy");
}

int A53_SECTION(".text.el3.loader") deci_sig_mp4_start(void)
{
    g_deci_sig_mp4_data.dsim_base = 0xc1600000ULL;
    g_deci_sig_mp4 = &g_deci_sig_mp4_data;
    g_deci_sig_mp4_data.dsim_msi = 0xc17c8400ULL;
    g_deci_sig_mp4_data.dsim_sig1 = 0xc1683800ULL;
    g_deci_sig_mp4_data.dsim_sig2 = 0xc1784000ULL;
    g_deci_sig_mp4_data.dsim_sig3 = 0xc1784800ULL;
    return 0;
}
