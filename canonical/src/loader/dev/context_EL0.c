#include "a53_context.h"

extern int svc_sttyp_write(char *buffer, a53_u64 length);
extern int svc_putchar(int c);
extern int putchar_el0_direct(int c);
extern int dev_pmu_init(void);
extern int dev_pmu_setup_default(void);

int A53_SECTION(".text.dev.loader") dev_context_init(
    dev_context_t *dc, sttyp_putchar_context_t *spc,
    a53_u32 cpu, a53_u32 cp_param2,
    char *buf, a53_u64 len, mp4_debug_status_t *mds)
{
    a53_u32 pericom_flag;
    a53_u32 pericom_addr;
    a53_u8 *pericom_base;

    dc->dc_dev_context_el0 = (dev_context_t *)0;
    dc->dc_dev_context_el1 = (dev_context_t *)0;
    dc->dc_dev_context_el2 = (dev_context_t *)0;
    dc->dc_exception_nest = 0;
    dc->dc_putchar_low_hook = svc_putchar;
    dc->dc_sttyp_putchar_context = spc;

    pericom_flag = 0x2000000U;
    if (cpu != 0) {
        pericom_flag = 0x20000U;
    }
    spc->spc_cpu = cpu;
    spc->spc_buf = buf;
    spc->spc_size = len;
    spc->spc_count = 0;
    spc->spc_mds_el0 = mds;
    pericom_addr = 0x3b01000U;
    if (cpu != 0) {
        pericom_addr = 0x3d01200U;
    }
    pericom_base = (a53_u8 *)0;
    if ((pericom_flag & cp_param2) != 0) {
        pericom_base = (a53_u8 *)(a53_u64)pericom_addr;
    }
    spc->spc_pericom = pericom_base;
    spc->spc_begin = spc_begin;
    spc->spc_putchar = spc_putchar;
    spc->spc_end = spc_end;
    dev_pmu_init();
    dev_pmu_setup_default();
    __asm__("msr pmcr_el0, %0" : : "r"(7UL));
    return 0;
}

int A53_SECTION(".text.dev.loader") spc_begin(sttyp_putchar_context_t *context)
{
    context->spc_count = 0;
    return 0;
}

int A53_SECTION(".text.dev.loader") spc_putchar(sttyp_putchar_context_t *context, int character)
{
    if (context->spc_count < context->spc_size) {
        context->spc_buf[context->spc_count] = (char)character;
        ++context->spc_count;
    }
    return character;
}

int A53_SECTION(".text.dev.loader") spc_end(sttyp_putchar_context_t *context)
{
    a53_u64 length = context->spc_count;

    if (length < context->spc_size) {
        context->spc_buf[length] = '\0';
    }
    aarch64_DC_CVAC_range_bs(context->spc_buf, length);
    svc_sttyp_write(context->spc_buf, context->spc_count);
    context->spc_count = 0;
    return (int)length;
}
