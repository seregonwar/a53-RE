#include "a53_abi.h"
#include "a53_context.h"

int printf(char *format, ...);

int A53_SECTION(".text.dev.loader") dev_pmu_init(void)
{
    a53_u64 v;

    __asm__("mrs %0, pmcr_el0" : "=r"(v));
    printf("PMCR_EL0      = 0x%08x\n", (a53_u32)v);
    __asm__("mrs %0, pmuserenr_el0" : "=r"(v));
    printf("PMUSERENR_EL0 = 0x%08x\n", (a53_u32)v);
    __asm__("mrs %0, pmccntr_el0" : "=r"(v));
    printf("PMCCNTR_EL0   = 0x%08X [0x41033000]\n", (a53_u32)v);
    __asm__("mrs %0, pmceid0_el0" : "=r"(v));
    printf("PMCEID0_EL0   = 0x%08X [0x67FFBFFF]\n", (a53_u32)v);
    __asm__("mrs %0, pmceid1_el0" : "=r"(v));
    printf("PMCEID1_EL0   = 0x%08X [0x00000000]\n", (a53_u32)v);
    __asm__("mrs %0, pmevtyper0_el0" : "=r"(v));
    printf("PMCCFILTR_EL0 = 0x%08X [0x00000000]\n", (a53_u32)v);
    __asm__("mrs %0, pmccfiltr_el0" : "=r"(v));
    printf("PMCCFILTR_EL0 = 0x%08X [0x00000000]\n", (a53_u32)v);
    /* Keep current PMCCFILTR_EL0 */
    __asm__("msr pmccfiltr_el0, %0" : : "r"(v));
    return 0;
}

int A53_SECTION(".text.dev.loader") dev_pmu_setup_default(void)
{
    __asm__("msr pmevtyper0_el0, %0" : : "r"(0x8000001UL));
    __asm__("msr pmevtyper1_el0, %0" : : "r"(0x8000002UL));
    __asm__("msr pmevtyper2_el0, %0" : : "r"(0x8000003UL));
    __asm__("msr pmevtyper3_el0, %0" : : "r"(0x8000008UL));
    __asm__("msr pmevtyper4_el0, %0" : : "r"(0x8000015UL));
    __asm__("msr pmevtyper5_el0, %0" : : "r"(0x8000017UL));
    __asm__("msr pmcntenset_el0, %0" : : "r"(0x8000003fUL));
    return 0;
}

void A53_SECTION(".text.dev.loader")
pmu_print_count(a53_u32 type, a53_u32 count)
{
    static const char *names[] = {
        "Instruction architecturally executed",
        "Cycle",
        "Branch miss",
        "L1I cache miss",
        "L1D cache miss",
        "L1D cache access",
        "TLB instruction miss",
        "TLB data miss",
        "STALL_FRONTEND",
        "STALL_BACKEND",
        "BUS_ACCESS",
        "BUS_CYCLES",
        "CHAIN",
        [0x15] = "L1I_CACHE_REFILL",
        [0x16] = "L1I_TLB_REFILL",
        [0x17] = "L1D_CACHE",
    };
    const char *name;

    if ((type & 0x3ff) < 0x18 && names[type & 0x3ff] != NULL) {
        name = names[type & 0x3ff];
    } else {
        name = "Unknown";
    }
    printf("    0x%08x - 0x%08x  [%s]\n", (a53_u64)count,
           (a53_u64)type, name);
}

int A53_SECTION(".text.dev.loader") dev_pmu_report(void)
{
    a53_u64 v;

    printf("PMU (Performance Monitor Unit) REPORT\n");
    __asm__("mrs %0, pmcr_el0" : "=r"(v));
    printf("    PMCR_EL0                     - 0x%08X\n", (a53_u32)v);
    __asm__("mrs %0, pmccntr_el0" : "=r"(v));
    printf("    0x%08x - PMCCNTR_EL0 [Cycle Count]\n", (a53_u32)v);

    {
        a53_u64 typer0, cntr0;
        __asm__("mrs %0, pmevtyper0_el0" : "=r"(typer0));
        __asm__("mrs %0, pmevcntr0_el0" : "=r"(cntr0));
        pmu_print_count((a53_u32)typer0, (a53_u32)cntr0);
    }
    {
        a53_u64 typer1, cntr1;
        __asm__("mrs %0, pmevtyper1_el0" : "=r"(typer1));
        __asm__("mrs %0, pmevcntr1_el0" : "=r"(cntr1));
        pmu_print_count((a53_u32)typer1, (a53_u32)cntr1);
    }
    {
        a53_u64 typer2, cntr2;
        __asm__("mrs %0, pmevtyper2_el0" : "=r"(typer2));
        __asm__("mrs %0, pmevcntr2_el0" : "=r"(cntr2));
        pmu_print_count((a53_u32)typer2, (a53_u32)cntr2);
    }
    {
        a53_u64 typer3, cntr3;
        __asm__("mrs %0, pmevtyper3_el0" : "=r"(typer3));
        __asm__("mrs %0, pmevcntr3_el0" : "=r"(cntr3));
        pmu_print_count((a53_u32)typer3, (a53_u32)cntr3);
    }
    {
        a53_u64 typer4, cntr4;
        __asm__("mrs %0, pmevtyper4_el0" : "=r"(typer4));
        __asm__("mrs %0, pmevcntr4_el0" : "=r"(cntr4));
        pmu_print_count((a53_u32)typer4, (a53_u32)cntr4);
    }
    {
        a53_u64 typer5, cntr5;
        __asm__("mrs %0, pmevtyper5_el0" : "=r"(typer5));
        __asm__("mrs %0, pmevcntr5_el0" : "=r"(cntr5));
        pmu_print_count((a53_u32)typer5, (a53_u32)cntr5);
    }
    return 0;
}
