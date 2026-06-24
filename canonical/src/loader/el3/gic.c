#include "a53_abi.h"
#include "a53_context.h"

extern a53_u8 g_cbar_el1;

static a53_u64 gic_base(void)
{
    return (a53_u64)(g_cbar_el1 ? 0x32a0000UL : 0);
}

static a53_u32 A53_SECTION(".text.el3.loader")
uint32_set_field(a53_u32 src, a53_u32 mask, a53_u32 value)
{
    return (value & mask) | (src & ~mask);
}

int A53_SECTION(".text.el3.loader")
gic_print_reg(char *name, a53_u32 v)
{
    return printf_low("%d:%s:%-16s  0x%08x\n", (a53_u64)mp4_get_cpu(),
                      "gic_print_reg", name, (a53_u64)v);
}

int A53_SECTION(".text.el3.loader")
gic_print_gicd_reg(char *name, a53_u32 offset)
{
    return gic_print_reg(name,
                         *(volatile a53_u32 *)(gic_base() + offset + 0x1000));
}

int A53_SECTION(".text.el3.loader")
gic_print_gicc_reg(char *name, a53_u32 offset)
{
    return gic_print_reg(name,
                         *(volatile a53_u32 *)(gic_base() + offset + 0x2000));
}

a53_u32 A53_SECTION(".text.el3.loader") gic_read_GICC_IAR(void)
{
    return *(volatile a53_u32 *)(gic_base() + 0x200c);
}

void A53_SECTION(".text.el3.loader") gic_write_GICC_EOIR(a53_u32 v)
{
    *(volatile a53_u32 *)(gic_base() + 0x2010) = v;
}

a53_u32 A53_SECTION(".text.el3.loader") gic_read_GICC_RPR(void)
{
    gic_print_gicc_reg("GICC_RPR", 0x14);
    return *(volatile a53_u32 *)(gic_base() + 0x2014);
}

a53_u32 A53_SECTION(".text.el3.loader") gic_read_GICC_HPPIR(void)
{
    return *(volatile a53_u32 *)(gic_base() + 0x2018);
}

a53_u32 A53_SECTION(".text.el3.loader")
gic_status_check(gic_status *gs)
{
    a53_u64 base;
    a53_u32 i;
    a53_u32 pgVar5;
    int count;

    base = gic_base();
    pgVar5 = 0;
    count = gs->gs_count;
    if (count == 0) {
        for (i = 0; i < 4; ++i) {
            gs->gsd_igroupr[i] = *(volatile a53_u32 *)(base + 0x1080 + i * 4);
        }
        for (i = 0; i < 8; ++i) {
            gs->gsd_isenabler[i] = *(volatile a53_u32 *)(base + 0x1100 + i * 4);
        }
        for (i = 0; i < 8; ++i) {
            gs->gsd_ispendr[i] = *(volatile a53_u32 *)(base + 0x1200 + i * 4);
        }
        for (i = 0; i < 8; ++i) {
            gs->gsd_isactiver[i] = *(volatile a53_u32 *)(base + 0x1300 + i * 4);
        }
    } else {
        for (i = 0; i < 4; ++i) {
            a53_u32 v;

            v = *(volatile a53_u32 *)(base + 0x1080 + i * 4);
            if (gs->gsd_igroupr[i] != v) {
                pgVar5 = (a53_u32)printf_low("%d:%s:GICD_IGROUPR[%d]   : 0x%08x => 0x%08x\n",
                    (a53_u64)mp4_get_cpu(), "gic_status_check",
                    (a53_u64)i, (a53_u64)gs->gsd_igroupr[i], (a53_u64)v);
                gs->gsd_igroupr[i] = v;
            }
        }
        for (i = 0; i < 8; ++i) {
            a53_u32 v;

            v = *(volatile a53_u32 *)(base + 0x1100 + i * 4);
            if (gs->gsd_isenabler[i] != v) {
                pgVar5 = (a53_u32)printf_low("%d:%s:GICD_ISENABLER[%d] : 0x%08x => 0x%08x\n",
                    (a53_u64)mp4_get_cpu(), "gic_status_check",
                    (a53_u64)i, (a53_u64)gs->gsd_isenabler[i], (a53_u64)v);
                gs->gsd_isenabler[i] = v;
            }
        }
        for (i = 0; i < 8; ++i) {
            a53_u32 v;

            v = *(volatile a53_u32 *)(base + 0x1200 + i * 4);
            if (gs->gsd_ispendr[i] != v) {
                printf_low("%d:%s:GICD_ISPENDR[%d]   : 0x%08x => 0x%08x\n",
                    (a53_u64)mp4_get_cpu(), "gic_status_check",
                    (a53_u64)i, (a53_u64)gs->gsd_ispendr[i], (a53_u64)v);
                gs->gsd_ispendr[i] = v;
                for (;;) { }
            }
        }
        for (i = 0; i < 8; ++i) {
            a53_u32 v;

            v = *(volatile a53_u32 *)(base + 0x1300 + i * 4);
            if (gs->gsd_isactiver[i] != v) {
                pgVar5 = (a53_u32)printf_low("%d:%s:GICD_ISACTIVER[%d] : 0x%08x => 0x%08x\n",
                    (a53_u64)mp4_get_cpu(), "gic_status_check",
                    (a53_u64)i, (a53_u64)gs->gsd_isactiver[i], (a53_u64)v);
                gs->gsd_isactiver[i] = v;
            }
        }
    }
    ++gs->gs_count;
    return pgVar5;
}

int A53_SECTION(".text.el3.loader") gic_check(void)
{
    extern gic_status g_gic_status_core0;

    gic_status_check(&g_gic_status_core0);
    return 0;
}

a53_u32 A53_SECTION(".text.el3.loader")
gic_enable_irq(a53_u32 irq, a53_u32 cpu)
{
    a53_u64 base;

    base = gic_base();
    *(volatile a53_u32 *)(base + 0x1100 + ((irq >> 3) & ~3)) |=
        1U << (irq & 0x1f);
    *(volatile a53_u32 *)(base + 0x1800 + (irq & ~3)) |=
        1U << (((irq & 3) * 8 + cpu) & 0x1f);
    return irq;
}

int A53_SECTION(".text.el3.loader") gic_init_by_1st_core(void)
{
    extern void aarch64_read_CBAR_EL1(void);
    extern gic_status g_gic_status_core0;
    extern gic_status g_gic_status_core1;
    a53_u64 base;
    a53_u32 i;

    aarch64_read_CBAR_EL1();
    g_cbar_el1 = 1;
    g_gic_status_core0.gs_count = 0;
    g_gic_status_core1.gs_count = 0;
    gic_check();
    gic_enable_irq(0x4e, 0);
    gic_enable_irq(0x53, 1);

    base = gic_base();
    *(volatile a53_u32 *)(base + 0x1080) &= ~1U;

    for (i = 0; i < 0x28; i += 4) {
        *(volatile a53_u32 *)(base + 0x1c00 + i) = 0xaaaaaaaa;
    }

    *(volatile a53_u32 *)(base + 0x2004) = 0xff;
    *(volatile a53_u32 *)(base + 0x1000) = 1;
    *(volatile a53_u32 *)(base + 0x2000) = 1;
    gic_check();
    return 0;
}

int A53_SECTION(".text.el3.loader") gic_init_by_2nd_core(void)
{
    a53_u64 base;

    base = gic_base();
    *(volatile a53_u32 *)(base + 0x2004) = 0xff;
    printf_low("%d:%s:GICC_CTLR = 0x%08x\n", (a53_u64)mp4_get_cpu(),
               "gic_init_by_2nd_core",
               (a53_u64)*(volatile a53_u32 *)(base + 0x2000));
    *(volatile a53_u32 *)(base + 0x2000) = 1;
    return 0;
}

void A53_SECTION(".text.el3.loader") gic_sgi1(void)
{
    a53_u64 base;
    a53_u32 i;

    base = gic_base();
    printf_low("%d:%s:()\n", (a53_u64)mp4_get_cpu(), "gic_sgi1");
    printf_low("%d:%s:DAIF: 0x%016lx\n", (a53_u64)mp4_get_cpu(), "gic_sgi1",
               (a53_u64)aarch64_read_DAIF());
    gic_print_gicd_reg("GICD_CTLR", 0);
    gic_print_gicd_reg("GICD_TYPER", 4);
    gic_print_gicd_reg("GICD_IIDR", 8);

    for (i = 0; i < 8; ++i) {
        printf_low("%d:%s:GICD_IGROUPR[%d] = 0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "gic_print_gicd",
                   (a53_u64)i,
                   (a53_u64)*(volatile a53_u32 *)(base + 0x1080 + i * 4));
    }
    for (i = 0; i < 8; ++i) {
        printf_low("%d:%s:GICD_ISENABLER[%d] = 0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "gic_print_gicd",
                   (a53_u64)i,
                   (a53_u64)*(volatile a53_u32 *)(base + 0x1100 + i * 4));
    }
    for (i = 0; i < 8; ++i) {
        printf_low("%d:%s:GICD_ISPENDR[%d]   = 0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "gic_print_gicd",
                   (a53_u64)i,
                   (a53_u64)*(volatile a53_u32 *)(base + 0x1200 + i * 4));
    }
    gic_print_gicd_reg("GICD_ISACTIVER0", 0x300);
    gic_print_gicd_reg("GICD_ITARGETSR0", 0x800);
    for (i = 0; i < 8; ++i) {
        printf_low("%d:%s:GICD_IPRIORITYR[%d]   = 0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "gic_print_gicd",
                   (a53_u64)i,
                   (a53_u64)*(volatile a53_u32 *)(base + 0x1400 + i * 4));
    }
    gic_print_gicc_reg("GICC_CTLR", 0);
    gic_print_gicc_reg("GICC_IAR", 0xc);
    gic_print_gicc_reg("GICC_IIDR", 0xfc);

    {
        a53_u32 sgi;

        sgi = (*(volatile a53_u32 *)(base + 0x1f00) & 0xfc00fff0) | 0x30001;
        printf_low("%d:%s:GICD_SGIR <= 0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "gic_sgi01",
                   (a53_u64)sgi);
        *(volatile a53_u32 *)(base + 0x1f00) = sgi;
    }
}

void A53_SECTION(".text.el3.loader") gic_core1(void)
{
    extern gic_status g_gic_status_core1;

    printf_low("%d:%s:DAIF: 0x%016lx\n", (a53_u64)mp4_get_cpu(), "gic_core1",
               (a53_u64)aarch64_read_DAIF());
    printf_low("%d:%s:Enter LOOP\n", (a53_u64)mp4_get_cpu(), "gic_core1");
    for (;;) {
        gic_status_check(&g_gic_status_core1);
    }
}
