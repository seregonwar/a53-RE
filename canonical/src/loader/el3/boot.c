#include "a53_abi.h"
#include "a53_context.h"

extern el3_jmp_buf *el3_jmpbufp;
extern el3_param_t g_param;
extern layout_t g_layout;
extern dev_context_t g_dev_context_el3_core0;
extern dev_context_t g_dev_context_el3_core1;
extern a53_u32 g_count_0;
extern a53_u32 g_count_1;

extern void el3_longjmp(el3_jmp_buf *buf, int val);
extern void *mm_controller_dram_entry;
extern void *io_controller_dram_entry;
extern a53_u64 __loader_el3_text_end;
extern a53_u64 __loader_el3_bss_end;
extern a53_u64 __loader_dev_text_end;
extern a53_u64 __loader_el2_end;
extern a53_u64 __loader_el1_end;
extern a53_u64 __controller_dev_text_end;
extern a53_u64 __controller_dev_data_end;
extern a53_u64 EL3_VBAR;
extern a53_u64 EL2_VBAR;
extern a53_u64 EL1_VBAR;

extern void aarch64_BRK(int code);
extern void mmu_init_phase1(void);
extern void mmu_init_phase2a(void);
extern void mmu_init_phase2b(void);
extern void deci5s_mp4_start(a53_u32 core);
extern void deci5s_mp4_panic_and_loop(a53_u32 cpu, a53_u64 pc);

void A53_SECTION(".text.el3.loader") el3_print_common(void)
{
    a53_u64 uVar8;
    a53_u32 cpu;

    cpu = mp4_get_cpu();
    printf_low("%d:%s:()\n", (a53_u64)cpu, "el3_print_common");

    aarch64_print_CurrentEL();

    cpu = mp4_get_cpu();
    printf_low("%d:%s:DAIF:           0x%016lx\n",
               (a53_u64)cpu, "el3_print_common",
               aarch64_read_DAIF());

    cpu = mp4_get_cpu();
    printf_low("%d:%s:ELR             0x%016lx\n",
               (a53_u64)cpu, "el3_print_common",
               aarch64_read_ELR());

    /* NZCV */
    {
        a53_u64 nzcv;
        __asm__("mrs %0, nzcv" : "=r"(nzcv));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:NZCV            0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", nzcv);
    }

    /* SP_EL0 */
    {
        a53_u64 sp;
        __asm__("mrs %0, sp_el0" : "=r"(sp));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:SP_EL0          0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", sp);
    }

    /* SP_EL1 */
    {
        a53_u64 sp;
        __asm__("mrs %0, sp_el1" : "=r"(sp));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:SP_EL1          0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", sp);
    }

    /* SP_EL2 */
    {
        a53_u64 sp;
        __asm__("mrs %0, sp_el2" : "=r"(sp));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:SP_EL2          0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", sp);
    }

    /* SPSR_EL1 */
    {
        a53_u64 spsr;
        __asm__("mrs %0, spsr_el1" : "=r"(spsr));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:SPSR_EL1        0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", spsr);
    }

    /* SPSR_EL2 */
    {
        a53_u64 spsr;
        __asm__("mrs %0, spsr_el2" : "=r"(spsr));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:SPSR_EL2        0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", spsr);
    }

    /* SPSR_EL3 */
    {
        a53_u64 spsr;
        __asm__("mrs %0, spsr_el3" : "=r"(spsr));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:SPSR_EL3        0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", spsr);
    }

    /* SPSel */
    {
        a53_u64 spsel;
        __asm__("mrs %0, spsel" : "=r"(spsel));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:SPSel           0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", spsel);
    }

    /* MIDR_EL1 */
    {
        a53_u64 midr;
        __asm__("mrs %0, midr_el1" : "=r"(midr));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:MIDR_EL1        0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", midr);
    }

    /* MPIDR_EL1 */
    {
        a53_u64 mpidr;
        __asm__("mrs %0, mpidr_el1" : "=r"(mpidr));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:MPIDR_EL1       0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", mpidr);
    }

    /* CLIDR_EL1 */
    {
        a53_u64 clidr;
        __asm__("mrs %0, clidr_el1" : "=r"(clidr));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:CLIDR_EL1       0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", clidr);
    }

    /* CONTEXTIDR_EL1 */
    {
        a53_u64 ctx;
        __asm__("mrs %0, contextidr_el1" : "=r"(ctx));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:CONTEXTIDR_EL1  0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", ctx);
    }

    /* CSSELR_EL1 */
    {
        a53_u64 csselr;
        __asm__("mrs %0, csselr_el1" : "=r"(csselr));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:CSSELR_EL1      0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", csselr);
    }

    /* ESR_EL1 */
    {
        a53_u64 esr;
        __asm__("mrs %0, esr_el1" : "=r"(esr));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:ESR_EL1         0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", esr);
    }

    /* ESR_EL2 */
    {
        a53_u64 esr;
        __asm__("mrs %0, esr_el2" : "=r"(esr));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:ESR_EL2         0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", esr);
    }

    /* FAR_EL1 */
    {
        a53_u64 far;
        __asm__("mrs %0, far_el1" : "=r"(far));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:FAR_EL1         0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", far);
    }

    /* FAR_EL2 */
    {
        a53_u64 far;
        __asm__("mrs %0, far_el2" : "=r"(far));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:FAR_EL2         0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", far);
    }

    /* FAR_EL3 */
    {
        a53_u64 far;
        __asm__("mrs %0, far_el3" : "=r"(far));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:FAR_EL3         0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", far);
    }

    /* VBAR_EL2 */
    {
        a53_u64 vbar;
        __asm__("mrs %0, vbar_el2" : "=r"(vbar));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:VBAR_EL2        0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", vbar);
    }

    /* VBAR_EL3 */
    {
        a53_u64 vbar;
        __asm__("mrs %0, vbar_el3" : "=r"(vbar));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:VBAR_EL3        0x%016lx\n",
                   (a53_u64)cpu, "el3_print_common", vbar);
    }

    aarch64_print_HCR_EL2();

    /* SCTLR_EL1 */
    {
        a53_u64 sctlr;
        __asm__("mrs %0, sctlr_el1" : "=r"(sctlr));
        cpu = mp4_get_cpu();
        printf_low("%d:SCTLR_EL1       0x%016lx\n", (a53_u64)cpu, sctlr);
    }
    aarch64_print_SCTLR_EL2();

    /* SCTLR_EL3 */
    {
        a53_u64 sctlr;
        __asm__("mrs %0, sctlr_el3" : "=r"(sctlr));
        cpu = mp4_get_cpu();
        printf_low("%d:SCTLR_EL3       0x%016lx\n", (a53_u64)cpu, sctlr);
    }

    /* TCR_EL1 */
    {
        a53_u64 tcr;
        __asm__("mrs %0, tcr_el1" : "=r"(tcr));
        cpu = mp4_get_cpu();
        printf_low("%d:TCR_EL1         0x%016lx\n", (a53_u64)cpu, tcr);
    }

    /* TCR_EL2 */
    {
        a53_u64 tcr;
        __asm__("mrs %0, tcr_el2" : "=r"(tcr));
        cpu = mp4_get_cpu();
        printf_low("%d:TCR_EL2         0x%016lx\n", (a53_u64)cpu, tcr);
    }

    /* TPIDR_EL0 */
    {
        a53_u64 tp;
        __asm__("mrs %0, tpidr_el0" : "=r"(tp));
        cpu = mp4_get_cpu();
        printf_low("%d:TPIDR_EL0       0x%016lx\n", (a53_u64)cpu, tp);
    }

    /* TPIDR_EL1 */
    {
        a53_u64 tp;
        __asm__("mrs %0, tpidr_el1" : "=r"(tp));
        cpu = mp4_get_cpu();
        printf_low("%d:TPIDR_EL1       0x%016lx\n", (a53_u64)cpu, tp);
    }

    /* TPIDR_EL2 */
    {
        a53_u64 tp;
        __asm__("mrs %0, tpidr_el2" : "=r"(tp));
        cpu = mp4_get_cpu();
        printf_low("%d:TPIDR_EL2       0x%016lx\n", (a53_u64)cpu, tp);
    }

    /* TPIDR_EL3 */
    {
        a53_u64 tp;
        __asm__("mrs %0, tpidr_el3" : "=r"(tp));
        cpu = mp4_get_cpu();
        printf_low("%d:TPIDR_EL3       0x%016lx\n", (a53_u64)cpu, tp);
    }

    /* TPIDRRO_EL0 */
    {
        a53_u64 tp;
        __asm__("mrs %0, tpidrro_el0" : "=r"(tp));
        cpu = mp4_get_cpu();
        printf_low("%d:TPIDRRO_EL0     0x%016lx\n", (a53_u64)cpu, tp);
    }

    /* TTBR0_EL1 */
    {
        a53_u64 ttbr;
        __asm__("mrs %0, ttbr0_el1" : "=r"(ttbr));
        cpu = mp4_get_cpu();
        printf_low("%d:TTBRO_EL1       0x%016lx\n", (a53_u64)cpu, ttbr);
    }

    /* TTBR0_EL2 */
    {
        a53_u64 ttbr;
        __asm__("mrs %0, ttbr0_el2" : "=r"(ttbr));
        cpu = mp4_get_cpu();
        printf_low("%d:TTBRO_EL2       0x%016lx\n", (a53_u64)cpu, ttbr);
    }

    /* TTBR0_EL3 */
    {
        a53_u64 ttbr;
        __asm__("mrs %0, ttbr0_el3" : "=r"(ttbr));
        cpu = mp4_get_cpu();
        printf_low("%d:TTBRO_EL3       0x%016lx\n", (a53_u64)cpu, ttbr);
    }

    /* TTBR1_EL1 */
    {
        a53_u64 ttbr;
        __asm__("mrs %0, ttbr1_el1" : "=r"(ttbr));
        cpu = mp4_get_cpu();
        printf_low("%d:TTBR1_EL1       0x%016lx\n", (a53_u64)cpu, ttbr);
    }

    /* CBAR_EL1 */
    {
        a53_u64 cbar;
        __asm__("mrs %0, cbar_el1" : "=r"(cbar));
        cpu = mp4_get_cpu();
        printf_low("%d:CBAR_EL1        0x%016lx\n", (a53_u64)cpu, cbar);
    }
}

void *A53_SECTION(".text.el3.loader") el3_jmpbuf_enter(el3_jmp_buf *n)
{
    el3_jmp_buf *prev;

    prev = el3_jmpbufp;
    el3_jmpbufp = n;
    return prev;
}

void A53_SECTION(".text.el3.loader") el3_jmpbuf_exit(el3_jmp_buf *n)
{
    el3_jmpbufp = n;
}

static void A53_SECTION(".text.el3.loader")
el3_show_steps(printf_func_t printf_func, char *msg,
                a53_u64 *log, a53_u64 *log_another)
{
    printf_func("----- %s -----\n");
    printf_func("log        =%p, step=0x%016lx\n", log, *log);
    printf_func("log_another=%p, step=0x%016lx\n", log_another, *log_another);
}

int A53_SECTION(".text.el3.loader")
el3_boot(a53_u64 *log, printf_func_t printf_func, a53_u32 cp_param2)
{
    a53_u64 *log_another;
    a53_u32 cpu;
    a53_u32 cpu_next;
    a53_u32 *piVar1;
    dev_context_t *dc;
    char *pcVar7;

    cpu = mp4_get_cpu();
    log_another = (a53_u64 *)0x1d00;
    if (cpu == 0) {
        printf_func("================================================\n");
        printf_func("el3_boot(cp_param2 0x%08x) 0.5.0.01 [2018-11-13]\n",
                   (a53_u64)cp_param2, 0x50202, 9);
        printf_func("el3_bss:SKIP:  0x%08x-0x%08x\n",
                   &g_count_0, &__loader_el3_bss_end);
        printf_func("dev_bss:CLEAR:  0x%08x-0x%08x\n", EL2_VBAR, EL2_VBAR);

        g_param.core0_main = mm_controller_dram_entry;
        g_param.core0_dev_context = &g_dev_context_el3_core0;
        g_param.log0 = (a53_u64 *)0x1900;
        g_param.core1_main = io_controller_dram_entry;
        g_param.core1_dev_context = &g_dev_context_el3_core1;
        g_param.log1 = (a53_u64 *)0x1d00;

        /* Handshake: wait for core1 at step 0x600 */
        el3_show_steps(printf_func, "el3_boot 1st entry", log, (a53_u64 *)0x1d00);
        *log = 0x600;
        el3_show_steps(printf_func, "wait 0x600 ANOTHER CORE", log, (a53_u64 *)0x1d00);
        while (*(a53_u64 *)0x1d00 < 0x600) {
        }
        el3_show_steps(printf_func, "after 0x600 ANOTHER CORE", log, (a53_u64 *)0x1d00);
    } else {
        log_another = (a53_u64 *)0x1900;
        *log = 0x6fe;
        while (*(a53_u64 *)0x1900 < 0x700) {
        }
        *log = 0x6ff;
    }

    /* Set up putchar hooks based on cp_param2 flags */
    cpu_next = mp4_get_cpu();
    if (cpu_next == 0) {
        if ((cp_param2 & 0x2000200U) == 0x200U) {
            printf_func("USE PERICOM\n");
            g_dev_context_el3_core0.dc_putchar_low_hook =
                (void *)putchar_pericom;
        } else {
            g_dev_context_el3_core0.dc_putchar_low_hook =
                (void *)putchar_cp;
        }
        dc = &g_dev_context_el3_core0;
    } else {
        g_dev_context_el3_core1.dc_putchar_low_hook =
            (void *)putchar_cp;
        if (((cp_param2 >> 9 & 1) != 0)
            && ((cp_param2 & 0x20000U) != 0)) {
            g_dev_context_el3_core1.dc_putchar_low_hook =
                (void *)putchar_cp;
        }
        dc = &g_dev_context_el3_core1;
    }
    dev_context_init_for_el3(dc);
    mp4_debug_status_init();

    if (cpu == 0) {
        printf_low("----- %s -----\n", "el3_boot 1st entry");
        printf_low("log        =%p, step=0x%016lx\n", log, *log);
        printf_low("log_another=%p, step=0x%016lx\n", log_another, *log_another);
        *log = 0x700;
        printf_low("----- %s -----\n", "wait 0x700 ANOTHER CORE");
        printf_low("log        =%p, step=0x%016lx\n", log, *log);
        printf_low("log_another=%p, step=0x%016lx\n", log_another, *log_another);
        while (*log_another < 0x700) {
        }
        el3_show_steps(printf_func, "after 0x700 ANOTHER CORE", log, log_another);
    } else {
        *log = 0x7fe;
        while (*log_another < 0x800) {
        }
        *log = 0x7ff;
    }

    printf_low("================================================\n");
    printf_low("el3_boot(cp_param2 0x%08x) 0.5.0.01 [2018-11-13]\n",
               (a53_u64)cp_param2, 0x50202, 9);
    printf_low("call from printf_low\n");

    cpu = mp4_get_cpu();
    if (cpu == 0) {
        cp_param_init();

        /* Set up layout */
        g_layout.msl_loader_el3_text.msi_begin = 0x100000ULL;
        g_layout.msl_loader_el3_text.msi_end =
            (a53_u64)&__loader_el3_text_end;
        g_layout.msl_loader_el3_text.msi_page_size = 0x15000ULL;
        g_layout.msl_loader_el3_text.msi_sram = 0x10000ULL;
        g_layout.msl_loader_el3_text.msi_pa = 0x88000000ULL;

        g_layout.msl_loader_el3_data.msi_begin = (a53_u64)&g_layout;
        g_layout.msl_loader_el3_data.msi_end =
            (a53_u64)&__loader_el3_bss_end;
        g_layout.msl_loader_el3_data.msi_page_size = 0x2000ULL;
        g_layout.msl_loader_el3_data.msi_sram = 0x25000ULL;
        g_layout.msl_loader_el3_data.msi_pa = 0x88015000ULL;

        g_layout.msl_loader_dev_text.msi_begin = (a53_u64)&mp4_get_cpu;
        g_layout.msl_loader_dev_text.msi_end =
            (a53_u64)&__loader_dev_text_end;
        g_layout.msl_loader_dev_text.msi_page_size = 0xd000ULL;
        g_layout.msl_loader_dev_text.msi_sram = 0x27000ULL;
        g_layout.msl_loader_dev_text.msi_pa = 0x88017000ULL;

        g_layout.msl_loader_dev_data.msi_begin = (a53_u64)EL2_VBAR;
        g_layout.msl_loader_dev_data.msi_end = (a53_u64)EL2_VBAR;
        g_layout.msl_loader_dev_data.msi_page_size = 0;
        g_layout.msl_loader_dev_data.msi_sram = 0x34000ULL;
        g_layout.msl_loader_dev_data.msi_pa = 0x88024000ULL;

        g_layout.msl_loader_el2.msi_begin = (a53_u64)EL2_VBAR;
        g_layout.msl_loader_el2.msi_end = (a53_u64)&__loader_el2_end;
        g_layout.msl_loader_el2.msi_page_size = 0x1000ULL;
        g_layout.msl_loader_el2.msi_sram = 0x34000ULL;
        g_layout.msl_loader_el2.msi_pa = 0x88024000ULL;

        g_layout.msl_loader_el1.msi_begin = (a53_u64)EL1_VBAR;
        g_layout.msl_loader_el1.msi_end = (a53_u64)&__loader_el1_end;
        g_layout.msl_loader_el1.msi_page_size = 0x1000ULL;
        g_layout.msl_loader_el1.msi_sram = 0x35000ULL;
        g_layout.msl_loader_el1.msi_pa = 0x88025000ULL;

        g_layout.msl_controller_dev_text.msi_begin = 0x6000000ULL;
        g_layout.msl_controller_dev_text.msi_end =
            (a53_u64)&__controller_dev_text_end;
        g_layout.msl_controller_dev_text.msi_page_size = 0x6a000ULL;
        g_layout.msl_controller_dev_text.msi_sram = 0x30000ULL;
        g_layout.msl_controller_dev_text.msi_pa = 0x88020000ULL;

        g_layout.msl_controller_dev_data.msi_begin =
            (a53_u64)&g_dev_context_mm;
        g_layout.msl_controller_dev_data.msi_end =
            (a53_u64)&__controller_dev_data_end;
        g_layout.msl_controller_dev_data.msi_page_size = 0x1000ULL;
        g_layout.msl_controller_dev_data.msi_sram = 0x40000ULL;
        g_layout.msl_controller_dev_data.msi_pa = 0x88030000ULL;

        *log = 0x1000;
    } else {
        while (*log_another < 0x1000) {
        }
    }
    printf_low("after 0x1000\n");

    /* Counters */
    cpu = mp4_get_cpu();
    if (cpu != 0) {
        piVar1 = &g_count_1;
    } else {
        piVar1 = &g_count_0;
    }
    *piVar1 = 0;
    el3_jmpbufp = (el3_jmp_buf *)0;

    /* CPUECTLR_EL1 */
    {
        a53_u64 cpuectlr;

        __asm__("mrs %0, s3_1_c15_c2_1" : "=r"(cpuectlr));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:CPUECTLR_EL1:   0x%016lx\n",
                   (a53_u64)cpu, "el3_boot", cpuectlr);
    }

    /* Size checks */
    pcVar7 = mp4_basename("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\boot.c");
    el3_assert(pcVar7, "el3_boot", 0x2ca, 1, "sizeof(int8_t) == 1");
    el3_assert(pcVar7, "el3_boot", 0x2cb, 1, "sizeof(uint8_t) == 1");
    el3_assert(pcVar7, "el3_boot", 0x2cc, 1, "sizeof(int16_t) == 2");
    el3_assert(pcVar7, "el3_boot", 0x2cd, 1, "sizeof(uint16_t) == 2");
    el3_assert(pcVar7, "el3_boot", 0x2ce, 1, "sizeof(int32_t) == 4");
    el3_assert(pcVar7, "el3_boot", 0x2cf, 1, "sizeof(uint32_t) == 4");
    el3_assert(pcVar7, "el3_boot", 0x2d0, 1, "sizeof(int64_t) == 8");
    el3_assert(pcVar7, "el3_boot", 0x2d1, 1, "sizeof(uint64_t) == 8");

    /* Print and set VBAR_EL3 */
    {
        a53_u64 vbar;

        __asm__("mrs %0, vbar_el3" : "=r"(vbar));
        cpu = mp4_get_cpu();
        printf_low("%d:%s:_loader_el3_vector     : %p\n",
                   (a53_u64)cpu, "el3_boot", (void *)vbar);
        __asm__ volatile("msr vbar_el3, %0" : : "r"(0x107000ULL));
    }

    cpu = mp4_get_cpu();
    if (cpu == 0) {
        aarch64_ccahe_op_init();
        mmu_init_phase1();
        mmu_init_phase2a();
        *log = 0x1100;
    } else {
        while (*log_another >> 8 < 0x11) {
        }
    }
    printf_low("%d:%s:after 0x1100\n", (a53_u64)mp4_get_cpu(), "el3_boot");

    cpu = mp4_get_cpu();
    if (cpu == 0) {
        mmu_init_phase2b();
        syshub_init();
        *log = 0x1200;
    } else {
        while (*log_another >> 9 < 9) {
        }
    }
    printf_low("%d:%s:after 0x1200\n", (a53_u64)mp4_get_cpu(), "el3_boot");

    cpu = mp4_get_cpu();
    if (cpu == 0) {
        c2pmsg_init();
        dvm_init();
        arm_timer_init();
        mp4_timer_init();
        *log = 0x1300;
    } else {
        arm_timer_init();
        while (*log_another >> 8 < 0x13) {
        }
    }
    printf_low("%d:%s:after 0x1300\n", (a53_u64)mp4_get_cpu(), "el3_boot");

    cpu = mp4_get_cpu();
    if (cpu == 0) {
        msi_get_info_by_1st_core();
        syshub_init_after_main_param();
        smnif_init();
        gic_init_by_1st_core();
        gic_v2m_init();

        *log = 0x13ff;
        while (*log_another < 0x13ff) {
        }
        *log = 0x1400;
        printf_low("%d:%s:after step 0x1400!\n",
                   (a53_u64)mp4_get_cpu(), "el3_boot");
        msi_send_command_sync(0, 0x10030000);
    } else {
        *log = 0x13ff;
        while (*log_another < 0x13ff) {
        }
        *log = 0x1400;
        printf_low("%d:%s:after step 0x1400!\n",
                   (a53_u64)mp4_get_cpu(), "el3_boot");
        gic_init_by_2nd_core();
        msi_get_info_by_2nd_core();
    }

    if (msi_has_internal_qaf() == 0) {
        printf_low("%d:%s:No QAF!!!\n",
                   (a53_u64)mp4_get_cpu(), "el3_boot");
    } else {
        if (cp_param_check(0x100) == 0) {
            cpu = mp4_get_cpu();
            if (cpu == 0) {
                deci5s_mp4_start(0);
                msi_send_command_sync(0, 0x10070000);

                *log = 0x15ff;
                while (*log_another < 0x15ff) {
                }
                *log = 0x1600;
                printf_low("%d:%s:after step 0x1600!\n",
                           (a53_u64)mp4_get_cpu(), "el3_boot");

                *log = 0x16ff;
                while (*log_another < 0x16ff) {
                }
            } else {
                *log = 0x15ff;
                while (*log_another < 0x15ff) {
                }
                *log = 0x1600;
                printf_low("%d:%s:after step 0x1600!\n",
                           (a53_u64)mp4_get_cpu(), "el3_boot");
                deci5s_mp4_start(1);

                *log = 0x16ff;
                while (*log_another < 0x16ff) {
                }
            }
            *log = 0x1700;
            printf_low("%d:%s:after step 0x1700!\n",
                       (a53_u64)mp4_get_cpu(), "el3_boot");
        }
    }

    cpu = mp4_get_cpu();
    if (cpu == 0) {
        msi_send_command_sync(0, 0x10080000);
        *log = 0x17ff;
        while (*log_another < 0x17ff) {
        }
    } else {
        *log = 0x17ff;
        while (*log_another < 0x17ff) {
        }
    }
    *log = 0x1800;
    printf_low("%d:%s:after step 0x1800!\n",
               (a53_u64)mp4_get_cpu(), "el3_boot");

    el0_support(&g_param, cp_param2);

    printf_low("%d:%s:STOP MP4/A53\n",
               (a53_u64)mp4_get_cpu(), "el3_boot");
    for (;;) {
    }
}

void A53_SECTION(".text.el3.loader")
el3_serror_handler(a53_u64 x0, a53_u64 vector)
{
    dev_context_t *dc;
    mp4_debug_status_t *status;
    a53_u32 cpu;
    a53_u32 *piVar1;
    a53_u64 elr_el1;
    a53_u64 elr_el2;
    a53_u64 elr_el3;
    a53_u64 esr;
    a53_u64 spsr;
    a53_u64 currentel;
    a53_u64 far;
    a53_u64 far_el3_val;
    a53_u64 ec;

    dc = get_dev_context();
    status = mp4_debug_status_get();
    cpu = mp4_get_cpu();

    /* mp4_debug_status_c_set inline */
    if (cpu != 0) {
        piVar1 = &g_count_1;
    } else {
        piVar1 = &g_count_0;
    }
    *piVar1 = *piVar1 + 1;

    __asm__("mrs %0, CurrentEL" : "=r"(currentel));
    if ((currentel & 0xcULL) == 0) {
        printf_low("%d:%s:Invalid CurrentEL=0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                   (a53_u32)currentel);
        for (;;) {
        }
    }

    __asm__("mrs %0, elr_el1" : "=r"(elr_el1));
    __asm__("mrs %0, elr_el2" : "=r"(elr_el2));
    __asm__("mrs %0, elr_el3" : "=r"(elr_el3));
    __asm__("mrs %0, esr_el3" : "=r"(esr));
    __asm__("mrs %0, spsr_el3" : "=r"(spsr));

    ec = (esr >> 26) & 0x3fULL;
    {
        a53_u64 vec_type;
        a53_u64 adjusted;

        vec_type = vector & 0x7ffULL;
        adjusted = vector & 0x7ffULL;

        if (vec_type >= 0x200 && vec_type < 0x400) {
            adjusted = vector - 0x200;
        } else if (vec_type >= 0x400 && vec_type < 0x600) {
            adjusted = vector - 0x400;
        } else if (vec_type >= 0x600 && vec_type < 0x800) {
            adjusted = vector - 0x600;
        }
        if (adjusted >= 4) {
            printf_low("Unknown vector_offset=0x%016lx\n");
        }
    }

    /* Check for MSI interrupt via GIC */
    if (vector == 0x480 || vector == 0x280) {
        a53_u32 v;

        v = gic_read_GICC_IAR();
        if (v == 0x53 || v == 0x4e) {
            a53_u32 command;
            a53_u32 bits;

            command = msi_read_c2p_command(cpu);
            bits = msi_read_c2p_arg1(cpu);
            if (msi_has_internal_qaf() != 0) {
                if (command >> 12 == 0x20211) {
                    deci_target_mp4_intr(bits);
                } else {
                    printf_low("%d:%s:Unsupport command 0x%08x\n",
                               (a53_u64)mp4_get_cpu(),
                               "el3_serror_handler",
                               (a53_u64)command);
                }
            }
            msi_write_c2p_ack(cpu, command);
        } else {
            printf_low("%d:%s:Unsupport IID = 0x%08x\n",
                       (a53_u64)mp4_get_cpu(),
                       "el3_serror_handler", (a53_u64)v);
        }
        gic_write_GICC_EOIR(v);
        if (vector == 0x480) {
            dc = get_dev_context();
            if (dc->dc_dev_context_el0 != (dev_context_t *)0) {
                a53_u64 ctx;

                ctx = (a53_u64)dc->dc_dev_context_el0;
                dc->dc_dev_context_el0 = (dev_context_t *)0;
                __asm__ volatile("msr tpidrro_el0, %0" : : "r"(ctx));
            }
        }
        return;
    }

    /* SVC handling for EL1 */
    if (elr_el1 != 0) {
        a53_u64 esr_el1;

        __asm__("mrs %0, spsr_el1" : "=r"(spsr));
        __asm__("mrs %0, esr_el1" : "=r"(esr_el1));

        if (((esr_el1 >> 26) & 0x3fULL) == 0x15ULL) {
            svc_EL3((a53_u32)esr_el1, status);
            __asm__ volatile("msr spsr_el3, %0" : : "r"(spsr));
            __asm__ volatile("msr elr_el3, %0" : : "r"(elr_el1));
            dc = get_dev_context();
            if (dc->dc_dev_context_el0 != (dev_context_t *)0) {
                a53_u64 ctx;

                ctx = (a53_u64)dc->dc_dev_context_el0;
                dc->dc_dev_context_el0 = (dev_context_t *)0;
                __asm__ volatile("msr tpidrro_el0, %0" : : "r"(ctx));
            }
            return;
        }
        printf_low("%d:%s:Unsupport EC\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler");
        aarch64_print_ESR_EL1();
    }

    /* Crash: print state */
    printf_low("%d:%s:<<<<< <<<<< vector=0x%08x >>>>> >>>>>\n",
               (a53_u64)mp4_get_cpu(), "el3_serror_handler",
               (a53_u32)vector);
    printf_low("%d:%s:(0x%016lx, 0x%016lx)\n",
               (a53_u64)mp4_get_cpu(), "el3_serror_handler",
               x0, vector);

    /* Print EL1 state */
    if (elr_el1 != 0) {
        __asm__("mrs %0, far_el1" : "=r"(far));
        printf_low("%d:%s:----- EL1 -----\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler");
        aarch64_print_SCTLR_EL1();
        aarch64_print_SPSR_EL1();
        aarch64_print_ESR_EL1();
        printf_low("%d:%-16s = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "ELR_EL1", elr_el1);
        printf_low("%d:%-16s = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "FAR_EL1", far);

        if (((esr >> 26) & 0x3fULL) == 0x24ULL) {
            printf_low("%d:%s:AARCH64_ESR_ELx_EC_DATA_ABORT_LOW_EL\n",
                       (a53_u64)mp4_get_cpu(), "el3_serror_handler");
            if ((esr >> 25 & 1) != 0) {
                printf_low("%d:%s:- IL\n",
                           (a53_u64)mp4_get_cpu(), "el3_serror_handler");
            }
            if ((esr >> 6 & 1) != 0) {
                printf_low("%d:%s:- Write Operation\n",
                           (a53_u64)mp4_get_cpu(), "el3_serror_handler");
            }
            if ((esr & 0x3fULL) == 0x21ULL) {
                printf_low("%d:%s:- Alignment fault\n",
                           (a53_u64)mp4_get_cpu(), "el3_serror_handler");
            }
        }
    }

    /* Print EL2 state */
    if (elr_el2 != 0) {
        __asm__("mrs %0, far_el2" : "=r"(far));
        printf_low("%d:%s:----- EL2 -----\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler");
        aarch64_print_HCR_EL2();
        aarch64_print_SCTLR_EL2();
        aarch64_print_SPSR_EL2();
        aarch64_print_ESR_EL2();
        printf_low("%d:%-16s = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "ELR_EL2", elr_el2);
        printf_low("%d:%-16s = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "FAR_EL2", far);
    }

    /* Print EL3 state */
    __asm__("mrs %0, far_el3" : "=r"(far_el3_val));
    printf_low("%d:%s:----- EL3 -----\n",
               (a53_u64)mp4_get_cpu(), "el3_serror_handler");
    aarch64_print_SCR_EL3();
    aarch64_print_SCTLR_EL3();
    aarch64_print_SPSR_EL3();
    aarch64_print_ESR_EL3();
    printf_low("%d:%-16s = 0x%016lx\n",
               (a53_u64)mp4_get_cpu(), "ELR_EL3", elr_el3);
    printf_low("%d:%-16s = 0x%016lx\n",
               (a53_u64)mp4_get_cpu(), "FAR_EL3", far_el3_val);

    /* Show backtrace from debug status */
    {
        a53_u32 i;

        for (i = 0; i < 16; ++i) {
            a53_u64 fp;
            a53_u64 sp;

            fp = status->mds_gpr[0x1d - (a53_s32)i] ? *((a53_u64 *)(status->mds_gpr[0x1d - (a53_s32)i] + 0)) : 0;
            sp = (a53_u64)&status->mds_sp;
            (void)fp; (void)sp;
        }
    }

    /* Print GPR dump */
    {
        a53_u32 i;

        printf_low("%d:%s:GPR[x0]   = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                   status->mds_gpr[0]);
        printf_low("%d:%s:GPR[x1]   = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                   status->mds_gpr[1]);
        printf_low("%d:%s:GPR[x2]   = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                   status->mds_gpr[2]);
        printf_low("%d:%s:GPR[x3]   = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                   status->mds_gpr[3]);
        printf_low("%d:%s:GPR[x4]   = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                   status->mds_gpr[4]);
        printf_low("%d:%s:GPR[x5]   = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                   status->mds_gpr[5]);
        printf_low("%d:%s:GPR[x6]   = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                   status->mds_gpr[6]);
        printf_low("%d:%s:GPR[x7]   = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                   status->mds_gpr[7]);

        for (i = 8; i < 19; ++i) {
            printf_low("%d:%s:GPR[x%d]  = 0x%016lx\n",
                       (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                       i, status->mds_gpr[i]);
        }
        for (i = 21; i <= 30; ++i) {
            printf_low("%d:%s:GPR[x%d]  = 0x%016lx\n",
                       (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                       i, status->mds_gpr[i]);
        }
    }

    /* SYSHUB interrupt status */
    {
        a53_u32 stat;

        stat = *(volatile a53_u32 *)0x032305d0ULL;
        printf_low("%d:%s:mmMP4_SYSHUB_INT_STATUS=0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                   (a53_u64)stat);

        if ((stat & 1) != 0) {
            printf_low("%d:%s:mmMP4_SYSHUB_RD_INT_ADDR  = 0x%08x\n",
                       (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                       (a53_u64)*(volatile a53_u32 *)0x032305e0ULL);
            printf_low("%d:%s:mmMP4_SYSHUB_RD_INT_OTHER = 0x%08x\n",
                       (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                       (a53_u64)*(volatile a53_u32 *)0x032305e4ULL);
        } else if ((stat >> 1 & 1) != 0) {
            printf_low("%d:%s:mmMP4_SYSHUB_WR_INT_ADDR  = 0x%08x\n",
                       (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                       (a53_u64)*(volatile a53_u32 *)0x032305d4ULL);
            printf_low("%d:%s:mmMP4_SYSHUB_WR_INT_OTHER = 0x%08x\n",
                       (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                       (a53_u64)*(volatile a53_u32 *)0x032305dcULL);
        } else {
            printf_low("%d:%s:mmMP4_NS_PROT_FAULT_STATUS_0 = 0x%08x\n",
                       (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                       (a53_u64)*(volatile a53_u32 *)0x03200090ULL);
        }
    }

    if (dc->dc_exception_nest > 3) {
        printf_low("%d:%s:dc_exceion_nest=%d, Enter LOOP\n",
                   (a53_u64)mp4_get_cpu(), "el3_serror_handler",
                   (a53_u64)dc->dc_exception_nest);
        for (;;) {
        }
    }

    ++dc->dc_exception_nest;
    if (msi_has_internal_qaf() != 0 && cp_param_check(0x100) == 0) {
        deci5s_mp4_panic_and_loop(cpu, elr_el3);
    }
    --dc->dc_exception_nest;

    printf_low("%d:%s:Enter LOOP\n",
               (a53_u64)mp4_get_cpu(), "el3_serror_handler");
    for (;;) {
    }
}
