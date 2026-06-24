#include "a53_abi.h"
#include "a53_context.h"

#define CP_PARAM_BASE ((volatile a53_u32 *)0xd0000000UL)

a53_u32 g_param2;

int A53_SECTION(".text.el3.loader") cp_param_check(a53_u32 bit)
{
    return (int)(g_param2 & bit);
}

int A53_SECTION(".text.el3.loader") cp_param_init(void)
{
    volatile a53_u32 *cp = CP_PARAM_BASE;

    if (cp[0] == 0x20) {
        if (cp[1] == 0x8FE36D30UL) {
            g_param2 = cp[6];
            return 0;
        }
        printf_low("%d:%s:Invalid MAGIC1\n",
                   (a53_u64)mp4_get_cpu(), "cp_param_init");
    } else {
        printf_low("%d:%s:size mismatch\n",
                   (a53_u64)mp4_get_cpu(), "cp_param_init");
    }
    return -1;
}
