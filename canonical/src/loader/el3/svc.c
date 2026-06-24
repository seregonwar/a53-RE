#include "a53_abi.h"
#include "a53_context.h"

int A53_SECTION(".text.el3.loader") svc_EL3(a53_u32 esr_el1,
                                            mp4_debug_status_t *status)
{
    if ((esr_el1 & 0xffff) == 0x152) {
        int ret;

        ret = write_EL3((char *)status->mds_gpr[0], status->mds_gpr[1]);
        status->mds_gpr[0] = (a53_s64)ret;
        return 0;
    }
    printf_low("%d:%s:Unsupport imm16\n", (a53_u64)mp4_get_cpu(), "svc_EL3");
    aarch64_print_ESR_EL1();
    return -1;
}
