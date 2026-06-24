#include "a53_context.h"

/* Function pointer fields in deci_target_md_t */
#define MD_SETUP(md) do { \
    (md)->dtmd_name = "MP4"; \
    (md)->dtmd_shm_node_target_magic1 = DECI_SHM_NODE_MAGIC1_MP4; \
    (md)->dtmd_shm_ch_node_fix_cp_to_target_magic = 0xcb9b4abaU; \
    (md)->dtmd_shm_ch_node_fix_target_to_cp_magic = 0xcb9b4abaU; \
    (md)->dtmd_get_shm_common = deci_target_mp4_get_shm_common; \
    (md)->dtmd_get_shm_node_target = deci_target_mp4_get_shm_node_target; \
    (md)->dtmd_get_shm_ch_fix_cp_to_target = deci_target_mp4_get_shm_ch_fix_cp_to_target; \
    (md)->dtmd_get_shm_ch_fix_target_to_cp = deci_target_mp4_get_shm_ch_fix_target_to_cp; \
    (md)->dtmd_get_shm_ch_ring_cp_to_target = deci_target_mp4_get_shm_ch_ring_cp_to_target; \
    (md)->dtmd_get_shm_ch_ring_target_to_cp = deci_target_mp4_get_shm_ch_ring_target_to_cp; \
    (md)->dtmd_int_to_cp = deci_target_mp4_int_to_cp; \
    (md)->dtmd_wait_clear_target_to_cp = deci_target_mp4_wait_clear_target_to_cp; \
    (md)->dtmd_clear_int_from_cp = deci_target_mp4_clear_int_from_cp; \
} while (0)


int A53_SECTION(".text.el3.loader")
deci_target_ch_fix_send_request(deci_target_ch_fix_t *dtcf,
                                 a53_u32 psize, a53_u32 hint)
{
    a53_u32 mbox;

    dtcf->dtcf_write_count = dtcf->dtcf_write_count + 1;
    dtcf->dtcf_total_write_size = dtcf->dtcf_total_write_size + (a53_u64)psize;
    *dtcf->dtcf_t2c_cmd_status_ptr =
        (*dtcf->dtcf_t2c_cmd_status_ptr & 0xfff00000U) | 0x20U;
    *dtcf->dtcf_t2c_cmd_data_size_ptr = psize;
    mbox = deci_shm_make_mbox0(0x2000U, dtcf->dtcf_t2c_cmd_bid);
    deci_target_ch_fix_write_mbox(dtcf, mbox, hint);
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci_target_ch_fix_write_mbox(deci_target_ch_fix_t *dtcf,
                               a53_u32 mbox, a53_u32 hint)
{
    a53_u32 sig_bit;
    deci_target_md_t *pmd;

    sig_bit = dtcf->dtcf_sig_bit_t2c;
    pmd = deci_target_get_md();
    (pmd->dtmd_wait_clear_target_to_cp)(dtcf->dtcf_sig_no_t2c,
                                         dtcf->dtcf_sig_dst_t2c, sig_bit);
    pmd = deci_target_get_md();
    (pmd->dtmd_int_to_cp)(dtcf->dtcf_sig_no_t2c, dtcf->dtcf_sig_dst_t2c,
                           sig_bit, dtcf->dtcf_mbox_t2c, mbox, hint);
    pmd = deci_target_get_md();
    return (pmd->dtmd_wait_clear_target_to_cp)(dtcf->dtcf_sig_no_t2c,
                                                dtcf->dtcf_sig_dst_t2c, sig_bit);
}

int A53_SECTION(".text.el3.loader")
deci_target_ch_fix_send_reply(deci_target_ch_fix_t *dtcf, deci5s_context_t *dc)
{
    a53_u32 mbox;

    *dtcf->dtcf_c2t_cmd_status_ptr =
        (*dtcf->dtcf_c2t_cmd_status_ptr & 0xfff00000U) | 0x80000U;
    if ((dc->dc_result & 1) == 0) {
        mbox = deci_shm_make_mbox0(0x1000U, dtcf->dtcf_c2t_cmd_bid);
    } else {
        a53_u32 *puVar2;
        a53_u32 uVar1;

        puVar2 = dtcf->dtcf_c2t_res_data_size_ptr;
        uVar1 = *dtcf->dtcf_c2t_res_status_ptr;
        dtcf->dtcf_write_count = dtcf->dtcf_write_count + 1;
        dtcf->dtcf_total_write_size =
            dtcf->dtcf_total_write_size + (a53_u64)dc->dc_res_data_size;
        *dtcf->dtcf_c2t_res_status_ptr =
            (uVar1 & 0xfff00000U) | 0x20U;
        *puVar2 = dc->dc_res_data_size;
        mbox = deci_shm_make_mbox01(0x1000U, dtcf->dtcf_c2t_cmd_bid,
                                     0x4000U, dtcf->dtcf_c2t_res_bid);
    }
    deci_target_ch_fix_write_mbox(dtcf, mbox, 0);
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci_target_ch_fix_handle_irq_poll(deci_target_ch_fix_t *dtcf)
{
    deci_target_ch_fix_handle_intr(dtcf);
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci_target_ch_fix_handle_intr(deci_target_ch_fix_t *dtcf)
{
    a53_u32 uVar1;
    a53_u16 op;
    a53_u16 op_00;

    uVar1 = *dtcf->dtcf_mbox_c2t;
    deci_mp4_sig3_clear_int_from_emc(dtcf->dtcf_sig_bit_c2t);
    dtcf->dtcf_intr_count = dtcf->dtcf_intr_count + 1;

    if (uVar1 == 0x77777777U) {
        /* DECI_SHM_MBOX_CLOSE */
        printf_low("%d:%s:DECI_SHM_MBOX_CLOSE\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci_target_ch_fix_handle_mbox_intr");
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                   "deci_target_ch_fix_make_close", 0x27d, 1, "dtcf != NULL");
        if (dtcf->dtcf_d5cf == (deci5s_ch_fix_t *)0) {
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                       "deci_target_ch_fix_make_close", 0x27f, 0, "0");
            return 0;
        }
        dtcf->dtcf_d5cf->d5cf_status =
            dtcf->dtcf_d5cf->d5cf_status & 0xfffffffdU;
        return 0;
    }

    if (uVar1 == 0xddddddddU) {
        /* DECI_SHM_MBOX_OPEN */
        printf_low("%d:%s:DECI_SHM_MBOX_OPEN\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci_target_ch_fix_handle_mbox_intr");
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                   "deci_target_ch_fix_make_open", 0x26f, 1, "dtcf != NULL");
        if (dtcf->dtcf_d5cf == (deci5s_ch_fix_t *)0) {
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                       "deci_target_ch_fix_make_open", 0x271, 0, "0");
            return 0;
        }
        dtcf->dtcf_d5cf->d5cf_status =
            dtcf->dtcf_d5cf->d5cf_status | 2U;
        return 0;
    }

    op = deci_shm_mbox_get_op0(uVar1);
    op_00 = deci_shm_mbox_get_op1(uVar1);
    deci_target_ch_fix_handle_op_intr(dtcf, op);
    return deci_target_ch_fix_handle_op_intr(dtcf, op_00);
}

int A53_SECTION(".text.el3.loader")
deci_target_mp4_intr_with_cpu(a53_u32 cpu, a53_u32 bits)
{
    deci_target_t *dts;
    deci_target_ch_fix_t *dtcf;
    a53_u64 uVar3;

    dts = g_deci_target;
    uVar3 = (a53_u64)bits;
    while (uVar3 != 0) {
        if ((uVar3 >> 8 & 1) == 0) {
            if ((uVar3 >> 9 & 1) == 0) {
                printf_low("%s:Unsupport SIG bits 0x%08x\n",
                           "deci_target_mp4_intr_with_cpu",
                           (a53_u32)uVar3);
                uVar3 = 0;
            } else {
                dtcf = deci_target_get_ch_fix(dts, 1);
                deci_target_ch_fix_handle_intr(dtcf);
                uVar3 = uVar3 & 0xfffffdffULL;
            }
        } else {
            dtcf = deci_target_get_ch_fix(dts, 0);
            deci_target_ch_fix_handle_intr(dtcf);
            uVar3 = uVar3 & 0xfffffeffULL;
        }
    }
    return 0;
}

deci_target_t *A53_SECTION(".text.el3.loader") deci_target_get(void)
{
    return g_deci_target;
}

deci_target_ch_fix_t *A53_SECTION(".text.el3.loader")
deci_target_get_ch_fix(deci_target_t *dts, a53_u32 no)
{
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
               "deci_target_get_ch_fix", 0x4d6,
               (int)(dts != (deci_target_t *)0),
               "dts != NULL");
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
               "deci_target_get_ch_fix", 0x4d7,
               (int)(dts->dts_ch_fix_vec != (deci_target_ch_fix_t **)0),
               "dts->dts_ch_fix_vec != NULL");
    return dts->dts_ch_fix_vec[no];
}

int A53_SECTION(".text.el3.loader") deci_target_mp4_intr(a53_u32 bits)
{
    deci_target_t *dts;
    deci_target_ch_fix_t *dtcf;
    a53_u64 uVar3;

    dts = g_deci_target;
    uVar3 = (a53_u64)bits;
    while (uVar3 != 0) {
        if ((uVar3 >> 8 & 1) == 0) {
            if ((uVar3 >> 9 & 1) == 0) {
                printf_low("%s:Unsupport SIG bits 0x%08x\n",
                           "deci_target_mp4_intr",
                           (a53_u32)uVar3);
                uVar3 = 0;
            } else {
                dtcf = deci_target_get_ch_fix(dts, 1);
                deci_target_ch_fix_handle_intr(dtcf);
                uVar3 = uVar3 & 0xfffffdffULL;
            }
        } else {
            dtcf = deci_target_get_ch_fix(dts, 0);
            deci_target_ch_fix_handle_intr(dtcf);
            uVar3 = uVar3 & 0xfffffeffULL;
        }
    }
    return 0;
}

int A53_SECTION(".text.el3.loader") deci_target_mp4_poll(void)
{
    deci_target_t *dts;
    deci_target_ch_fix_t *dtcf;
    deci_target_ch_fix_t *dtcf_00;
    a53_u32 uVar2;
    int bVar1;

    dts = g_deci_target;
    dtcf = deci_target_get_ch_fix(dts, 0);
    dtcf_00 = deci_target_get_ch_fix(dts, 1);
    bVar1 = 0;
    while (!bVar1) {
        uVar2 = deci_mp4_sig3_read_int_from_emc();
        if ((dtcf->dtcf_sig_bit_c2t & uVar2) != 0) {
            deci_target_ch_fix_handle_intr(dtcf);
            bVar1 = 1;
        }
        if ((dtcf_00->dtcf_sig_bit_c2t & uVar2) != 0) {
            deci_target_ch_fix_handle_intr(dtcf_00);
            bVar1 = 1;
        }
    }
    return 0;
}

int A53_SECTION(".text.el3.loader") deci_target_start(void)
{
    deci_target_md_t *pmd;
    deci_shm_common_t *dsc;
    deci_shm_node_t *dsn_cp;
    deci_shm_node_t *dsn_target;
    deci_target_ch_fix_t *dtcf;
    a53_u32 i;
    a53_u32 n_ch_fix;

    printf_low("%d:%s:()\n", (a53_u64)mp4_get_cpu(), "deci_target_start");

    if (g_deci_target == (deci_target_t *)0) {
        g_deci_target = &g_deci_target_data;
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                   "deci_target_init", 0x4a6, 1, "dts != NULL");

        g_deci_target_data.dts_n_ch_fix = 0;
        g_deci_target_data.dts_n_ch_ring = 0;
        g_deci_target_data.dts_ch_fix_vec = (deci_target_ch_fix_t **)0;
        g_deci_target_data.dts_ch_ring_vec = (deci_target_ch_ring_t **)0;

        deci_target_get_md();
        pmd = g_deci_target_data.dts_md;
        dsc = pmd->dtmd_get_shm_common();
        dsn_cp = deci_shm_common_get_node_cp(dsc);
        dsn_target = pmd->dtmd_get_shm_node_target(dsc);

        n_ch_fix = dsn_target->dsn_n_ch_fix;
        g_deci_target_data.dts_n_ch_fix = n_ch_fix;
        g_deci_target_data.dts_n_ch_ring = dsn_target->dsn_n_ch_ring;

        dtcf = g_vec_deci_target_ch_fix;
        for (i = 0; i < n_ch_fix; ++i) {
            deci_shm_ch_node_fix_t *dscnf_cp;
            deci_shm_ch_node_fix_t *dscnf_target;
            deci_shm_buf_t *buf;
            a53_u8 *puVar8;

            dscnf_cp = pmd->dtmd_get_shm_ch_fix_cp_to_target(dsc, i);
            dscnf_target = pmd->dtmd_get_shm_ch_fix_target_to_cp(dsc, i);

            if (dsc->dsc_magic1 != 0x63267916U) {
                printf_low("%s:illegal dsc_magic1 0x%08x\n",
                           "deci_target_ch_fix_init");
            }
            if (dsn_cp->dsn_magic1 != DECI_SHM_NODE_MAGIC1_CP) {
                printf_low("%s:illegal dsn_cp->dsn_magic1 0x%08x\n",
                           "deci_target_ch_fix_init");
            }

            {
                deci_target_md_t *pmd2;

                pmd2 = deci_target_get_md();
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                           "deci_target_ch_fix_init", 0x9e,
                           deci_shm_ch_node_fix_check_with_magic(
                               dscnf_cp, pmd2->dtmd_shm_ch_node_fix_cp_to_target_magic),
                           "deci_shm_ch_node_fix_check_with_magic(dscnf_cp, "
                           "deci_target_get_md()->dtmd_shm_ch_node_fix_cp_to_target_magic)");

                pmd2 = deci_target_get_md();
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                           "deci_target_ch_fix_init", 0x9f,
                           deci_shm_node_check_with_magic(
                               dsn_target, pmd2->dtmd_shm_node_target_magic1),
                           "deci_shm_node_check_with_magic(dsn_target, "
                           "deci_target_get_md()->dtmd_shm_node_target_magic1)");

                pmd2 = deci_target_get_md();
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                           "deci_target_ch_fix_init", 0xa1,
                           deci_shm_ch_node_fix_check_with_magic(
                               dscnf_target, pmd2->dtmd_shm_ch_node_fix_target_to_cp_magic),
                           "deci_shm_ch_node_fix_check_with_magic(dscnf_target, "
                           "deci_target_get_md()->dtmd_shm_ch_node_fix_target_to_cp_magic)");
            }

            dtcf->dtcf_self_size = 0x128U;
            dtcf->dtcf_id = i;
            dtcf->dtcf_magic1 = 0x11609934U;
            dtcf->dtcf_intr_count = 0;
            dtcf->dtcf_mbox_req_count = 0;
            dtcf->dtcf_mbox_free_count = 0;
            dtcf->dtcf_mbox_nop_count = 0;
            dtcf->dtcf_read_count = 0;
            dtcf->dtcf_write_count = 0;
            dtcf->dtcf_total_read_size = 0;
            dtcf->dtcf_total_write_size = 0;
            dtcf->dtcf_ch_fix_c2t = dscnf_cp;
            dtcf->dtcf_ch_fix_t2c = dscnf_target;

            dtcf->dtcf_mbox_c2t = &dscnf_cp->dscnf_mbox.dsm_mbox;
            dtcf->dtcf_sig_no_c2t = dscnf_cp->dscnf_mbox.dsm_sig_no;
            dtcf->dtcf_sig_dst_c2t = dscnf_cp->dscnf_mbox.dsm_sig_dst;
            dtcf->dtcf_sig_bit_c2t = dscnf_cp->dscnf_mbox.dsm_sig_bit;

            dtcf->dtcf_mbox_t2c = &dscnf_target->dscnf_mbox.dsm_mbox;
            dtcf->dtcf_sig_no_t2c = dscnf_target->dscnf_mbox.dsm_sig_no;
            dtcf->dtcf_sig_dst_t2c = dscnf_target->dscnf_mbox.dsm_sig_dst;
            dtcf->dtcf_sig_bit_t2c = dscnf_target->dscnf_mbox.dsm_sig_bit;

            buf = (deci_shm_buf_t *)
                deci_shm_common_get_ptr(dsc, dscnf_cp->dscnf_buf_spec_offset_cmd);
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                       "deci_target_ch_fix_init", 0xca,
                       deci_shm_buf_check(buf),
                       "deci_shm_buf_check(buf)");
            dtcf->dtcf_c2t_cmd_bid = buf->dsb_id;
            puVar8 = deci_shm_common_get_ptr(dsc, buf->dsb_buf_offset);
            dtcf->dtcf_c2t_cmd_buf_ptr = puVar8;
            dtcf->dtcf_c2t_cmd_buf_size = buf->dsb_buf_size;
            dtcf->dtcf_c2t_cmd_status_ptr = &dscnf_cp->dscnf_buf_status_cmd;
            dtcf->dtcf_c2t_cmd_data_size_ptr = &dscnf_cp->dscnf_buf_data_size_cmd;

            buf = (deci_shm_buf_t *)
                deci_shm_common_get_ptr(dsc, dscnf_target->dscnf_buf_spec_offset_cmd);
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                       "deci_target_ch_fix_init", 0xd6,
                       deci_shm_buf_check(buf),
                       "deci_shm_buf_check(buf)");
            dtcf->dtcf_t2c_cmd_bid = buf->dsb_id;
            puVar8 = deci_shm_common_get_ptr(dsc, buf->dsb_buf_offset);
            dtcf->dtcf_t2c_cmd_buf_ptr = puVar8;
            dtcf->dtcf_t2c_cmd_buf_size = buf->dsb_buf_size;
            dtcf->dtcf_t2c_cmd_status_ptr = &dscnf_target->dscnf_buf_status_cmd;
            dtcf->dtcf_t2c_cmd_data_size_ptr = &dscnf_target->dscnf_buf_data_size_cmd;

            buf = (deci_shm_buf_t *)
                deci_shm_common_get_ptr(dsc, dscnf_target->dscnf_buf_spec_offset_res);
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                       "deci_target_ch_fix_init", 0xe2,
                       deci_shm_buf_check(buf),
                       "deci_shm_buf_check(buf)");
            dtcf->dtcf_c2t_res_bid = buf->dsb_id;
            puVar8 = deci_shm_common_get_ptr(dsc, buf->dsb_buf_offset);
            dtcf->dtcf_c2t_res_buf_ptr = puVar8;
            dtcf->dtcf_c2t_res_buf_size = buf->dsb_buf_size;
            dtcf->dtcf_c2t_res_status_ptr = &dscnf_target->dscnf_buf_status_res;
            dtcf->dtcf_c2t_res_data_size_ptr = &dscnf_target->dscnf_buf_data_size_res;

            buf = (deci_shm_buf_t *)
                deci_shm_common_get_ptr(dsc, dscnf_cp->dscnf_buf_spec_offset_res);
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                       "deci_target_ch_fix_init", 0xee,
                       deci_shm_buf_check(buf),
                       "deci_shm_buf_check(buf)");
            dtcf->dtcf_t2c_res_bid = buf->dsb_id;
            puVar8 = deci_shm_common_get_ptr(dsc, buf->dsb_buf_offset);
            dtcf->dtcf_t2c_res_buf_ptr = puVar8;
            dtcf->dtcf_t2c_res_buf_size = buf->dsb_buf_size;
            dtcf->dtcf_t2c_res_status_ptr = &dscnf_cp->dscnf_buf_status_res;
            dtcf->dtcf_t2c_res_data_size_ptr = &dscnf_cp->dscnf_buf_data_size_res;

            dtcf->dtcf_d5cf = (deci5s_ch_fix_t *)0;
            dtcf->dtcf_magic2 = 0x7d7b6e04U;

            g_vecp_deci_target_ch_fix[i] = dtcf;
            ++dtcf;
        }
        g_deci_target_data.dts_ch_fix_vec = g_vecp_deci_target_ch_fix;
    }
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci_target_up(deci_target_t *dts, a53_u32 core)
{
    deci_target_ch_fix_t *dtcf;
    int c;

    dtcf = deci_target_get_ch_fix(dts, core);
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
               "deci_target_up", 0x53d,
               (int)(dtcf != (deci_target_ch_fix_t *)0),
               "dtcf != NULL");

    if (dtcf->dtcf_self_size == 0x128U) {
        if (dtcf->dtcf_magic1 == 0x11609934U) {
            if (dtcf->dtcf_magic2 == 0x7d7b6e04U) {
                c = 1;
            } else {
                printf_low("%s:illegal magic2 0x%08x\n",
                           "deci_target_ch_fix_check",
                           (a53_u64)dtcf->dtcf_magic2);
                c = 0;
            }
        } else {
            printf_low("%s:illegal magic1 0x%08x\n",
                       "deci_target_ch_fix_check",
                       (a53_u64)dtcf->dtcf_magic1);
            c = 0;
        }
    } else {
        printf_low("%s:illegal self_size 0x%08x\n",
                   "deci_target_ch_fix_check",
                   (a53_u64)dtcf->dtcf_self_size);
        c = 0;
    }

    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
               "deci_target_up", 0x53e, c, "deci_target_ch_fix_check(dtcf)");
    deci_target_ch_fix_write_mbox(dtcf, 0xeeeeeeeeU, 0);
    return 0;
}

deci_target_md_t *A53_SECTION(".text.el3.loader")
deci_target_get_md_variable(deci_target_t *dts)
{
    g_deci_target_md.dtmd_dts = dts;
    return &g_deci_target_md;
}

/* ---- Thin wrappers that tail-call into deci_shm common helpers ----
 * In the reference binary these are single b (branch) instructions at
 * 0x0011010c-0x0011011c. With -Oz the compiler produces identical tail-calls.
 */

deci_shm_node_t *A53_SECTION(".text.el3.loader")
deci_target_mp4_get_shm_node_target(deci_shm_common_t *dsc)
{
    return deci_shm_common_get_node_mp4(dsc);
}

deci_shm_ch_node_fix_t *A53_SECTION(".text.el3.loader")
deci_target_mp4_get_shm_ch_fix_cp_to_target(deci_shm_common_t *dsc, a53_u32 ui)
{
    return deci_shm_common_get_ch_fix_cp_to_mp4(dsc, ui);
}

deci_shm_ch_node_fix_t *A53_SECTION(".text.el3.loader")
deci_target_mp4_get_shm_ch_fix_target_to_cp(deci_shm_common_t *dsc, a53_u32 ui)
{
    return deci_shm_common_get_ch_fix_mp4_to_cp(dsc, ui);
}

deci_shm_ch_node_ring_t *A53_SECTION(".text.el3.loader")
deci_target_mp4_get_shm_ch_ring_cp_to_target(deci_shm_common_t *dsc, a53_u32 ui)
{
    return deci_shm_common_get_ch_ring_cp_to_mp4(dsc, ui);
}

deci_shm_ch_node_ring_t *A53_SECTION(".text.el3.loader")
deci_target_mp4_get_shm_ch_ring_target_to_cp(deci_shm_common_t *dsc, a53_u32 ui)
{
    return deci_shm_common_get_ch_ring_mp4_to_cp(dsc, ui);
}

deci_target_ch_fix_t *A53_SECTION(".text.el3.loader")
deci_target_mp4_get_ch_fix(a53_u32 id)
{
    return deci_target_get_ch_fix(g_deci_target, id);
}

int A53_SECTION(".text.el3.loader") deci_target_mp4_start(a53_u32 core)
{
    if (core == 0) {
        deci_sig_mp4_start();
        deci_target_start();
    }
    return 0;
}

int A53_SECTION(".text.el3.loader") deci_target_mp4_up(a53_u32 core)
{
    deci_target_up(g_deci_target, core);
    return 0;
}

deci_target_md_t *A53_SECTION(".text.el3.loader") deci_target_get_md(void)
{
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
               "deci_target_get_md", 0x60,
               (int)(g_deci_target != (deci_target_t *)0),
               "g_deci_target != NULL");

    if (g_deci_target_data.dts_md == (deci_target_md_t *)0) {
        MD_SETUP(&g_deci_target_md);
        g_deci_target_data.dts_md = &g_deci_target_md;
    }
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
               "deci_target_get_md", 100,
               (int)(g_deci_target_data.dts_md != (deci_target_md_t *)0),
               "g_deci_target->dts_md != NULL");
    return g_deci_target_data.dts_md;
}

int A53_SECTION(".text.el3.loader")
deci_target_ch_fix_handle_op_intr(deci_target_ch_fix_t *dtcf, a53_u16 op)
{
    a53_u32 uVar1;

    uVar1 = (a53_u32)op;
    switch (((uVar1 & 0xf000U) + 0x1000U) >> 12) {
    case 0:
        dtcf->dtcf_mbox_nop_count = dtcf->dtcf_mbox_nop_count + 1;
        break;
    case 2: {
        a53_u32 bid;

        bid = uVar1 & 0xfffU;
        dtcf->dtcf_mbox_free_count = dtcf->dtcf_mbox_free_count + 1;
        if (bid != dtcf->dtcf_c2t_res_bid && bid != dtcf->dtcf_t2c_cmd_bid) {
            printf_low("%s:DECI_MBOX_FREE: bid=0x%08x != 0x%08x\n",
                       "deci_target_ch_fix_handle_op_intr",
                       (a53_u64)bid, (a53_u64)dtcf->dtcf_t2c_cmd_bid);
            printf_low("%s:DECI_MBOX_FREE: bid=0x%08x != 0x%08x\n",
                       "deci_target_ch_fix_handle_op_intr",
                       (a53_u64)bid, (a53_u64)dtcf->dtcf_c2t_res_bid);
        }
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                   "deci_target_ch_fix_handle_op_intr", 0x24a,
                   (int)(bid == dtcf->dtcf_c2t_res_bid
                         || bid == dtcf->dtcf_t2c_cmd_bid),
                   "(bid == dtcf->dtcf_c2t_res_bid) || (bid == dtcf->dtcf_t2c_cmd_bid)");
        *dtcf->dtcf_t2c_cmd_status_ptr =
            (*dtcf->dtcf_t2c_cmd_status_ptr & 0xfff00000U) | 2U;
        break;
    }
    case 3: {
        deci5s_context_t dStack;
        int ret;

        dtcf->dtcf_mbox_req_count = dtcf->dtcf_mbox_req_count + 1;
        deci5s_context_init(&dStack, 1, dtcf->dtcf_id);
        dStack.dc_cmd_ptr = dtcf->dtcf_c2t_cmd_buf_ptr;
        dStack.dc_cmd_size = *dtcf->dtcf_c2t_cmd_data_size_ptr;
        dStack.dc_res_ptr = dtcf->dtcf_c2t_res_buf_ptr;
        dStack.dc_res_max = dtcf->dtcf_c2t_res_buf_size;
        dStack.dc_res_data_size = 0;
        dtcf->dtcf_read_count = dtcf->dtcf_read_count + 1;
        dtcf->dtcf_total_read_size =
            dtcf->dtcf_total_read_size + (a53_u64)dStack.dc_cmd_size;
        ret = deci5s_context_handle_packet(&dStack);
        if (ret != 0) {
            printf_low("%d:%s:deci5s_context_handle_packet failed %d\n",
                       (a53_u64)mp4_get_cpu(),
                       "deci_target_ch_fix_handle_request_intr",
                       (a53_u64)ret);
        }
        return deci_target_ch_fix_send_reply(dtcf, &dStack);
    }
    case 5:
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                   "deci_target_ch_fix_handle_op_intr", 600, 0, "0");
        return 0;
    default:
        printf_low("%d:%s:Unknown op 0x%04x\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci_target_ch_fix_handle_op_intr",
                   (a53_u64)uVar1);
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci_target_mp4.c",
                   "deci_target_ch_fix_handle_op_intr", 0x262, 0, "0");
        return 0;
    }
    return 0;
}
