#include "a53_abi.h"
#include "a53_context.h"

extern deci_shm_mp4_t g_deci_shm_mp4_data;
extern deci_shm_mp4_t *g_deci_shm_mp4;

deci_shm_common_t *A53_SECTION(".text.el3.loader") deci_shm_mp4_common(void)
{
    if (g_deci_shm_mp4 != (deci_shm_mp4_t *)0) {
        return (deci_shm_common_t *)g_deci_shm_mp4_data.dsm4_shm_common;
    }
    printf_low("%d:%s:There is no g_deci_shm_mp4.\n",
               (a53_u64)mp4_get_cpu(), "deci_shm_mp4_common");
    return (deci_shm_common_t *)0;
}

int A53_SECTION(".text.el3.loader") deci_shm_mp4_start(a53_u32 core)
{
    deci_shm_common_t *dsc;
    deci_shm_node_t *node;

    if (core == 0) {
        g_deci_shm_mp4_data.dsm4_firm = (void *)0xd0000000ULL;
        g_deci_shm_mp4_data.dsm4_shm_common = (a53_u8 *)0xe0000000ULL;
        if (*(volatile a53_u32 *)0xd0000004ULL == 0x8fe36d30U
            && *(volatile a53_u32 *)0xd000001cULL == 0xcf8b42c3U) {
            g_deci_shm_mp4_data.dsm4_cp_param0 = *(volatile a53_u32 *)0xd0000010ULL;
            g_deci_shm_mp4_data.dsm4_cp_param2 = *(volatile a53_u32 *)0xd0000018ULL;
        } else {
            printf_low("%d:%s:NOT found firmware magic!!!",
                       (a53_u64)mp4_get_cpu(), "deci_shm_mp4_start");
        }
        dsc = (deci_shm_common_t *)g_deci_shm_mp4_data.dsm4_shm_common;
        if (deci_shm_common_check(dsc) < 1) {
            printf_low("%d:%s:deci_shm_common_check(%p) failed %d",
                       (a53_u64)mp4_get_cpu(), "deci_shm_mp4_start",
                       dsc,
                       (a53_u64)deci_shm_common_check(dsc));
            return -1;
        }
        node = deci_shm_common_get_node_mp4(dsc);
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm_mp4.c",
                   "deci_shm_mp4_start", 0x9a,
                   (int)(node->dsn_magic1 == 0x8e7e62d6U),
                   "node->dsn_magic1 == DECI_SHM_NODE_MAGIC1_MP4");
        g_deci_shm_mp4 = &g_deci_shm_mp4_data;
    }
    return 0;
}
