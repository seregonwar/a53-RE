/*
 * A53-RE canonical source.
 * Original path: I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\reset\\sram.c
 *
 * SRAM boot stage: UART putchar/puts/printf, syshub IOMMU init,
 * boot sequence, and SError trap handler.
 *
 * NOTE: prnt_el3_sram is an identical copy of canonical prnt() — not duplicated here.
 *       printf_el3_sram uses canonical prnt() via putchar_el3_sram_hook.
 * NOTE: strnlen_el3_sram and sram_sceMp4LookCtypeTable are identical to canonical
 *       strnlen() and the shared ctype table — not duplicated here.
 */

#include <stdarg.h>
#include <stddef.h>

#include "a53_context.h"

/* ---- SRAM UART base address ---- */
/* Determined from putchar_el3_sram_core @ 0x06067798:
 *   movz x8, #0x1000
 *   movk x8, #0x0302, LSL #16
 *   → 0x03021000
 */
#define SRAM_UART_BASE  ((volatile void *)0x03021000ULL)

/* ---- UART MMIO helpers (16550-compatible) ---- */

static a53_u32 A53_SECTION(".text.reset.loader")
uart_get_stat(volatile void *base)
{
    return *(volatile a53_u32 *)((a53_u8 *)base + 0xc);
}

static void A53_SECTION(".text.reset.loader")
uart_set_data(volatile void *base, a53_u32 data)
{
    *(volatile a53_u32 *)((a53_u8 *)base + 0x4) = data & 0xff;
}

/* ---- SRAM putchar / puts ---- */

int A53_SECTION(".text.reset.loader")
putchar_el3_sram_core(volatile void *uart_base, int c)
{
    /* Poll until TX FIFO not full (bit 11 of status register) */
    while ((uart_get_stat(uart_base) >> 11) & 1) {
        /* wait */
    }
    uart_set_data(uart_base, c);
    return c;
}

int A53_SECTION(".text.reset.loader")
putchar_el3_sram(int c)
{
    volatile void *uart = SRAM_UART_BASE;

    if (c == '\n') {
        putchar_el3_sram_core(uart, '\r');
    }
    putchar_el3_sram_core(uart, c);
    return c;
}

/* ---- putchar hook for prnt() ---- */

int A53_SECTION(".text.reset.loader")
putchar_el3_sram_hook(void *pfd, int ch)
{
    (void)pfd;
    /* prnt() protocol: 0x200 = begin, 0x201 = end — ignored */
    if (ch != 0x200 && ch != 0x201) {
        putchar_el3_sram(ch);
    }
    return 0;
}

/* ---- puts ---- */

int A53_SECTION(".text.reset.loader")
puts_el3_sram(char *s)
{
    int n = 0;

    while (*s != '\0') {
        if (*s == '\n') {
            putchar_el3_sram_core(SRAM_UART_BASE, '\r');
        }
        putchar_el3_sram_core(SRAM_UART_BASE, (unsigned char)*s);
        ++s;
        ++n;
    }
    return n;
}

/* ---- printf (uses canonical prnt) ---- */

int A53_SECTION(".text.reset.loader")
printf_el3_sram(char *format, ...)
{
    va_list ap;
    int ret;

    va_start(ap, format);
    ret = prnt(putchar_el3_sram_hook, (void *)0, format, &ap);
    va_end(ap);
    return ret;
}

/* ---- debug_status helpers (write into log buffer) ---- */

void A53_SECTION(".text.reset.loader")
debug_status_step(a53_u64 *log, a53_u64 v)
{
    *log = v;
}

void A53_SECTION(".text.reset.loader")
debug_status_set_x3(a53_u64 *log, a53_u64 v)
{
    *(volatile a53_u64 *)((a53_u8 *)log + 0x28) = v;
}

void A53_SECTION(".text.reset.loader")
debug_status_set_x4(a53_u64 *log, a53_u64 v)
{
    *(volatile a53_u64 *)((a53_u8 *)log + 0x30) = v;
}

void A53_SECTION(".text.reset.loader")
debug_status_set_x5(a53_u64 *log, a53_u64 v)
{
    *(volatile a53_u64 *)((a53_u8 *)log + 0x38) = v;
}

/* ---- syshub IOMMU initialization (SRAM boot) ---- */

static a53_u32 A53_SECTION(".text.reset.loader")
seg_size_to_tlb0_encoding(a53_u32 seg_size)
{
    if      (seg_size == 0x20000)   return 0;
    else if (seg_size == 0x40000)   return 2;
    else if (seg_size == 0x80000)   return 4;
    else if (seg_size == 0x100000)  return 6;
    else if (seg_size == 0x200000)  return 8;
    else if (seg_size == 0x400000)  return 10;
    else if (seg_size == 0x800000)  return 12;
    else if (seg_size == 0x1000000) return 14;
    else if (seg_size == 0x2000000) return 16;
    else if (seg_size == 0x4000000) return 18;
    else {
        printf_el3_sram("Unsupport seg_size 0x%08x\n", seg_size);
        return 18; /* default */
    }
}

void A53_SECTION(".text.reset.loader")
syshub_init_core(int tlb, a53_u64 phy_addr, a53_u32 seg_size)
{
    a53_u32 tlb0;
    a53_u32 base;
    a53_u32 offset;
    volatile a53_u32 *tlb_base;

    /* Build TLB0: size encoding | (offset bits << 5) */
    tlb0 = seg_size_to_tlb0_encoding(seg_size);

    base = (a53_u32)(phy_addr & 0xfc000000ULL);
    offset = (a53_u32)(phy_addr - base);
    if (offset != 0) {
        tlb0 |= (offset >> 17) << 5;
        printf_el3_sram("tlb=%d, 0x%08x, offset=0x%08x, tlb1=0x%08x\n",
                         (unsigned)tlb, base, offset, tlb0);
    }

    /* TLB registers 0-3: four 32-bit words at 0x03230000 + tlb * 0x10.
     * Verified from disassembly at 0x0606934c-0x06069394:
     *   mov w8, #base_offset; movk w8, #0x323, lsl #16
     *   add w0, w8, w9, lsl #4   → base + tlb * 0x10
     */
    tlb_base = (volatile a53_u32 *)(0x03230000ULL + (tlb * 0x10));

    tlb_base[0] = (a53_u32)(phy_addr >> 26);           /* TLB0: high address bits */
    tlb_base[1] = tlb0;                                 /* TLB1: size + offset */
    tlb_base[2] = 4;                                    /* TLB2: attribute */
    tlb_base[3] = 4;                                    /* TLB3: sub-attribute */

    /* Mask register: at 0x032303e0 + tlb * 4 (stride 4, NOT 0x10!).
     * Verified from disassembly at 0x0606939c-0x060693ac:
     *   mov w8, #0x3e0; movk w8, #0x323, lsl #16
     *   add w0, w8, w9, lsl #2   → 0x032303e0 + tlb * 4
     */
    *(volatile a53_u32 *)(0x032303e0ULL + (tlb * 4)) = 0xffffffff;

    /* Control register: at 0x032304d8 + tlb * 4 (stride 4).
     * Verified from disassembly at 0x060693b4-0x060693c4:
     *   mov w8, #0x4d8; movk w8, #0x323, lsl #16
     *   add w0, w8, w9, lsl #2   → 0x032304d8 + tlb * 4
     * Conditional: skipped when tlb == 0x3e (cmp+b.eq at 0x0606940c).
     */
    if (tlb != 0x3e) {
        *(volatile a53_u32 *)(0x032304d8ULL + (tlb * 4)) = 0xc1800003;
    }
}

static void A53_SECTION(".text.reset.loader")
sram_syshub_init(int tlb, a53_u64 phy_addr)
{
    syshub_init_core(tlb, phy_addr, 0x4000000);
}

int A53_SECTION(".text.reset.loader")
syshub_init_all(a53_u64 *log)
{
    a53_u32 i;

    /* Initial step marker — verified at 0x06068dc8 in reference binary */
    debug_status_step(log, 0);

    /* Delay loop */
    for (i = 0; i < 0x8000000; ++i) {
        /* spin */
    }

    /* Step markers and syshub TLB configuration */
    debug_status_step(log, 0);
    debug_status_step(log, 0);
    debug_status_step(log, 0);

    sram_syshub_init(0x22, 0x60000000ULL);
    sram_syshub_init(0x27, 0xc0000000ULL);
    sram_syshub_init(0x28, 0xc0000000ULL);
    sram_syshub_init(0x29, 0xc0000000ULL);

    debug_status_set_x3(log, 0);
    debug_status_set_x4(log, 0);
    debug_status_set_x5(log, 0);

    debug_status_step(log, 0);
    debug_status_step(log, 0);
    debug_status_step(log, 0);

    sram_syshub_init(0x30, 0x80000000ULL);
    sram_syshub_init(0x31, 0xc0000000ULL);
    sram_syshub_init(0x34, 0x880000000ULL);

    /* Binary has 3 debug_status_step calls here (verified at 0x06068f18-0x06068f3c) */
    debug_status_step(log, 0);
    debug_status_step(log, 0);
    debug_status_step(log, 0);

    debug_status_set_x3(log, 0);
    debug_status_step(log, 0);

    sram_syshub_init(0x35, 0x884000000ULL);
    sram_syshub_init(0x36, 0x888000000ULL);
    sram_syshub_init(0x37, 0x88c000000ULL);
    sram_syshub_init(0x38, 0x890000000ULL);
    sram_syshub_init(0x39, 0x894000000ULL);
    sram_syshub_init(0x3a, 0x898000000ULL);
    sram_syshub_init(0x3b, 0x89c000000ULL);

    syshub_init_core(0x0d, 0xc4000000ULL, 0x4000000);
    syshub_init_core(0x20, 0x7c000000ULL, 0x4000000);
    syshub_init_core(0x21, 0x3c000000ULL, 0x4000000);

    /* Final MMIO register writes */
    *(volatile a53_u32 *)0x032303c0ULL = 0x3f;
    *(volatile a53_u32 *)0x032303c4ULL = 0x12;
    *(volatile a53_u32 *)0x032303c8ULL = 0;
    *(volatile a53_u32 *)0x032303ccULL = 0;
    *(volatile a53_u32 *)0x032304d0ULL = 0xffffffff;
    *(volatile a53_u32 *)0x032305c8ULL = 0xc0000003;

    return 0;
}

/* ---- Test function ---- */

void A53_SECTION(".text.reset.loader")
test_uart(void)
{
    volatile a53_u64 *cpddr0 = (volatile a53_u64 *)0xd0000000ULL;
    volatile a53_u64 *cpddr1 = (volatile a53_u64 *)0xec000000ULL;
    volatile a53_u64 *g6mem   = (volatile a53_u64 *)0x88000000ULL;
    a53_u64 val;

    printf_el3_sram("=======================================================\n");
    printf_el3_sram("Hello MP4/A53!!! - SRAM\n");

    printf_el3_sram("Check CPDDR: %p == 0x%08x\n", cpddr0, (unsigned)*cpddr0);
    printf_el3_sram("Check CPDDR: %p == 0x%016lx\n", cpddr1, *cpddr1);

    *cpddr1 = 0xfedcba9876543210ULL;
    printf_el3_sram("Check CPDDR: %p == 0x%016lx\n", cpddr1, 0xfedcba9876543210ULL);

    val = *g6mem;
    printf_el3_sram("Check G6: %p == 0x%016lx\n", g6mem, val);

    *g6mem = 0x89abcdef01234567ULL;
    printf_el3_sram("Check G6: %p == 0x%016lx\n", g6mem, 0x89abcdef01234567ULL);
}

/* ---- SRAM boot sequence ---- */

int A53_SECTION(".text.reset.loader")
el3_sram_boot(a53_u64 *log)
{
    syshub_init_all(log);
    test_uart();

    printf_el3_sram("Install Image from SRAM to G6\n");
    printf_el3_sram("__g6_begin = %p [0x%08x]\n", (void *)0x100000ULL, 0x10000);

    /* TODO: __page_table_ext_core1_end symbol */
    printf_el3_sram("__g6_end   = %p\n", (void *)0x88000000ULL);

    printf_el3_sram("mmMP4_C2PMSG_0:  %p = 0x%08x\n",
                   (void *)0x03010500ULL,
                   *(volatile a53_u32 *)0x03010500ULL);
    printf_el3_sram("mmDVM_MAILBOX_0: %p = 0x%08x\n",
                   (void *)0x030c0000ULL,
                   *(volatile a53_u32 *)0x030c0000ULL);
    printf_el3_sram("mmDVM_MAILBOX_1: %p = 0x%08x\n",
                   (void *)0x030c1000ULL,
                   *(volatile a53_u32 *)0x030c1000ULL);
    printf_el3_sram("mmDVM_MAILBOX_2: %p = 0x%08x\n",
                   (void *)0x030c2000ULL,
                   *(volatile a53_u32 *)0x030c2000ULL);

    *(volatile a53_u32 *)0x03200900ULL = 0xffffffff;
    *(volatile a53_u32 *)0x03200904ULL = 0xffffffff;
    *(volatile a53_u32 *)0x03010500ULL = 0x77777777;

    /* Wait for C2PMSG acknowledgment */
    while (*(volatile a53_u32 *)0x03010504ULL == 0) {
        /* spin */
    }

    printf_el3_sram("mmMP4_C2PMSG_1:  %p = 0x%08x\n",
                   (void *)0x03010504ULL,
                   *(volatile a53_u32 *)0x03010504ULL);
    printf_el3_sram("<-0\n");
    return 0;
}

/* ---- Bridge from SRAM boot to main EL3 boot ----
 *
 * NOTE: cp_param2 was inferred from the w2 register observed in the
 * original Ghidra body (in_w2). The function is called from reset/vector.S
 * which sets w2 from the cp_param value before branching.
 */

int A53_SECTION(".text.reset.loader")
el3_sram_boot2(a53_u64 *log, a53_u32 cp_param2)
{
    printf_el3_sram("el3_sram_boot2 = %p\n", (void *)el3_sram_boot2);
    el3_boot(log, (printf_func_t)printf_low, cp_param2);
    return 0;
}

/* ---- SError handler for SRAM boot ---- */

int A53_SECTION(".text.reset.loader")
el3_sram_serror(a53_u64 *log)
{
    a53_u64 step;
    a53_u64 vector;
    a53_u32 int_status;
    a53_u32 i;

    printf_el3_sram("-------------------------------------------------------\n");
    printf_el3_sram("Trap handler on SRAM\n");

    step   = *log;
    vector = *(log + 1);

    printf_el3_sram("STEP                    = 0x%016lx\n", step);
    printf_el3_sram("VECTOR                  = 0x%016lx\n", vector);

    for (i = 0; i < 6; ++i) {
        printf_el3_sram("X%d                      = 0x%016lx\n",
                        i, *(log + 2 + i));
    }
    printf_el3_sram("SP                      = 0x%016lx\n", *(log + 0x21));
    printf_el3_sram("ESR_EL3                 = 0x%016lx\n", *(log + 0x22));
    printf_el3_sram("ELR_EL3                 = 0x%016lx\n", *(log + 0x23));
    printf_el3_sram("FAR_EL3                 = 0x%016lx\n", *(log + 0x24));

    debug_status_step(log, 0);
    debug_status_set_x3(log, 0);

    int_status = *(volatile a53_u32 *)0x032305d0ULL;
    printf_el3_sram("mmMP4_SYSHUB_INT_STATUS = 0x%08x\n", int_status);

    if ((int_status & 1) != 0) {
        debug_status_set_x4(log, 0);
        printf_el3_sram("mmMP4_SYSHUB_RD_INT_ADDR  = 0x%08x\n",
                       *(volatile a53_u32 *)0x032305e0ULL);
        printf_el3_sram("mmMP4_SYSHUB_RD_INT_OTHER = 0x%08x\n",
                       *(volatile a53_u32 *)0x032305e4ULL);
        debug_status_set_x5(log, 0);
        debug_status_step(log, 0);
    } else if ((int_status >> 1 & 1) != 0) {
        debug_status_set_x4(log, 0);
        printf_el3_sram("mmMP4_SYSHUB_WR_INT_ADDR  = 0x%08x\n",
                       *(volatile a53_u32 *)0x032305d4ULL);
        printf_el3_sram("mmMP4_SYSHUB_WR_INT_OTHER = 0x%08x\n",
                       *(volatile a53_u32 *)0x032305dcULL);
        debug_status_set_x5(log, 0);
        debug_status_step(log, 0);
    } else {
        debug_status_set_x4(log, 0);
        debug_status_set_x5(log, 0);
        printf_el3_sram("mmMP4_NS_PROT_FAULT_STATUS_0 = 0x%08x\n",
                       *(volatile a53_u32 *)0x03200090ULL);
        debug_status_step(log, 0);
    }

    return 0;
}
