#include "a53_abi.h"
#include "a53_context.h"

int A53_SECTION(".text.el3.loader") arm_timer_init(void)
{
    a53_u64 val;

    /* Enable CNTP for EL0 (set bit 0 of CNTKCTL_EL1). */
    __asm__ volatile("mrs %0, cntkctl_el1" : "=r"(val));
    val |= 1;
    __asm__ volatile("msr cntkctl_el1, %0" : : "r"(val));

    printf_low("%d:%s:CNTKCTL_EL1 :    0x%08x\n",
               (a53_u64)mp4_get_cpu(), "arm_timer_init",
               (a53_u32)val);

    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(val));
    printf_low("%d:%s:CNTFRQ_EL0 :     0x%08x\n",
               (a53_u64)mp4_get_cpu(), "arm_timer_init",
               (a53_u32)val);

    __asm__ volatile("mrs %0, cntp_ctl_el0" : "=r"(val));
    printf_low("%d:%s:CNTP_CTL_EL0 :   0x%08x\n",
               (a53_u64)mp4_get_cpu(), "arm_timer_init",
               (a53_u32)val);

    __asm__ volatile("mrs %0, cntpct_el0" : "=r"(val));
    printf_low("%d:%s:CNTPCT_EL0 :     0x%016lx\n",
               (a53_u64)mp4_get_cpu(), "arm_timer_init", val);
    return 0;
}
