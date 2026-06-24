#include "a53_abi.h"

#define MP4_TIMER_BASE ((volatile a53_u32 *)0x03200400UL)
#define MP4_TIMER_STRIDE 0x24UL

a53_u32 A53_SECTION(".text.el3.loader") mp4_timer_get_cnt(a53_u32 id)
{
    volatile a53_u32 *timer;

    timer = (volatile a53_u32 *)((a53_u64)MP4_TIMER_BASE + (a53_u64)id * MP4_TIMER_STRIDE);
    return timer[8]; /* count register at offset 0x20 */
}

int A53_SECTION(".text.el3.loader") mp4_timer_init(void)
{
    a53_u64 off;

    for (off = 0; off < 0x90UL; off += MP4_TIMER_STRIDE) {
        volatile a53_u32 *timer = (volatile a53_u32 *)((a53_u64)MP4_TIMER_BASE + off);
        timer[0] = 0x100;   /* timer interval */
        timer[0] = 1;       /* enable */
    }
    return 0;
}
