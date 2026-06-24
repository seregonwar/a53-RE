#include "a53_abi.h"

#define DVM_MAILBOX_BASE ((volatile a53_u32 *)0x030c0000UL)
#define DVM_MAILBOX_STRIDE 0x1000UL

a53_u32 A53_SECTION(".text.el3.loader") dvm_read_mailbox(a53_u32 no)
{
    volatile a53_u32 *mbox;

    mbox = (volatile a53_u32 *)((a53_u64)DVM_MAILBOX_BASE + (a53_u64)no * DVM_MAILBOX_STRIDE);
    return *mbox;
}

int A53_SECTION(".text.el3.loader") dvm_init(void)
{
    a53_u64 off;

    for (off = 0; off < 0x30000UL; off += DVM_MAILBOX_STRIDE) {
        *(volatile a53_u32 *)((a53_u64)DVM_MAILBOX_BASE + off) = 0;
    }
    return 0;
}
