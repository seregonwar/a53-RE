#include "a53_abi.h"
#include "a53_context.h"

a53_u64 A53_SECTION(".text.el3.loader") c2pmsg_addr64(a53_u32 ui)
{
    if (ui < 4) {
        return (a53_u64)(ui * 4 + 0x3010500UL);
    }
    if (ui < 0x14) {
        return (a53_u64)(ui * 0x1000 + 0x30ec000UL);
    }
    printf_low("%d:%s:Illegal C2MSG no %u\n",
               (a53_u64)mp4_get_cpu(), "c2pmsg_addr32", (a53_u64)ui);
    return 0x3010500UL;
}

a53_u32 A53_SECTION(".text.el3.loader") c2pmsg_read(a53_u32 no)
{
    return *(volatile a53_u32 *)c2pmsg_addr64(no);
}

void A53_SECTION(".text.el3.loader") c2pmsg_write(a53_u32 no, a53_u32 data)
{
    *(volatile a53_u32 *)c2pmsg_addr64(no) = data;
}

void A53_SECTION(".text.el3.loader") c2pmsg_init(void)
{
    a53_u32 ui;

    for (ui = 0; ui != 6; ++ui) {
        c2pmsg_write(ui, 0);
    }
}
