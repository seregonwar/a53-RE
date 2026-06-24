#include "a53_abi.h"
#include "a53_context.h"

#define SMNIF_TLB0 ((volatile a53_u32 *)0x03220000UL)
#define SMNIF_ADDR ((volatile a53_u32 *)0x0100a000UL)
#define SMNIF_REG0 ((volatile a53_u32 *)0x01000018UL)
#define SMNIF_REG1 ((volatile a53_u32 *)0x0100001cUL)

int A53_SECTION(".text.el3.loader") smnif_init(void)
{
    a53_u32 cpu;

    cpu = mp4_get_cpu();
    printf_low("%d:%s:()\n", (a53_u64)cpu, "smnif_init");

    *SMNIF_TLB0 = 0x24;
    cpu = mp4_get_cpu();
    printf_low("%d:%s:mmSMNIF_TLB_0: 0x%08lx = 0x%08x\n",
               (a53_u64)cpu, "smnif_init",
               (a53_u64)(a53_u64)SMNIF_TLB0, (a53_u64)*SMNIF_TLB0);

    __asm__ volatile("dsb sy");
    __asm__ volatile("isb");

    cpu = mp4_get_cpu();
    printf_low("%d:%s:addr:0x%08lx = 0x%08x\n",
               (a53_u64)cpu, "smnif_init",
               (a53_u64)(a53_u64)SMNIF_ADDR, (a53_u64)*SMNIF_ADDR);
    printf_low("%d:%s:addr:0x%08lx = 0x%08x\n",
               (a53_u64)cpu, "smnif_init",
               (a53_u64)(a53_u64)SMNIF_REG0, (a53_u64)*SMNIF_REG0);
    printf_low("%d:%s:addr:0x%08lx = 0x%08x\n",
               (a53_u64)cpu, "smnif_init",
               (a53_u64)(a53_u64)SMNIF_REG1, (a53_u64)*SMNIF_REG1);
    return 0;
}
