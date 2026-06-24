#include "a53_abi.h"
#include "a53_context.h"

#define DECI_SHM_NODE_MAGIC1_CP   0xc8f7e343U
#define DECI_SHM_NODE_MAGIC1_MP4  0x8e7e62d6U
#define DECI_SHM_NODE_MAGIC1_MP3  0x8fec3303U
#define DECI_SHM_NODE_MAGIC_MAIN   0x812c4d3dU
#define DECI_SHM_MAGIC_SYCORAX    0xfbdf45e4U
#define DECI_SHM_MAGIC_CP         0xf470b785U
#define DECI_SHM_MAGIC_MP4_OTHER  0x43fe7688U
#define DECI_SHM_MAGIC_SYNC       0x87661e49U
#define DECI_SHM_CH_NODE_FIX_MAGIC_CP_TO_MP4 0xcb9b4abaU
#define DECI_SHM_CH_NODE_RING_MAGIC_CP_TO_MP4 0x8e154c2aU

#define SELF_SIZE_NODE      0x20
#define SELF_SIZE_CH_FIX    0x60
#define SELF_SIZE_CH_RING   0x60
#define SELF_SIZE_BUF       0x20

a53_u16 A53_SECTION(".text.el3.loader") deci_shm_mbox_get_op0(a53_u32 mbox)
{
    return (a53_u16)(mbox >> 16);
}

a53_u16 A53_SECTION(".text.el3.loader") deci_shm_mbox_get_op1(a53_u32 mbox)
{
    return (a53_u16)mbox;
}

a53_u32 A53_SECTION(".text.el3.loader")
deci_shm_make_mbox0(a53_u16 type, a53_u32 bid)
{
    a53_u16 uVar1;

    uVar1 = deci_shm_make_mbox_16b(type, bid);
    return ((a53_u32)uVar1 << 16) | 0xffffU;
}

a53_u16 A53_SECTION(".text.el3.loader")
deci_shm_make_mbox_16b(a53_u16 type, a53_u32 bid)
{
    if ((bid & 0xffffU) > 0xfffU) {
        printf_low("%s:req=0x%04x, id=0x%04x\n",
                   "deci_shm_make_mbox_16b", (a53_u64)type);
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                   "deci_shm_make_mbox_16b", 0x1b6, 0, "0");
    }
    return (a53_u16)(type | ((a53_u16)bid & 0xfffU));
}

a53_u32 A53_SECTION(".text.el3.loader")
deci_shm_make_mbox01(a53_u16 type0, a53_u32 bid0, a53_u16 type1, a53_u32 bid1)
{
    a53_u16 uVar1;
    a53_u16 uVar2;

    uVar1 = deci_shm_make_mbox_16b(type0, bid0);
    uVar2 = deci_shm_make_mbox_16b(type1, bid1);
    return ((a53_u32)uVar1 << 16) | (a53_u32)uVar2;
}

a53_u8 *A53_SECTION(".text.el3.loader")
deci_shm_common_get_ptr(deci_shm_common_t *dsc, a53_u32 off)
{
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_ptr", 0x3a0,
               deci_shm_common_check(dsc),
               "deci_shm_common_check(dsc)");
    if ((off >> 21) > 0x7cU) {
        printf_low("%s:Illegal offset 0x%08x\n",
                   "deci_shm_common_get_ptr", (a53_u64)off);
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                   "deci_shm_common_get_ptr", 0x3a3, 0, "0");
    }
    return (a53_u8 *)((a53_u64)&dsc->dsc_self_size + (a53_u64)off);
}

int A53_SECTION(".text.el3.loader") deci_shm_common_check(deci_shm_common_t *dsc)
{
    return deci_shm_common_get_version(dsc) > 0;
}

deci_shm_node_t *A53_SECTION(".text.el3.loader")
deci_shm_common_v2_get_node_cp(deci_shm_common_v2_t *dsc2)
{
    deci_shm_common_target_t *pdVar1;

    pdVar1 = deci_shm_common_v2_find_target(dsc2, DECI_SHM_MAGIC_CP);
    if (pdVar1 == (deci_shm_common_target_t *)0) {
        printf_low("%s:deci_shm_common_v2_find_target() failed\n",
                   "deci_shm_common_v2_get_node_cp");
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                   "deci_shm_common_v2_get_node_cp", 0x64a, 0, "0");
        return (deci_shm_node_t *)0;
    }
    return (deci_shm_node_t *)
        deci_shm_common_get_ptr((deci_shm_common_t *)dsc2,
                                pdVar1->dsct_node_offset);
}

deci_shm_common_target_t *A53_SECTION(".text.el3.loader")
deci_shm_common_v2_find_target(deci_shm_common_v2_t *dsc2, a53_u32 magic)
{
    deci_shm_common_target_t *pdVar1;
    a53_u32 uVar2;

    uVar2 = 0;
    pdVar1 = (deci_shm_common_target_t *)
        ((a53_u64)&dsc2[1].dsc2_self_size + (a53_u64)dsc2->dsc2_n_buf_size);
    for (;;) {
        if (dsc2->dsc2_n_node <= uVar2) {
            printf_low("%s:Cannot find magic = 0x%08x\n",
                       "deci_shm_common_v2_find_target", (a53_u64)magic);
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                       "deci_shm_common_v2_find_target", 0x591, 0, "0");
            return (deci_shm_common_target_t *)0;
        }
        if (pdVar1->dsct_magic == magic) {
            break;
        }
        ++pdVar1;
        ++uVar2;
    }
    return pdVar1;
}

deci_shm_node_t *A53_SECTION(".text.el3.loader")
deci_shm_common_v2_get_node_main(deci_shm_common_v2_t *dsc2)
{
    deci_shm_common_target_t *pdVar1;

    pdVar1 = deci_shm_common_v2_find_target(dsc2, DECI_SHM_MAGIC_MAIN);
    if (pdVar1 == (deci_shm_common_target_t *)0) {
        printf_low("%s:deci_shm_common_v2_find_target() failed\n",
                   "deci_shm_common_v2_get_node_main");
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                   "deci_shm_common_v2_get_node_main", 0x658, 0, "0");
        return (deci_shm_node_t *)0;
    }
    return (deci_shm_node_t *)
        deci_shm_common_get_ptr((deci_shm_common_t *)dsc2,
                                pdVar1->dsct_node_offset);
}

deci_shm_node_t *A53_SECTION(".text.el3.loader")
deci_shm_common_v2_get_node_sycorax(deci_shm_common_v2_t *dsc2)
{
    deci_shm_common_target_t *pdVar1;

    pdVar1 = deci_shm_common_v2_find_target(dsc2, DECI_SHM_MAGIC_SYCORAX);
    if (pdVar1 == (deci_shm_common_target_t *)0) {
        printf_low("%s:deci_shm_common_v2_find_target() failed\n",
                   "deci_shm_common_v2_get_node_sycorax");
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                   "deci_shm_common_v2_get_node_sycorax", 0x666, 0, "0");
        return (deci_shm_node_t *)0;
    }
    return (deci_shm_node_t *)
        deci_shm_common_get_ptr((deci_shm_common_t *)dsc2,
                                pdVar1->dsct_node_offset);
}

int A53_SECTION(".text.el3.loader") deci_shm_common_get_version(deci_shm_common_t *dsc)
{
    if (dsc->dsc_magic1 != 0x63267916U) {
        printf_low("%s:%p: dsc_magic1 = 0x%08x != 0x%08x\n",
                   "deci_shm_common_get_version", dsc);
        return 0;
    }
    if (dsc[6].dsc_offset == 0x58d64767U) {
        return 1;
    }
    return (int)(dsc[3].dsc_magic1 == 0x158691aeU) << 1;
}

a53_u32 A53_SECTION(".text.el3.loader")
deci_shm_common_get_offset(deci_shm_common_t *dsc, a53_u8 *ptr)
{
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_offset", 0x708,
               deci_shm_common_check(dsc),
               "deci_shm_common_check(dsc)");
    return (a53_u32)((a53_u64)ptr - (a53_u64)dsc);
}

deci_shm_node_t *A53_SECTION(".text.el3.loader")
deci_shm_common_get_node_cp(deci_shm_common_t *dsc)
{
    return deci_shm_common_get_node(dsc, DECI_SHM_MAGIC_CP);
}

deci_shm_node_t *A53_SECTION(".text.el3.loader")
deci_shm_common_get_node(deci_shm_common_t *dsc, a53_u32 magic)
{
    a53_u32 off;
    int version;

    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_node", 0x7c9,
               deci_shm_common_check(dsc),
               "deci_shm_common_check(dsc)");
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_node_offset", 0x7af,
               deci_shm_common_check(dsc),
               "deci_shm_common_check(dsc)");

    version = deci_shm_common_get_version(dsc);
    if (version == 1) {
        if (magic == DECI_SHM_MAGIC_MAIN) {
            off = dsc[4].dsc_offset;
        } else if (magic == DECI_SHM_MAGIC_SYNC
                   || magic == DECI_SHM_MAGIC_MP3
                   || magic == DECI_SHM_MAGIC_MP4_OTHER) {
            off = 0;
        } else if (magic == DECI_SHM_MAGIC_SYCORAX) {
            off = dsc[4].dsc_magic1;
        } else if (magic == DECI_SHM_MAGIC_CP) {
            off = dsc[4].dsc_self_size;
        } else {
            printf_low("%s:Unknown magic 0x%08x\n",
                       "deci_shm_common_v1_get_node_offset", (a53_u64)magic);
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                       "deci_shm_common_v1_get_node_offset", 0x489, 0, "0");
            off = 0;
        }
    } else {
        deci_shm_common_target_t *pdVar3;

        pdVar3 = deci_shm_common_v2_find_target((deci_shm_common_v2_t *)dsc, magic);
        if (pdVar3 == (deci_shm_common_target_t *)0) {
            printf_low("%s:deci_shm_common_v2_find_target() failed\n",
                       "deci_shm_common_v2_get_node_offset");
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                       "deci_shm_common_v2_get_node_offset", 0x610, 0, "0");
            off = 0xffffffffU;
        } else {
            off = pdVar3->dsct_node_offset;
        }
    }
    return (deci_shm_node_t *)deci_shm_common_get_ptr(dsc, off);
}

deci_shm_node_t *A53_SECTION(".text.el3.loader")
deci_shm_common_get_node_main(deci_shm_common_t *dsc)
{
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_node_main", 0x7d9,
               deci_shm_common_check(dsc),
               "deci_shm_common_check(dsc)");
    if (deci_shm_common_get_version(dsc) == 1) {
        return (deci_shm_node_t *)deci_shm_common_get_ptr(dsc, dsc[4].dsc_offset);
    }
    return deci_shm_common_v2_get_node_main((deci_shm_common_v2_t *)dsc);
}

deci_shm_node_t *A53_SECTION(".text.el3.loader")
deci_shm_common_get_node_mp3(deci_shm_common_t *dsc)
{
    return deci_shm_common_get_node(dsc, DECI_SHM_MAGIC_MP3);
}

deci_shm_node_t *A53_SECTION(".text.el3.loader")
deci_shm_common_get_node_mp4(deci_shm_common_t *dsc)
{
    return deci_shm_common_get_node(dsc, DECI_SHM_MAGIC_MP4_OTHER);
}

deci_shm_ch_node_fix_t *A53_SECTION(".text.el3.loader")
deci_shm_common_get_ch_fix_cp_to_mp4(deci_shm_common_t *dsc, a53_u32 ui)
{
    deci_shm_node_t *pdVar4;
    a53_u8 *puVar5;
    a53_u64 uVar6;

    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_index_ch_fix_mp4", 0x730,
               deci_shm_common_check(dsc),
               "deci_shm_common_check(dsc)");

    if (deci_shm_common_get_version(dsc) == 1) {
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                   "deci_shm_common_get_index_ch_fix_mp4", 0x732, 0, "0");
        uVar6 = 0;
    } else {
        deci_shm_common_target_t *pdVar3;

        pdVar3 = deci_shm_common_v2_find_target((deci_shm_common_v2_t *)dsc,
                                                 DECI_SHM_MAGIC_MP4_OTHER);
        if (pdVar3 == (deci_shm_common_target_t *)0) {
            printf_low("%s:deci_shm_common_v2_find_target() failed\n",
                       "deci_shm_common_v2_get_base_ch_fix");
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                       "deci_shm_common_v2_get_base_ch_fix", 0x5be, 0, "0");
            uVar6 = 0;
        } else {
            uVar6 = (a53_u64)pdVar3->dsct_base_ch_fix;
        }
    }

    pdVar4 = deci_shm_common_get_node_cp(dsc);
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_ch_fix_cp_to_target", 0x96a,
               (int)(pdVar4->dsn_magic1 == DECI_SHM_NODE_MAGIC1_CP),
               "dsn->dsn_magic1 == DECI_SHM_NODE_MAGIC1_CP");

    puVar5 = deci_shm_common_get_ptr(dsc, pdVar4->dsn_ch_offset);
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_ch_fix_cp_to_mp4", 0x98e,
               (int)(((deci_shm_ch_node_fix_t *)
                      (puVar5 + (a53_u64)ui * 0x60 + uVar6 * 0x60))->dscnf_magic
                     == DECI_SHM_CH_NODE_FIX_MAGIC_CP_TO_MP4),
               "dscnf->dscnf_magic == DECI_SHM_CH_NODE_FIX_MAGIC_CP_TO_MP4");
    return (deci_shm_ch_node_fix_t *)
        (puVar5 + (a53_u64)ui * 0x60 + uVar6 * 0x60);
}

deci_shm_ch_node_fix_t *A53_SECTION(".text.el3.loader")
deci_shm_common_get_ch_fix_mp4_to_cp(deci_shm_common_t *dsc, a53_u32 ui)
{
    deci_shm_node_t *pdVar1;
    a53_u8 *puVar2;

    pdVar1 = deci_shm_common_get_node_mp4(dsc);
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_ch_fix_mp4_to_cp", 0x9cd,
               (int)(pdVar1->dsn_magic1 == DECI_SHM_NODE_MAGIC1_MP4),
               "dsn->dsn_magic1 == DECI_SHM_NODE_MAGIC1_MP4");
    puVar2 = deci_shm_common_get_ptr(dsc, pdVar1->dsn_ch_offset);
    return (deci_shm_ch_node_fix_t *)(puVar2 + (a53_u64)ui * 0x60);
}

deci_shm_ch_node_ring_t *A53_SECTION(".text.el3.loader")
deci_shm_common_get_ch_ring_cp_to_mp4(deci_shm_common_t *dsc, a53_u32 ui)
{
    deci_shm_node_t *pdVar5;
    a53_u8 *puVar6;
    a53_u64 uVar8;
    a53_u32 uVar1;

    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_index_ch_ring_mp4", 0x77d,
               deci_shm_common_check(dsc),
               "deci_shm_common_check(dsc)");

    if (deci_shm_common_get_version(dsc) == 1) {
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                   "deci_shm_common_get_index_ch_ring_mp4", 0x77f, 0, "0");
        uVar8 = 0;
    } else {
        deci_shm_common_target_t *pdVar4;
        a53_u32 uVar7;

        pdVar4 = deci_shm_common_v2_find_target((deci_shm_common_v2_t *)dsc,
                                                 DECI_SHM_MAGIC_MP4_OTHER);
        if (pdVar4 == (deci_shm_common_target_t *)0) {
            printf_low("%s:deci_shm_common_v2_find_target() failed\n",
                       "deci_shm_common_v2_get_base_ch_ring");
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
                       "deci_shm_common_v2_get_base_ch_ring", 0x5cd, 0, "0");
            uVar7 = 0;
        } else {
            uVar7 = pdVar4->dsct_base_ch_ring;
        }
        uVar8 = (a53_u64)(uVar7 - dsc[1].dsc_magic1);
    }

    pdVar5 = deci_shm_common_get_node_cp(dsc);
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_ch_ring_cp_to_target", 0x9ec,
               (int)(pdVar5->dsn_magic1 == DECI_SHM_NODE_MAGIC1_CP),
               "dsn->dsn_magic1 == DECI_SHM_NODE_MAGIC1_CP");

    puVar6 = deci_shm_common_get_ptr(dsc, pdVar5->dsn_ch_offset);
    uVar1 = pdVar5->dsn_n_ch_fix;

    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_ch_ring_cp_to_mp4", 0xa16,
               (int)(((deci_shm_ch_node_ring_t *)
                      (puVar6 + (a53_u64)ui * 0x60 + uVar8 * 0x60
                       + (a53_u64)uVar1 * 0x60))->dscnr_magic
                     == DECI_SHM_CH_NODE_RING_MAGIC_CP_TO_MP4),
               "dscnr->dscnr_magic == DECI_SHM_CH_NODE_RING_MAGIC_CP_TO_MP4");
    return (deci_shm_ch_node_ring_t *)
        (puVar6 + (a53_u64)ui * 0x60 + uVar8 * 0x60 + (a53_u64)uVar1 * 0x60);
}

deci_shm_ch_node_ring_t *A53_SECTION(".text.el3.loader")
deci_shm_common_get_ch_ring_mp4_to_cp(deci_shm_common_t *dsc, a53_u32 ui)
{
    deci_shm_node_t *pdVar2;
    a53_u8 *puVar4;
    a53_u32 uVar1;

    pdVar2 = deci_shm_common_get_node_mp4(dsc);
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_ch_ring_mp4_to_cp", 0xa59,
               (int)(pdVar2->dsn_magic1 == DECI_SHM_NODE_MAGIC1_MP4),
               "dsn->dsn_magic1 == DECI_SHM_NODE_MAGIC1_MP4");

    puVar4 = deci_shm_common_get_ptr(dsc, pdVar2->dsn_ch_offset);
    uVar1 = pdVar2->dsn_n_ch_fix;

    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_shm.c",
               "deci_shm_common_get_ch_ring_target_to_cp", 0xa39,
               deci_shm_ch_node_ring_check(
                   (deci_shm_ch_node_ring_t *)(puVar4 + (a53_u64)uVar1 * 0x60)),
               "deci_shm_ch_node_ring_check(dscnr)");
    return (deci_shm_ch_node_ring_t *)
        (puVar4 + (a53_u64)uVar1 * 0x60) + ui;
}

int A53_SECTION(".text.el3.loader") deci_shm_buf_check(deci_shm_buf_t *buf)
{
    if (buf->dsb_self_size == SELF_SIZE_BUF) {
        return (int)(buf->dsb_magic == 0x15b50c6U);
    }
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci_shm_node_check_with_magic(deci_shm_node_t *dsn, a53_u32 magic)
{
    if (dsn->dsn_self_size == SELF_SIZE_NODE) {
        if (dsn->dsn_magic1 == magic) {
            return 1;
        }
        printf_low("%d:%s:Illegal magic 0x%08x != 0x%08x [expect]\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci_shm_node_check_with_magic",
                   (a53_u64)dsn->dsn_magic1, (a53_u64)magic);
    } else {
        printf_low("%d:%s:Illegal self_size 0x%08x\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci_shm_node_check_with_magic",
                   (a53_u64)dsn->dsn_self_size);
    }
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci_shm_ch_node_fix_check_with_magic(deci_shm_ch_node_fix_t *dscnf, a53_u32 magic)
{
    if (dscnf->dscnf_self_size != SELF_SIZE_CH_FIX) {
        return 0;
    }
    if (dscnf->dscnf_magic == magic) {
        return 1;
    }
    printf_low("%s:Unknown magic 0x%08x != 0x%08x [expect]\n",
               "deci_shm_ch_node_fix_check_with_magic",
               (a53_u64)dscnf->dscnf_magic, (a53_u64)magic);
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci_shm_ch_node_ring_check(deci_shm_ch_node_ring_t *dscnr)
{
    a53_u32 uVar1;

    if (dscnr->dscnr_self_size != SELF_SIZE_CH_RING) {
        return 0;
    }
    uVar1 = dscnr->dscnr_magic;
    if (uVar1 == 0x89aabe53U || uVar1 == 0x8e154c2aU
        || uVar1 == 0xa97c5f30U || uVar1 == 0xb7baa60cU
        || uVar1 == 0xdde744feU || uVar1 == 0x2029fbdU
        || uVar1 == 0x129c4ef8U || uVar1 == 0x530e0397U
        || uVar1 == 0x610a3c53U || uVar1 == 0x6feb0d6aU) {
        return 1;
    }
    printf_low("%s:Unknown magic 0x%08x\n", "deci_shm_ch_node_ring_check");
    return 0;
}
