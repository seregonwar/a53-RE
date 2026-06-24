#include "a53_abi.h"
#include "a53_context.h"

/* Forward declarations for local helper */
extern int deci5s_context_handle_dcmp_packet(deci5s_context_t *dc);

extern deci5s_t g_d5s;
extern deci5s_ch_fix_t g_d5cf_core0;
extern deci5s_ch_fix_t g_d5cf_core1;
extern deci5s_sttyp_t g_d5s_sttyp;

void A53_SECTION(".text.el3.loader")
deci5s_assert(char *file, char *func, a53_u32 line, int c, char *cstr)
{
    if (c != 0) {
        return;
    }
    printf_low("Assertion failed at <%s:%s:%d:%s>\n", file, func, (a53_u64)line);
    for (;;) {
    }
}

char *A53_SECTION(".text.el3.loader") deci5s_basename(char *f)
{
    char *pcVar3;
    char *pcVar4;

    pcVar3 = (char *)0;
    pcVar4 = f;
    for (;;) {
        char *pcVar1;

        pcVar1 = pcVar4 + 1;
        if (*pcVar4 == '\0') {
            break;
        }
        pcVar4 = pcVar1;
        if ((*pcVar1 - 1 == '\\' || *pcVar1 - 1 == '/')
            && *pcVar1 != '\0') {
            pcVar3 = pcVar1;
        }
    }
    if (pcVar3 != (char *)0) {
        f = pcVar3;
    }
    return f;
}

a53_u32 A53_SECTION(".text.el3.loader") deci5s_get_cpu(void)
{
    a53_u64 v;

    __asm__("mrs %0, mpidr_el1" : "=r"(v));
    return (a53_u32)(v & 0xffU);
}

a53_u32 A53_SECTION(".text.el3.loader") deci5s_roundup64(a53_u32 orig)
{
    return (orig + 7) & 0xfffffff8U;
}

a53_u8 *A53_SECTION(".text.el3.loader")
deci5s_ch_fix_get_t2c_cmd_ptr(deci5s_ch_fix_t *d5cf)
{
    return d5cf->d5cf_low->dtcf_t2c_cmd_buf_ptr;
}

deci5s_sdbgp_command_spec_t *A53_SECTION(".text.el3.loader")
deci5s_sdbgp_com_spec_vector_find(deci5s_sdbgp_command_spec_t *vec, a53_u32 type)
{
    deci5s_sdbgp_command_spec_t *pdVar2;

    pdVar2 = vec - 1;
    do {
        ++pdVar2;
    } while (pdVar2->dscs_type != 0x1ffffffU && pdVar2->dscs_type != type);
    return pdVar2;
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_alloc_res_command(deci5s_sdbgp_context_t *dsc)
{
    return deci5s_sdbgp_context_alloc_res_command_common(
        dsc,
        dsc->dsc_cmd_command->self_size & 0xffffffU | 0x2000000U,
        dsc->dsc_cmd_spec->dscs_res_size);
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_alloc_res_command_common(deci5s_sdbgp_context_t *dsc,
                                               a53_u32 type, a53_u32 csize)
{
    SceDeci5sSdbgpCommand *pSVar5;
    SceDeci5sSdbgpHeader *pSVar6;
    SceDeci5sSdbgpHeader *pSVar7;
    a53_u8 *puVar1;
    deci5s_context_t *pdVar9;

    if (deci5s_context_check_overflow(dsc->dsc_dc, csize) == 0) {
        printf_low("%d:%s:deci5s_context_check_overflow() failed\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci5s_sdbgp_context_alloc_res_command_common");
        return -1;
    }

    pSVar5 = dsc->dsc_res_command;
    if (pSVar5 == (SceDeci5sSdbgpCommand *)0) {
        pSVar7 = dsc->dsc_res_sdbgp;
        pSVar6 = pSVar7 + 1;
    } else {
        pSVar6 = (SceDeci5sSdbgpHeader *)
            ((a53_u64)pSVar5 + (a53_u64)pSVar5->total_size);
        pSVar7 = dsc->dsc_res_sdbgp;
    }
    dsc->dsc_res_command = (SceDeci5sSdbgpCommand *)pSVar6;

    puVar1 = (a53_u8 *)((a53_u64)&pSVar6->self_size + (a53_u64)csize);
    pSVar7->n_command = pSVar7->n_command + 1;
    pSVar6->self_size = csize;
    pSVar6->total_size = pSVar6->self_size;
    pSVar6->sequence_no = type;
    pSVar6->packet_no = 0;
    pSVar6->attr = dsc->dsc_cmd_command->command_no;
    pSVar6->n_command = 0;

    pdVar9 = dsc->dsc_dc;
    pdVar9->dc_res_data_size = pdVar9->dc_res_data_size + csize;
    pSVar7->total_size = pSVar7->total_size + csize;

    dsc->dsc_res_info = puVar1;
    if (pdVar9->dc_res_ptr + pdVar9->dc_res_data_size == puVar1) {
        return 0;
    }
    printf_low("%s:mismatch [writep] %p and %p [res_info]\n",
               "deci5s_sdbgp_context_alloc_res_command_common");
    return -1;
}

void A53_SECTION(".text.el3.loader")
deci5s_context_init(deci5s_context_t *dc, a53_u32 mode, a53_u32 id)
{
    dc->dc_mode = mode;
    dc->dc_id = id;
    dc->dc_d5s = &g_d5s;
    if (mode == 1) {
        deci5s_ch_fix_t *pdVar1;

        pdVar1 = g_d5s.d5s_ch_fix[id];
        dc->dc_ch_ring = (deci5s_ch_ring_t *)0;
        dc->dc_ch_fix = pdVar1;
        dc->dc_cmd_ptr = pdVar1->d5cf_low->dtcf_c2t_cmd_buf_ptr;
        dc->dc_cmd_size = pdVar1->d5cf_low->dtcf_c2t_cmd_buf_size;
        dc->dc_res_ptr = pdVar1->d5cf_low->dtcf_c2t_res_buf_ptr;
        dc->dc_res_max = 0;
    } else {
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci5s_mp4.c",
                   "deci5s_context_init", 0x9e2, 0, "0");
    }
    dc->dc_result = 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_context_handle_packet(deci5s_context_t *dc)
{
    a53_s32 *piVar10;
    int protocol_id;

    piVar10 = (a53_s32 *)dc->dc_cmd_ptr;
    if (*piVar10 != 0x73354450) {
        printf_low("%s:There is no DECI5S signature.\n",
                   "deci5s_context_handle_packet");
        printf_low("%s:Enable SKIP mode.\n",
                   "deci5s_context_handle_packet");
        dc->dc_result = dc->dc_result | 2;
        return -1;
    }

    protocol_id = piVar10[5];
    if (protocol_id + 0xeffefe00U < 2) {
        protocol_id = 0x10;
    } else if (protocol_id + 0xdffffe00U < 2) {
        protocol_id = 0x20;
    } else if (protocol_id == 0x200ffff) {
        protocol_id = 2;
    } else {
        printf_low("%s:Unsupport protocol_id  0x%08x\n",
                   "deci_header_to_protocol_type_mp4");
        protocol_id = 0;
    }

    if (dc->dc_mode == 2) {
        if (protocol_id == 2) {
            return deci5s_context_handle_dcmp_packet(dc);
        }
        dc->dc_result = dc->dc_result | 2;
        return -1;
    }

    if (dc->dc_mode != 1) {
        printf_low("%s:unsupport mode 0x%08x\n",
                   "deci5s_context_handle_packet", (a53_u64)dc->dc_mode);
        return -1;
    }

    if (protocol_id == 0x20) {
        /* SDBGP protocol */
        deci5s_sdbgp_context_t local_ctx;
        SceDeci5sSdbgpCmd *cmd;
        a53_u32 uVar8;
        a53_u32 uVar12;
        a53_u32 uVar11;

        local_ctx.dsc_cmd = (SceDeci5sSdbgpCmd *)dc->dc_cmd_ptr;
        local_ctx.dsc_res = (SceDeci5sSdbgpRes *)dc->dc_res_ptr;
        local_ctx.dsc_cmd_sdbgp = &local_ctx.dsc_cmd->sdbgp;

        uVar8 = 0x40;
        local_ctx.dsc_res_command = (SceDeci5sSdbgpCommand *)0;
        local_ctx.dsc_res_info = (a53_u8 *)0;
        uVar12 = 0x18;
        local_ctx.dsc_cmd_spec = (deci5s_sdbgp_command_spec_t *)0;
        local_ctx.dsc_res_sdbgp = &local_ctx.dsc_res->sdbgp;
        dc->dc_res_data_size = 0x40;
        local_ctx.dsc_dc = dc;
        local_ctx.dsc_cmd_deci5s = (SceDeci5sHeader *)local_ctx.dsc_cmd;
        local_ctx.dsc_res_deci5s = (SceDeci5sHeader *)local_ctx.dsc_res;

        {
            SceDeci5sSdbgpCommand *pSVar9;

            pSVar9 = (SceDeci5sSdbgpCommand *)(local_ctx.dsc_cmd + 1);
            for (uVar11 = 0;
                 uVar11 < local_ctx.dsc_cmd_sdbgp->n_command;
                 ++uVar11) {
                deci5s_sdbgp_command_spec_t *pdVar7;
                deci5s_sdbgp_command_spec_t *pdVar3;

                local_ctx.dsc_cmd_command = pSVar9;

                pdVar3 = (deci5s_sdbgp_command_spec_t *)
                    &g_d5s.d5s_sdbgp_command_spec;
                pdVar7 = deci5s_sdbgp_com_spec_vector_find(
                    pdVar3, pSVar9->deci5s.packet_size);

                if (pdVar7->dscs_func == (void *)0) {
                    printf_low("%d:%s:TODO: Unsupport Error: 0x%08x\n",
                               (a53_u64)mp4_get_cpu(),
                               "deci5s_context_handle_sdbgp_packet",
                               (a53_u64)pSVar9->deci5s.packet_size);
                    if (deci5s_sdbgp_context_alloc_res_command_common(
                            &local_ctx, 0x2010001U, 0x20) == 0) {
                        local_ctx.dsc_res_command[1].self_size = 0x10;
                        local_ctx.dsc_res_command[1].total_size = 0;
                    }
                } else {
                    local_ctx.dsc_cmd_spec = pdVar7;
                    pdVar7->dscs_func(&local_ctx);
                }

                uVar8 = local_ctx.dsc_res_command->total_size + uVar8;
                uVar12 = local_ctx.dsc_res_command->total_size + uVar12;
                pSVar9 = (SceDeci5sSdbgpCommand *)
                    ((a53_u64)&local_ctx.dsc_cmd_command->self_size
                     + (a53_u64)local_ctx.dsc_cmd_command->self_size);
                local_ctx.dsc_res_command =
                    (SceDeci5sSdbgpCommand *)
                    ((a53_u64)&local_ctx.dsc_res_command->self_size
                     + (a53_u64)local_ctx.dsc_res_command->total_size);
            }
        }

        {
            a53_u32 uVar1;

            uVar1 = local_ctx.dsc_cmd_sdbgp->sequence_no;
            local_ctx.dsc_res_sdbgp->self_size = 0x18;
            local_ctx.dsc_res_sdbgp->total_size = uVar12;
            local_ctx.dsc_res_sdbgp->sequence_no = uVar1;
            local_ctx.dsc_res_sdbgp->packet_no = 0;
            local_ctx.dsc_res_sdbgp->attr = 1;
            local_ctx.dsc_res_sdbgp->n_command = uVar11;
            deci5s_header_init_p_cmd(local_ctx.dsc_res_deci5s, uVar8,
                                      local_ctx.dsc_cmd_deci5s);
        }
        dc->dc_res_data_size = uVar8;
        dc->dc_result = dc->dc_result | 1;
    } else if (protocol_id == 0x10) {
        /* STTYP protocol */
        SceDeci5sHeader *cmd;
        SceDeci5sHeader *p;
        a53_u32 uVar8;
        a53_u64 uVar6;

        cmd = (SceDeci5sHeader *)dc->dc_cmd_ptr;
        p = (SceDeci5sHeader *)dc->dc_res_ptr;
        strncpy((char *)&p[2].attr, (char *)&cmd[2].attr,
                (a53_u64)cmd[2].src);
        uVar8 = cmd[2].src + 0x68;
        deci5s_header_init_p_cmd(p, uVar8, cmd);

        p[1].dst = 0;
        p[1].protocol_id = 0;
        p[1].attr = 0;
        p[1].user_data = 0;
        p[1].timestamp = 0;
        p[1].signature = 0x40;
        p[1].self_size = 1;
        p[1].packet_size = 0;
        p[1].src = 0;
        p[2].signature = 0;
        p[2].self_size = cmd[2].self_size;
        p[2].packet_size = cmd[2].packet_size;
        p[2].src = cmd[2].src;
        uVar6 = deci5s_timestamp();
        p[2].dst = (a53_s32)uVar6;
        p[2].protocol_id = (a53_s32)(uVar6 >> 32);
        dc->dc_res_data_size = uVar8;
        dc->dc_result = dc->dc_result | 1;
    } else {
        if (protocol_id == 2) {
            return deci5s_context_handle_dcmp_packet(dc);
        }
        printf_low("%s:unsupport protocol type 0x%08x\n",
                   "deci5s_context_handle_packet");
        return -1;
    }

    *piVar10 = 0;
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_context_handle_dcmp_packet(deci5s_context_t *dc)
{
    a53_u8 *puVar5;

    puVar5 = dc->dc_cmd_ptr;
    switch (*(a53_u32 *)(puVar5 + 0x2c)) {
    case 4:
        /* LINK START */
        printf_low("%d:%s:(dc %p, cmd %p, dcmp %p)\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci5s_context_handle_dcmp_link_start",
                   dc, puVar5, puVar5 + 0x28);
        if (dc->dc_mode == 2) {
            dc->dc_ch_ring->d5cr_status =
                dc->dc_ch_ring->d5cr_status | 0x40U;
            return 0;
        }
        if (dc->dc_mode != 1) {
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci5s_mp4.c",
                       "deci5s_context_handle_dcmp_link_start", 0x2fe, 0, "0");
            return 0;
        }
        dc->dc_ch_fix->d5cf_status =
            dc->dc_ch_fix->d5cf_status | 0x40U;
        return 0;
    case 5:
        /* LINK STOP */
        dc->dc_ch_fix->d5cf_status =
            dc->dc_ch_fix->d5cf_status & 0xffffff0fU;
        return 0;
    case 8:
        /* PROTOCOL START */
        {
            SceDeci5sDcmpProtocolInfo *pSVar6;
            a53_u32 uVar4;

            pSVar6 = (SceDeci5sDcmpProtocolInfo *)(puVar5 + 0x38);
            for (uVar4 = 0; uVar4 < *(a53_u32 *)(puVar5 + 0x34); ++uVar4) {
                deci5s_dcmp_protocol_info_print(pSVar6);
                ++pSVar6;
            }
            if (dc->dc_mode == 2) {
                dc->dc_ch_ring->d5cr_status =
                    dc->dc_ch_ring->d5cr_status | 0x200U;
                return 0;
            }
            if (dc->dc_mode != 1) {
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci5s_mp4.c",
                           "deci5s_context_handle_dcmp_protocol_start", 0x32f, 0, "0");
                return 0;
            }
            dc->dc_ch_fix->d5cf_status =
                dc->dc_ch_fix->d5cf_status | 0x200U;
        }
        return 0;
    case 9:
        /* PROTOCOL STOP */
        {
            SceDeci5sDcmpProtocolInfo *pSVar6;
            a53_u32 uVar4;

            printf_low("%d:%s:self_size  0x%08x\n",
                       (a53_u64)mp4_get_cpu(),
                       "deci5s_context_handle_dcmp_protocol_stop",
                       (a53_u64)*(a53_u32 *)(puVar5 + 0x30));
            printf_low("%d:%s:count      0x%08x\n",
                       (a53_u64)mp4_get_cpu(),
                       "deci5s_context_handle_dcmp_protocol_stop",
                       (a53_u64)*(a53_u32 *)(puVar5 + 0x34));
            pSVar6 = (SceDeci5sDcmpProtocolInfo *)(puVar5 + 0x38);
            for (uVar4 = 0; uVar4 < *(a53_u32 *)(puVar5 + 0x34); ++uVar4) {
                deci5s_dcmp_protocol_info_print(pSVar6);
                ++pSVar6;
            }
            dc->dc_ch_fix->d5cf_status =
                dc->dc_ch_fix->d5cf_status & 0xfffff0ffU;
        }
        return 0;
    default:
        printf_low("%d:%s:UNKNOWN code = 0x%08x\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci5s_context_handle_dcmp_packet",
                   (a53_u64)*(a53_u32 *)(puVar5 + 0x2c));
        return -1;
    }
}

int A53_SECTION(".text.el3.loader") deci5s_mp4_start(a53_u32 core)
{
    deci5s_ch_fix_t *d5cf;

    deci_shm_mp4_start(core);
    deci_target_mp4_start(core);

    if (core == 0) {
        d5cf = &g_d5cf_core0;
        g_d5cf_core0.d5cf_low = deci_target_mp4_get_ch_fix(g_d5cf_core0.d5cf_id);
        g_d5cf_core0.d5cf_low->dtcf_d5cf = &g_d5cf_core0;
        g_d5s.d5s_ch_fix[0] = &g_d5cf_core0;
        g_d5s.d5s_ch_ring[0] = (deci5s_ch_ring_t *)0;
    } else {
        d5cf = &g_d5cf_core1;
        g_d5cf_core1.d5cf_low = deci_target_mp4_get_ch_fix(g_d5cf_core1.d5cf_id);
        g_d5cf_core1.d5cf_low->dtcf_d5cf = &g_d5cf_core1;
        g_d5s.d5s_ch_fix[1] = &g_d5cf_core1;
        g_d5s.d5s_ch_ring[1] = (deci5s_ch_ring_t *)0;
    }

    deci_target_mp4_up(core);

    printf_low("%d:%s:WAIT OPEN bit=0x%08x, 0x%08x\n",
               (a53_u64)mp4_get_cpu(), "deci5s_mp4_start",
               (a53_u64)d5cf->d5cf_low->dtcf_sig_bit_c2t,
               (a53_u64)d5cf->d5cf_sig_bit);
    while ((d5cf->d5cf_status >> 1 & 1) == 0) {
    }
    msi_send_command_sync(core, 0x10040000);

    /* LINK CONNECT */
    {
        SceDeci5sHeader *pSVar3;

        printf_low("%d:%s:=====================================================\n",
                   (a53_u64)mp4_get_cpu(), "deci5s_mp4_start");
        printf_low("%d:%s:LINK CONNECT\n",
                   (a53_u64)mp4_get_cpu(), "deci5s_mp4_start");

        pSVar3 = (SceDeci5sHeader *)d5cf->d5cf_low->dtcf_t2c_cmd_buf_ptr;
        bzero(pSVar3, 0x40);
        deci5s_header_init_psdp(pSVar3, 0x40, d5cf->d5cf_node,
                                 0x40ff0100U, 0x200ffffU);
        pSVar3[1].dst = 0;
        pSVar3[1].signature = 8;
        pSVar3[1].self_size = 2;
        pSVar3[1].packet_size = 0x10;
        pSVar3[1].src = 0;
        deci5s_ch_fix_send_request(d5cf, 0x40, 0);

        while ((d5cf->d5cf_status >> 6 & 1) == 0) {
        }
    }

    msi_send_command_sync(core, 0x10050000);

    /* PROTOCOL REGISTER */
    {
        SceDeci5sHeader *pSVar3;

        printf_low("%d:%s:=====================================================\n",
                   (a53_u64)mp4_get_cpu(), "deci5s_mp4_start");
        printf_low("%d:%s:PROTOCOL REGISTER\n",
                   (a53_u64)mp4_get_cpu(), "deci5s_mp4_start");
        printf_low("%d:%s:(d5cf %p[%d])\n",
                   (a53_u64)mp4_get_cpu(), "deci5s_ch_fix_send_dcmp_protocol",
                   d5cf, (a53_u64)d5cf->d5cf_id);

        pSVar3 = (SceDeci5sHeader *)d5cf->d5cf_low->dtcf_t2c_cmd_buf_ptr;
        bzero(pSVar3, 0x68);
        deci5s_header_init_psdp(pSVar3, 0x68, d5cf->d5cf_node,
                                 0x40ff0100U, 0x200ffffU);
        pSVar3[1].signature = 8;
        pSVar3[1].self_size = 6;
        pSVar3[1].packet_size = 8;
        pSVar3[1].src = 2;
        pSVar3[1].attr = 1;
        pSVar3[1].user_data = 1;
        pSVar3[1].timestamp = 1;
        pSVar3[2].packet_size = 1;
        pSVar3[2].src = 1;
        pSVar3[2].dst = 1;
        pSVar3[2].protocol_id = 0;
        pSVar3[1].dst = 0x18;
        pSVar3[1].protocol_id = d5cf->d5cf_id | 0x20000200U;
        pSVar3[2].signature = 0x18;
        pSVar3[2].self_size = d5cf->d5cf_id | 0x10010200U;
        deci5s_ch_fix_send_request(d5cf, 0x68, 0);
    }

    printf_low("%d:%s:SKIP WAITING PROTOCOL_START\n",
               (a53_u64)mp4_get_cpu(), "deci5s_mp4_start");
    msi_send_command_sync(core, 0x10060000);
    printf_low("%d:%s:<-0\n", (a53_u64)mp4_get_cpu(), "deci5s_mp4_start");
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_send_sttyp(char *msg, a53_u64 len)
{
    deci5s_ch_fix_t *d5cf;
    deci_target_ch_fix_t *pdVar10;
    a53_u64 *buf;
    a53_u64 uVar11;
    a53_u64 uVar7;
    a53_s64 lVar8;
    a53_s32 slen;

    printf_low("%d:%s:(msg %p, len 0x%08x)\n",
               (a53_u64)mp4_get_cpu(), "deci5s_send_sttyp",
               msg, (a53_u32)len);

    if (mp4_get_cpu() != 0) {
        d5cf = &g_d5cf_core1;
    } else {
        d5cf = &g_d5cf_core0;
    }
    pdVar10 = d5cf->d5cf_low;

    printf_low("%d:%s:[%c - %c - %c - %c] %d\n",
               (a53_u64)mp4_get_cpu(), "deci5s_send_sttyp",
               (a53_u64)(a53_u8)*msg,
               (a53_u64)(a53_u8)msg[1],
               (a53_u64)(a53_u8)msg[2],
               (a53_u64)(a53_u8)msg[3],
               (a53_u64)pdVar10->dtcf_id);

    uVar11 = 0;
    while ((*pdVar10->dtcf_t2c_cmd_status_ptr & 0xfffffU) != 2) {
        if (uVar11 > 0xf0000000U) {
            printf_low("%d:%s:TIMEOUT\n",
                       (a53_u64)mp4_get_cpu(), "deci5s_send_sttyp");
        }
        ++uVar11;
    }

    printf_low("%d:%s:->deci5s_ch_fix_send_sttyp()\n",
               (a53_u64)mp4_get_cpu(), "deci5s_send_sttyp");

    buf = (a53_u64 *)d5cf->d5cf_low->dtcf_t2c_cmd_buf_ptr;
    slen = (a53_s32)strnlen(msg, 0x80);
    uVar11 = (a53_u64)((a53_u32)(slen + 7) & 0xfffffff8U);
    uVar7 = uVar11 + 0x68;

    bzero(buf, uVar7);
    *buf = 0x2873354450ULL;
    buf[1] = uVar7;
    *(a53_u32 *)(buf + 2) = 0x20ffffffU;
    buf[3] = 0;
    *(a53_u32 *)((a53_u64)buf + 0xc) = d5cf->d5cf_id | 0x80ff0200U;
    *(a53_u32 *)((a53_u64)buf + 0x14) = d5cf->d5cf_id | 0x10010200U;
    buf[4] = deci5s_timestamp();
    buf[6] = 0;
    buf[7] = 0;
    buf[8] = 0;
    buf[9] = 0;
    *(a53_s32 *)(buf + 0xb) = slen;
    *(a53_u32 *)((a53_u64)buf + 0x5c) = (a53_u32)uVar11;
    *(a53_u32 *)(buf + 5) = 0x40;
    *(a53_u32 *)((a53_u64)buf + 0x2c) = d5cf->d5cf_id | 0x83020100U;
    *(a53_u32 *)(buf + 10) = 0;
    *(a53_u32 *)((a53_u64)buf + 0x54) = g_d5s_sttyp.d5ss_seq_no;
    buf[12] = deci5s_timestamp();

    for (lVar8 = 0; slen != (a53_s32)lVar8; ++lVar8) {
        *(a53_u8 *)((a53_u64)buf + lVar8 + 0x68) = msg[lVar8];
    }
    for (; (a53_u32)((a53_u64)len & 0xffffffffU) < (a53_u32)uVar11;
         uVar7 = uVar7 - 1) {
        /* Pad with zeros */
    }

    g_d5s_sttyp.d5ss_seq_no = g_d5s_sttyp.d5ss_seq_no + 1;
    deci5s_ch_fix_send_request(d5cf, (a53_u32)uVar7,
                                (a53_u32)(a53_u8)*msg);
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_mp4_panic_and_loop(a53_u32 cpu, a53_u64 pc)
{
    deci5s_ch_fix_t *d5cf;
    extern a53_u32 deci5s_mp4_loop_first;

    if (cpu != 0) {
        d5cf = &g_d5cf_core1;
    } else {
        d5cf = &g_d5cf_core0;
    }

    {
        SceDeci5sHeader *pSVar4;

        pSVar4 = (SceDeci5sHeader *)d5cf->d5cf_low->dtcf_t2c_cmd_buf_ptr;
        bzero(pSVar4, 0x78);
        deci5s_header_init_psdp(pSVar4, 0x78, d5cf->d5cf_node,
                                 0x20ff0100U, d5cf->d5cf_sdbgp);
        pSVar4[1].timestamp = 0x4021001U;
        pSVar4[2].signature = 0;
        pSVar4[2].self_size = 0;
        pSVar4[2].packet_size = 0x11111111U;
        pSVar4[2].src = 0;
        pSVar4[2].dst = 0;
        pSVar4[2].protocol_id = 0;
        pSVar4[2].attr = (a53_s32)pc;
        pSVar4[2].user_data = (a53_s32)(pc >> 32);
        pSVar4[2].timestamp = 0;
        pSVar4[1].signature = 0x18;
        pSVar4[1].self_size = 0x50;
        pSVar4[1].packet_size = 0;
        pSVar4[1].src = 0;
        pSVar4[1].dst = 0;
        pSVar4[1].protocol_id = 1;
        pSVar4[1].attr = 0x38;
        pSVar4[1].user_data = 0x38;
        deci5s_ch_fix_send_request(d5cf, 0x78, 0);
    }

    d5cf->d5cf_status = d5cf->d5cf_status | 0x10000U;

    if ((deci5s_mp4_loop_first & 1) == 0) {
        extern a53_u32 deci5s_mp4_loop_first;

        g_d5s.d5s_ch_fix[cpu] = d5cf;
        deci5s_mp4_loop_first = 1;
    }

    for (;;) {
        a53_u32 command;
        a53_u32 bits;
        a53_u32 uVar2;

        do {
            uVar2 = g_d5s.d5s_status;
        } while (msi_wait_command(cpu) < 0);

        command = msi_read_c2p_command(cpu);
        bits = msi_read_c2p_arg1(cpu);
        deci_target_mp4_intr_with_cpu(cpu, bits);
        msi_write_c2p_ack(cpu, command);

        if (((uVar2 & 1) != 0) && (d5cf->d5cf_status & 0x10000U) == 0) {
            break;
        }
        if ((d5cf->d5cf_status >> 15 | 4) != 6) {
            return 0;
        }
    }

    /* Send DCMP protocol stop */
    {
        SceDeci5sHeader *pSVar4;

        pSVar4 = (SceDeci5sHeader *)d5cf->d5cf_low->dtcf_t2c_cmd_buf_ptr;
        bzero(pSVar4, 0x78);
        deci5s_header_init_psdp(pSVar4, 0x78, d5cf->d5cf_node,
                                 0x20ff0100U, d5cf->d5cf_sdbgp);
        pSVar4[1].signature = 0x18;
        pSVar4[1].self_size = 0x50;
        pSVar4[1].packet_size = 0;
        pSVar4[1].src = 0;
        *(a53_u64 *)((a53_u64)&pSVar4[1].timestamp + 4) = 0;
        pSVar4[2].self_size = 0;
        pSVar4[2].packet_size = 0x11111111U;
        pSVar4[2].src = 0;
        pSVar4[2].dst = 0;
        pSVar4[1].dst = 0;
        pSVar4[1].protocol_id = 1;
        pSVar4[1].attr = 0x38;
        pSVar4[1].user_data = 0x38;
        *(a53_u32 *)&pSVar4[1].timestamp = 0x4021010U;
        pSVar4[2].protocol_id = 0;
        pSVar4[2].attr = 0;
        pSVar4[2].user_data = 0;
        pSVar4[2].timestamp = 0;
        deci5s_ch_fix_send_request(d5cf, 0x78, 0);
    }
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_context_check_overflow(deci5s_context_t *dc, a53_u32 csize)
{
    if (dc->dc_res_data_size < dc->dc_res_max) {
        if (csize < dc->dc_res_max - dc->dc_res_data_size) {
            return 1;
        }
        printf_low("%d:%s:res_max 0x%08x <= 0x%08x res_data_size\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci5s_context_check_overflow",
                   (a53_u64)dc->dc_res_max,
                   (a53_u64)(dc->dc_res_data_size + csize));
        return 0;
    }
    printf_low("%d:%s:res_max 0x%08x <= 0x%08x res_data_size\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_context_check_overflow",
               (a53_u64)dc->dc_res_max,
               (a53_u64)dc->dc_res_data_size);
    return 0;
}

void A53_SECTION(".text.el3.loader")
deci5s_dcmp_protocol_info_print(SceDeci5sDcmpProtocolInfo *info)
{
    printf_low("%d:%s:self_size             0x%08x:0x%08lx\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_dcmp_protocol_info_print",
               (a53_u64)info->self_size, 0x18UL);
    printf_low("%d:%s:protocolNumber        0x%08x\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_dcmp_protocol_info_print",
               (a53_u64)info->protocolNumber);
    printf_low("%d:%s:protocolVersion       0x%08x\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_dcmp_protocol_info_print",
               (a53_u64)info->protocolVersion);
    printf_low("%d:%s:protocolVersionLimit  0x%08x\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_dcmp_protocol_info_print",
               (a53_u64)info->protocolVersionLimit);
    printf_low("%d:%s:targetStatus          0x%08x\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_dcmp_protocol_info_print",
               (a53_u64)info->targetStatus);
    printf_low("%d:%s:hostStatus            0x%08x\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_dcmp_protocol_info_print",
               (a53_u64)info->hostStatus);
}

void A53_SECTION(".text.el3.loader")
deci5s_header_init_p_cmd(SceDeci5sHeader *p, a53_u32 psize,
                          SceDeci5sHeader *cmd)
{
    deci5s_header_init_psdp(p, psize, cmd->dst, cmd->src,
                             cmd->protocol_id);
}

void A53_SECTION(".text.el3.loader")
deci5s_header_init_psdp(SceDeci5sHeader *deci5s, a53_u32 packet_size,
                          a53_u32 src, a53_u32 dst, a53_u32 protocol_id)
{
    deci5s->signature = 0x73354450U;
    deci5s->self_size = 0x28;
    deci5s->packet_size = packet_size;
    deci5s->src = src;
    deci5s->dst = dst;
    deci5s->protocol_id = protocol_id;
    deci5s->attr = 0;
    deci5s->user_data = 0;
    deci5s->timestamp = deci5s_timestamp();
}

a53_u64 A53_SECTION(".text.el3.loader") deci5s_timestamp(void)
{
    return (a53_u64)mp4_timer_get_cnt(0);
}

/* =========================================================================
 * SDBGP handler functions (were stubs - now fully canonicalized)
 * ========================================================================= */

int A53_SECTION(".text.el3.loader")
deci5s_ch_fix_send_request(deci5s_ch_fix_t *d5cf, a53_u32 psize, a53_u32 hint)
{
    return deci_target_ch_fix_send_request(d5cf->d5cf_low, psize, hint);
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_check_overflow(deci5s_sdbgp_context_t *dsc, a53_u32 csize)
{
    return deci5s_context_check_overflow(dsc->dsc_dc, csize);
}

SceDeci5sSdbgpMp4PMUCountInfo *A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_handle_get_pmu_count_info(deci5s_sdbgp_context_t *dsc)
{
    if (deci5s_sdbgp_context_alloc_res_info(dsc, 0x10) == 0) {
        return (SceDeci5sSdbgpMp4PMUCountInfo *)dsc->dsc_res_info;
    }
    return (SceDeci5sSdbgpMp4PMUCountInfo *)0;
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_handle_get_conf(deci5s_sdbgp_context_t *dsc)
{
    extern a53_u32 DAT_00114c50;
    extern a53_u32 __loader_el3_text_end;

    a53_u32 uVar9;
    a53_u32 *puVar10;
    a53_u64 *puVar12;
    a53_u32 uVar3;
    a53_u32 i;
    a53_u64 *puVar6;
    deci_target_ch_fix_t *pdVar7;
    deci5s_ch_ring_t *pdVar8;
    deci5s_t *pdVar11;

    printf_low("%d:%s:(dsc %p)\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_sdbgp_context_handle_get_conf", dsc);

    pdVar11 = dsc->dsc_dc->dc_d5s;
    if (deci5s_sdbgp_context_alloc_res_command(dsc) != 0) {
        printf_low("%d:%s:deci5s_sdbgp_context_alloc_res_command() failed %d\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci5s_sdbgp_context_handle_get_conf", -1);
        return -1;
    }

    if (deci5s_sdbgp_context_alloc_res_info(dsc, 0x38) != 0) {
        printf_low("%d:%s:deci5s_sdbgp_context_alloc_res_info() failed\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci5s_sdbgp_context_handle_get_conf");
        return -1;
    }

    puVar12 = (a53_u64 *)dsc->dsc_res_info;
    puVar12[0] = 0x0005020200000038ULL;
    puVar12[1] = 0x00000002000000b7ULL;
    *(a53_u32 *)(puVar12 + 2) = 2;
    *(a53_u32 *)((a53_u64)puVar12 + 0x14) = msi_get_msi_address32();
    *(a53_u32 *)(puVar12 + 3) = (a53_u32)msi_get_vector_for_mm();
    *(a53_u32 *)((a53_u64)puVar12 + 0x1c) = (a53_u32)msi_get_vector_for_io();
    *(a53_u32 *)(puVar12 + 4) = (a53_u32)msi_get_vector_1st_core();
    *(a53_u32 *)((a53_u64)puVar12 + 0x24) = (a53_u32)msi_get_vector_2nd_core();

    for (i = 0; i < 0x10; i += 4) {
        *(a53_u32 *)((a53_u64)puVar12 + i + 0x28) = 0;
    }

    printf_low("%d:%s:BUILD-ID: ID_SIZE:   0x%08x\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_sdbgp_context_handle_get_conf", 4);
    printf_low("%d:%s:BUILD-ID: HASH_SIZE: 0x%08x\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_sdbgp_context_handle_get_conf", 0x10);
    printf_low("%d:%s:BUILD-ID: TYPE:      0x%08x\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_sdbgp_context_handle_get_conf", 3);
    printf_low("%d:%s:BUILD-ID: ID:        0x%08x\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_sdbgp_context_handle_get_conf", 0x554e47);

    uVar3 = 0;
    for (puVar10 = &DAT_00114c50;
         puVar10 < &__loader_el3_text_end;
         ++puVar10) {
        printf_low("%d:%s:BUILD-ID: %p=0x%08x\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci5s_sdbgp_context_handle_get_conf",
                   puVar10, (a53_u64)*puVar10);
        if (uVar3 < 4) {
            *(a53_u32 *)((a53_u64)puVar12 + (a53_u64)uVar3 * 4 + 0x28) = *puVar10;
        }
        ++uVar3;
    }

    for (uVar9 = 0;
         uVar9 < *(a53_u32 *)((a53_u64)puVar12 + 0xc);
         ++uVar9) {
        if (deci5s_sdbgp_context_alloc_res_info(dsc, 0x18) != 0) {
            goto alloc_failed;
        }
        puVar6 = (a53_u64 *)dsc->dsc_res_info;
        *puVar6 = 0x18ULL;
        pdVar7 = (deci_target_ch_fix_t *)pdVar11->d5s_ch_fix[uVar9];
        if (pdVar7 == (deci_target_ch_fix_t *)0
            || pdVar7->dtcf_write_count == 0) {
            *(a53_u32 *)(puVar6 + 1) = 0;
            *(a53_u32 *)(puVar6 + 2) = 0;
            *(a53_u32 *)((a53_u64)puVar6 + 0x14) = 0;
        } else {
            *(a53_u32 *)(puVar6 + 1) = pdVar7->dtcf_c2t_cmd_buf_size;
            *(a53_u32 *)((a53_u64)puVar6 + 0xc) = pdVar7->dtcf_c2t_res_buf_size;
            *(a53_u32 *)(puVar6 + 2) = pdVar7->dtcf_t2c_cmd_buf_size;
            *(a53_u32 *)((a53_u64)puVar6 + 0x14) = pdVar7->dtcf_t2c_res_buf_size;
        }
    }

    for (uVar9 = 0;
         uVar9 < *(a53_u32 *)(puVar12 + 2);
         ++uVar9) {
        a53_u32 ring_status;

        if (deci5s_sdbgp_context_alloc_res_info(dsc, 0x18) != 0) {
            goto alloc_failed;
        }
        puVar6 = (a53_u64 *)dsc->dsc_res_info;
        *puVar6 = 0x18ULL;
        pdVar8 = pdVar11->d5s_ch_ring[uVar9];
        ring_status = (pdVar8 != (deci5s_ch_ring_t *)0)
                          ? pdVar8->d5cr_status
                          : 0;
        *(a53_u32 *)(puVar6 + 2) = ring_status;
        *(a53_u32 *)((a53_u64)puVar6 + 0x14) = 0;
        puVar6[1] = 0ULL;
    }

    return 0;

alloc_failed:
    printf_low("%d:%s:deci5s_sdbgp_context_alloc_res_info() failed\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_sdbgp_context_handle_get_conf");
    return -1;
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_handle_get_reg(deci5s_sdbgp_context_t *dsc)
{
    a53_u32 uVar8;
    a53_u32 uVar1;
    a53_s32 lVar10;
    SceDeci5sSdbgpCommand *pSVar11;
    SceDeci5sSdbgpCommand *pSVar12;

    printf_low("%d:%s:(dsc %p)\n",
               (a53_u64)mp4_get_cpu(),
               "deci5s_sdbgp_context_handle_get_reg", dsc);

    if (deci5s_sdbgp_context_alloc_res_command(dsc) != 0) {
        printf_low("%d:%s:deci5s_sdbgp_context_alloc_res_command() failed %d\n",
                   (a53_u64)mp4_get_cpu(),
                   "deci5s_sdbgp_context_handle_get_reg", -1);
        return -1;
    }

    pSVar11 = dsc->dsc_cmd_command;
    pSVar12 = dsc->dsc_res_command;

    /* Copy request fields to response */
    uVar1 = pSVar11[1].self_size;
    uVar8 = *(a53_u32 *)((a53_u64)&pSVar11[1].sequence_no);
    pSVar12[1].self_size = uVar1;
    *(a53_u32 *)((a53_u64)&pSVar12[1].sequence_no) = uVar8;
    pSVar12[1].attr = 0;

    /* Iterate register IDs and read from debug status */
    for (lVar10 = 0;
         (a53_u32)lVar10 < pSVar11[1].total_size;
         ++lVar10) {
        a53_u64 *puVar9;
        a53_u32 regid;
        a53_u64 regval;

        if (deci5s_sdbgp_context_alloc_res_info(dsc, 0x18) != 0) {
            printf_low(
                "%d:%s:deci5s_sdbgp_context_alloc_res_info(size 0x%08x) failed\n",
                (a53_u64)mp4_get_cpu(),
                "deci5s_sdbgp_context_handle_get_reg", 0x18);
            return -1;
        }
        puVar9 = (a53_u64 *)dsc->dsc_res_info;
        *puVar9 = 0x10ULL;
        puVar9[1] = 0x800000000ULL;
        regid = *(a53_u32 *)((a53_u64)&pSVar11[1].command_no
                             + lVar10 * 4);
        *(a53_u32 *)(puVar9 + 1) = regid;
        regval = mp4_debug_status_get_reg(regid & 0xffff);
        puVar9[2] = regval;
        pSVar12[1].attr = pSVar12[1].attr + 1;
    }
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_handle_get_backtrace(deci5s_sdbgp_context_t *dsc)
{
    a53_u32 uVar6;
    a53_u32 i;
    a53_u64 spsr;
    int is_el0;
    a53_u64 par_res;
    a53_u64 *puVar9;
    a53_u64 *puVar4;
    a53_u64 *puVar11;
    aarch64_frame_t local_68;
    SceDeci5sSdbgpCommand *pSVar10;

    if (deci5s_sdbgp_context_alloc_res_command(dsc) != 0) {
        printf_low(
            "%d:%s:deci5s_sdbgp_context_alloc_res_command() failed %d\n",
            (a53_u64)mp4_get_cpu(),
            "deci5s_sdbgp_context_handle_get_backtrace", -1);
        return -1;
    }

    pSVar10 = dsc->dsc_res_command;
    pSVar10[1].self_size = 0;
    pSVar10[1].total_size = 0;
    pSVar10[1].sequence_no = 1;
    pSVar10[1].attr = 0;

    if (deci5s_sdbgp_context_alloc_res_info(dsc, 0x18) != 0) {
        return -1;
    }

    puVar11 = (a53_u64 *)dsc->dsc_res_info;
    puVar11[0] = 0x18ULL;
    puVar11[1] = 0ULL;
    puVar11[2] = 0ULL;
    pSVar10[1].attr = pSVar10[1].attr + 1;

    mp4_debug_status_get_frame(&local_68);

    __asm__("mrs %0, spsr_el3" : "=r"(spsr));
    is_el0 = (spsr & 0x1f) == 0;

    for (i = 0; i < 0x20; ++i) {
        if (is_el0) {
            __asm__("at s1e1r, %1; mrs %0, par_el1"
                    : "=r"(par_res)
                    : "r"(local_68.af_pc));
            if ((par_res & 1) != 0) {
                is_el0 = 0;
            }
        }

        if (deci5s_sdbgp_context_alloc_res_info(dsc, 0x20) != 0) {
            break;
        }

        puVar9 = (a53_u64 *)dsc->dsc_res_info;
        puVar9[0] = 0x0001000000000020ULL;
        puVar9[1] = local_68.af_sp;
        puVar9[2] = local_68.af_pc;
        puVar9[3] = 0ULL;

        if (is_el0) {
            ++*(a53_u32 *)((a53_u64)puVar11 + 0x14);
        } else {
            ++*(a53_u32 *)(puVar11 + 2);
        }

        if (is_el0) {
            local_68.af_fp = (a53_u64)el0_va_to_el3_va(
                (a53_u8 *)local_68.af_fp);
        }

        __asm__("at s1e3r, %1; mrs %0, par_el1"
                : "=r"(par_res)
                : "r"(local_68.af_fp));
        if ((par_res & 1) != 0) {
            break;
        }

        puVar4 = (a53_u64 *)local_68.af_fp;
        local_68.af_sp = (a53_u64)(puVar4 + 2);
        local_68.af_pc = *(a53_u64 *)(local_68.af_fp + 8) - 4;
        local_68.af_fp = *(a53_u64 *)local_68.af_fp;
    }

    return 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_handle_address_translation(deci5s_sdbgp_context_t *dsc)
{
    a53_u64 va;
    a53_u64 par_val;
    SceDeci5sSdbgpCommand *pSVar5;
    SceDeci5sSdbgpCommand *pSVar4;

    pSVar5 = dsc->dsc_cmd_command;
    va = *(a53_u64 *)&pSVar5[1].self_size;

    printf_low(
        "%d:%s:cmd_command->virtual_address = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_address_translation", va);

    if (deci5s_sdbgp_context_alloc_res_command(dsc) != 0) {
        printf_low(
            "%d:%s:deci5s_sdbgp_context_alloc_res_command() failed %d\n",
            (a53_u64)mp4_get_cpu(),
            "deci5s_sdbgp_context_handle_address_translation", -1);
        return -1;
    }

    pSVar4 = dsc->dsc_res_command;
    *(a53_u64 *)&pSVar4[1].self_size = va;
    pSVar4[1].sequence_no = 0;
    pSVar4[1].attr = 0;

    /* AT S1E0R */
    __asm__("at s1e0r, %1; mrs %0, par_el1"
            : "=r"(par_val)
            : "r"(va));
    printf_low(
        "%d:%s:aarch64_AT_S1E0R = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_address_translation", par_val);
    *(a53_u64 *)&pSVar4[1].command_no = par_val;

    /* AT S1E0W */
    __asm__("at s1e0w, %1; mrs %0, par_el1"
            : "=r"(par_val)
            : "r"(va));
    printf_low(
        "%d:%s:aarch64_AT_S1E0W = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_address_translation", par_val);
    *(a53_u64 *)&pSVar4[2].self_size = par_val;

    /* AT S1E3R */
    __asm__("at s1e3r, %1; mrs %0, par_el1"
            : "=r"(par_val)
            : "r"(va));
    printf_low(
        "%d:%s:aarch64_AT_S1E3R = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_address_translation", par_val);
    *(a53_u64 *)((a53_u64)pSVar4 + 0xcc) = par_val;

    /* AT S1E3W */
    __asm__("at s1e3w, %1; mrs %0, par_el1"
            : "=r"(par_val)
            : "r"(va));
    printf_low(
        "%d:%s:aarch64_AT_S1E3W = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_address_translation", par_val);
    *(a53_u64 *)&pSVar4[2].command_no = par_val;

    return 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_handle_get_mmu(deci5s_sdbgp_context_t *dsc)
{
    a53_u64 mair;
    SceDeci5sSdbgpCommand *pSVar4;

    if (deci5s_sdbgp_context_alloc_res_command(dsc) != 0) {
        printf_low(
            "%d:%s:deci5s_sdbgp_context_alloc_res_command() failed %d\n",
            (a53_u64)mp4_get_cpu(),
            "deci5s_sdbgp_context_handle_get_mmu", -1);
        return -1;
    }

    pSVar4 = dsc->dsc_res_command;
    pSVar4[1].self_size = 0;
    pSVar4[1].total_size = 0;
    __asm__("mrs %0, mair_el3" : "=r"(mair));
    *(a53_u64 *)&pSVar4[1].command_no = mair;
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_handle_get_pmu(deci5s_sdbgp_context_t *dsc)
{
    a53_u64 pmcntenset;
    a53_u32 pmce;
    a53_u64 typer;
    a53_u64 count;
    SceDeci5sSdbgpCommand *pSVar8;
    SceDeci5sSdbgpMp4PMUCountInfo *pSVar7;

    if (deci5s_sdbgp_context_alloc_res_command(dsc) != 0) {
        printf_low(
            "%d:%s:deci5s_sdbgp_context_alloc_res_command() failed %d\n",
            (a53_u64)mp4_get_cpu(),
            "deci5s_sdbgp_context_handle_get_pmu", -1);
        return -1;
    }

    pSVar8 = dsc->dsc_res_command;
    pSVar8[1].self_size = 0;
    pSVar8[1].total_size = 0;

    __asm__("mrs %0, pmcntenset_el0" : "=r"(pmcntenset));
    pmce = (a53_u32)pmcntenset;

    /* Bit 31 (PMCCNTR enable) */
    if ((pmce >> 31) & 1) {
        pSVar7 = deci5s_sdbgp_context_handle_get_pmu_count_info(dsc);
        if (pSVar7 == (SceDeci5sSdbgpMp4PMUCountInfo *)0) {
            return 0;
        }
        pSVar7->self_size = 0x10;
        pSVar7->type = 0x3ff;
        __asm__("mrs %0, pmccntr_el0" : "=r"(count));
        pSVar7->count = count & 0xffffffffULL;
        pSVar8[1].self_size = pSVar8[1].self_size + 1;
    }

    /* PMEVCNTR0 */
    if ((pmce & 1) != 0) {
        pSVar7 = deci5s_sdbgp_context_handle_get_pmu_count_info(dsc);
        if (pSVar7 == (SceDeci5sSdbgpMp4PMUCountInfo *)0) {
            return 0;
        }
        pSVar7->self_size = 0x10;
        __asm__("mrs %0, pmevtyper0_el0" : "=r"(typer));
        pSVar7->type = (a53_u32)typer;
        __asm__("mrs %0, pmevcntr0_el0" : "=r"(count));
        pSVar7->count = count & 0xffffffffULL;
        pSVar8[1].self_size = pSVar8[1].self_size + 1;
    }

    /* PMEVCNTR1 */
    if (((pmce >> 1) & 1) != 0) {
        pSVar7 = deci5s_sdbgp_context_handle_get_pmu_count_info(dsc);
        if (pSVar7 == (SceDeci5sSdbgpMp4PMUCountInfo *)0) {
            return 0;
        }
        pSVar7->self_size = 0x10;
        __asm__("mrs %0, pmevtyper1_el0" : "=r"(typer));
        pSVar7->type = (a53_u32)typer;
        __asm__("mrs %0, pmevcntr1_el0" : "=r"(count));
        pSVar7->count = count & 0xffffffffULL;
        pSVar8[1].self_size = pSVar8[1].self_size + 1;
    }

    /* PMEVCNTR2 */
    if (((pmce >> 2) & 1) != 0) {
        pSVar7 = deci5s_sdbgp_context_handle_get_pmu_count_info(dsc);
        if (pSVar7 == (SceDeci5sSdbgpMp4PMUCountInfo *)0) {
            return 0;
        }
        pSVar7->self_size = 0x10;
        __asm__("mrs %0, pmevtyper2_el0" : "=r"(typer));
        pSVar7->type = (a53_u32)typer;
        __asm__("mrs %0, pmevcntr2_el0" : "=r"(count));
        pSVar7->count = count & 0xffffffffULL;
        pSVar8[1].self_size = pSVar8[1].self_size + 1;
    }

    /* PMEVCNTR3 */
    if (((pmce >> 3) & 1) != 0) {
        pSVar7 = deci5s_sdbgp_context_handle_get_pmu_count_info(dsc);
        if (pSVar7 == (SceDeci5sSdbgpMp4PMUCountInfo *)0) {
            return 0;
        }
        pSVar7->self_size = 0x10;
        __asm__("mrs %0, pmevtyper3_el0" : "=r"(typer));
        pSVar7->type = (a53_u32)typer;
        __asm__("mrs %0, pmevcntr3_el0" : "=r"(count));
        pSVar7->count = count & 0xffffffffULL;
        pSVar8[1].self_size = pSVar8[1].self_size + 1;
    }

    /* PMEVCNTR4 */
    if (((pmce >> 4) & 1) != 0) {
        pSVar7 = deci5s_sdbgp_context_handle_get_pmu_count_info(dsc);
        if (pSVar7 == (SceDeci5sSdbgpMp4PMUCountInfo *)0) {
            return 0;
        }
        pSVar7->self_size = 0x10;
        __asm__("mrs %0, pmevtyper4_el0" : "=r"(typer));
        pSVar7->type = (a53_u32)typer;
        __asm__("mrs %0, pmevcntr4_el0" : "=r"(count));
        pSVar7->count = count & 0xffffffffULL;
        pSVar8[1].self_size = pSVar8[1].self_size + 1;
    }

    /* PMEVCNTR5 */
    if (((pmce >> 5) & 1) != 0) {
        pSVar7 = deci5s_sdbgp_context_handle_get_pmu_count_info(dsc);
        if (pSVar7 == (SceDeci5sSdbgpMp4PMUCountInfo *)0) {
            return 0;
        }
        pSVar7->self_size = 0x10;
        __asm__("mrs %0, pmevtyper5_el0" : "=r"(typer));
        pSVar7->type = (a53_u32)typer;
        __asm__("mrs %0, pmevcntr5_el0" : "=r"(count));
        pSVar7->count = count & 0xffffffffULL;
        pSVar8[1].self_size = pSVar8[1].self_size + 1;
    }

    return 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_handle_get_syshub_tlb(deci5s_sdbgp_context_t *dsc)
{
    a53_u32 uVar1;
    a53_u32 tlb;
    a53_u32 tlb0;
    a53_u32 tlb1;
    a53_u32 tlb2;
    a53_u32 tlb3;
    a53_u32 sub;
    a53_u32 attr1;
    a53_u32 *puVar4;
    SceDeci5sSdbgpCommand *pSVar5;

    if (deci5s_sdbgp_context_alloc_res_command(dsc) != 0) {
        printf_low(
            "%d:%s:deci5s_sdbgp_context_alloc_res_command() failed %d\n",
            (a53_u64)mp4_get_cpu(),
            "deci5s_sdbgp_context_handle_get_syshub_tlb", -1);
        return -1;
    }

    pSVar5 = dsc->dsc_res_command;
    pSVar5[1].self_size = 0;
    pSVar5[1].total_size = 0;

    for (uVar1 = 1; uVar1 < 0x3f; ++uVar1) {
        syshub_tlb_get(uVar1, &tlb0, &tlb1, &tlb2, &tlb3,
                       &sub, &attr1);
        if (tlb0 != 0) {
            if (deci5s_sdbgp_context_alloc_res_info(dsc, 0x20) != 0) {
                return -1;
            }
            puVar4 = (a53_u32 *)dsc->dsc_res_info;
            puVar4[0] = 0x20;
            puVar4[1] = uVar1;
            puVar4[2] = tlb0;
            puVar4[3] = tlb1;
            puVar4[4] = tlb2;
            puVar4[5] = tlb3;
            puVar4[6] = sub;
            puVar4[7] = attr1;
            pSVar5[1].self_size = pSVar5[1].self_size + 1;
        }
    }

    return 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_handle_test(deci5s_sdbgp_context_t *dsc)
{
    a53_u64 cmd_arg;
    a53_u32 uVar6;
    a53_u32 *puVar7;
    SceDeci5sSdbgpCommand *pSVar8;
    SceDeci5sSdbgpCommand *pSVar9;
    main_mp4_param_t *pmVar4;

    printf_low("%d:%s:()\n", (a53_u64)mp4_get_cpu(),
               "deci5s_sdbgp_context_handle_test");

    if (deci5s_sdbgp_context_alloc_res_command(dsc) != 0) {
        printf_low(
            "%d:%s:deci5s_sdbgp_context_alloc_res_command() failed %d\n",
            (a53_u64)mp4_get_cpu(),
            "deci5s_sdbgp_context_handle_test", -1);
        return -1;
    }

    pSVar8 = dsc->dsc_cmd_command;
    pSVar9 = dsc->dsc_res_command;

    pSVar9[1].self_size = pSVar8[1].self_size;
    pSVar9[1].total_size = 0;
    pSVar9[1].sequence_no = 0;
    pSVar9[1].attr = 0;

    /* Copy arguments */
    for (uVar6 = 0; uVar6 < 8; ++uVar6) {
        a53_u64 arg_val;

        pSVar9[1].total_size = pSVar8[1].total_size;
        arg_val = (uVar6 < pSVar8[1].total_size)
                      ? *(a53_u64 *)((a53_u64)&pSVar8[1].command_no
                                     + uVar6 * 8)
                      : 0ULL;
        *(a53_u64 *)((a53_u64)&pSVar9[1].command_no + uVar6 * 8) = arg_val;
        printf_low(
            "%d:%s:res_command->arg[%d] = 0x%016lx\n",
            (a53_u64)mp4_get_cpu(),
            "deci5s_sdbgp_context_handle_test", uVar6, arg_val);
    }

    /* Print IOMMU/SYSHUB diagnostic info */
    pmVar4 = msi_get_main_param();

    printf_low(
        "%d:%s:SYSHUB_TLB10                          = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0x28000000ULL);
    printf_low(
        "%d:%s:SYSHUB_TLB10                          = 0x%016lx [IOMMU]\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0x54000000ULL);
    printf_low(
        "%d:%s:mm4p->mm4p_iommu_mmio.mimi_iommu_addr = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test",
        pmVar4->mm4p_iommu_mmio.mimi_iommu_addr);

    printf_low(
        "%d:%s:SYSHUB_TLB11                          = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0x2c000000ULL);
    printf_low(
        "%d:%s:SYSHUB_TLB11                          = 0x%016lx [IOMMU]\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0x54000000ULL);
    printf_low(
        "%d:%s:mm4p->mm4p_sdma0_mmio.mimi_iommu_addr = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test",
        pmVar4->mm4p_sdma0_mmio.mimi_iommu_addr);

    printf_low(
        "%d:%s:SYSHUB_TLB12                          = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0x30000000ULL);
    printf_low(
        "%d:%s:SYSHUB_TLB12                          = 0x%016lx [IOMMU]\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0x54000000ULL);
    printf_low(
        "%d:%s:mm4p->mm4p_sdma1_mmio.mimi_iommu_addr = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test",
        pmVar4->mm4p_sdma1_mmio.mimi_iommu_addr);

    printf_low(
        "%d:%s:SYSHUB_TLB15                          = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0x3c000000ULL);
    printf_low(
        "%d:%s:SYSHUB_TLB15                          = 0x%016lx [BYPASS]\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0xfc000000ULL);
    printf_low(
        "%d:%s:mm4p->mm4p_iommu_mmio.mini_pa         = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test",
        pmVar4->mm4p_iommu_mmio.mimi_pa);

    printf_low(
        "%d:%s:SYSHUB_TLB16                          = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0x40000000ULL);
    printf_low(
        "%d:%s:SYSHUB_TLB16                          = 0x%016lx [BYPASS]\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0x18000000ULL);
    printf_low(
        "%d:%s:mm4p->mm4p_sdma0_mmio.mimi_pa         = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test",
        pmVar4->mm4p_sdma0_mmio.mimi_pa);

    printf_low(
        "%d:%s:SYSHUB_TLB17                          = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0x44000000ULL);
    printf_low(
        "%d:%s:SYSHUB_TLB17                          = 0x%016lx [BYPASS]\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test", 0x18000000ULL);
    printf_low(
        "%d:%s:mm4p->mm4p_sdna1_mmio.mimi_pa         = 0x%016lx\n",
        (a53_u64)mp4_get_cpu(),
        "deci5s_sdbgp_context_handle_test",
        pmVar4->mm4p_sdma1_mmio.mimi_pa);

    /* Read MMIO based on command argument */
    cmd_arg = *(a53_u64 *)&pSVar8[1].command_no;
    puVar7 = (a53_u32 *)0;
    switch (cmd_arg) {
    case 0xa:
        puVar7 = (a53_u32 *)0x28200018ULL;
        break;
    case 0xb:
        puVar7 = (a53_u32 *)0x2c400018ULL;
        break;
    case 0xc:
        puVar7 = (a53_u32 *)0x30600018ULL;
        break;
    case 0xf:
        puVar7 = (a53_u32 *)0x3dd88018ULL;
        break;
    case 0x10:
        puVar7 = (a53_u32 *)0x40900000ULL;
        break;
    case 0x11:
        puVar7 = (a53_u32 *)0x44904000ULL;
        break;
    default:
        return 0;
    }

    printf_low("%d:%s:p  = %p\n", (a53_u64)mp4_get_cpu(),
               "deci5s_sdbgp_context_handle_test", puVar7);
    printf_low("%d:%s:*p = 0x%08x\n", (a53_u64)mp4_get_cpu(),
               "deci5s_sdbgp_context_handle_test", (a53_u64)*puVar7);
    return 0;
}

int A53_SECTION(".text.el3.loader")
deci5s_sdbgp_context_alloc_res_info(deci5s_sdbgp_context_t *dsc, a53_u32 isize)
{
    a53_u32 uVar2;
    a53_u32 uVar1;
    deci5s_context_t *pdVar5;
    SceDeci5sSdbgpCommand *pSVar6;
    SceDeci5sSdbgpHeader *pSVar7;
    SceDeci5sSdbgpHeader *pSVar8;

    if (deci5s_sdbgp_context_check_overflow(dsc, isize) == 0) {
        /* Overflow: send current response and start new packet */
        printf_low("%s:OVERFLOW\n",
                   "deci5s_sdbgp_context_alloc_res_info");

        uVar2 = dsc->dsc_res_command->self_size;
        uVar1 = dsc->dsc_res_command->packet_no;

        deci5s_header_init_p_cmd(dsc->dsc_res_deci5s,
                                  dsc->dsc_dc->dc_res_data_size,
                                  dsc->dsc_cmd_deci5s);

        pSVar8 = dsc->dsc_res_sdbgp;
        pSVar8->attr = pSVar8->attr | 0x80000000U;
        dsc->dsc_res_command->attr =
            dsc->dsc_res_command->attr | 0x80000000U;

        printf_low(
            "%d:%s:not implemented!\n",
            (a53_u64)mp4_get_cpu(),
            "deci5s_sdbgp_context_send");

        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\deci5s_mp4.c",
                   "deci5s_sdbgp_context_send", 0x43f, 0, "0");

        /* Reset for next packet */
        pSVar8 = dsc->dsc_res_sdbgp;
        pdVar5 = dsc->dsc_dc;
        pSVar8->total_size = pSVar8->self_size;
        pSVar8->packet_no = pSVar8->packet_no + 1;
        pSVar8->attr = 0;
        pSVar8->n_command = 0;

        pSVar7 = pSVar8 + 1;
        pSVar7->self_size = uVar2;
        dsc->dsc_res_command = (SceDeci5sSdbgpCommand *)pSVar7;
        pSVar7->total_size = uVar2;

        pSVar6 = dsc->dsc_cmd_command;
        pSVar7->sequence_no =
            pSVar6->self_size & 0xffffffU | 0x2000000U;
        pSVar7->packet_no = 0;
        pSVar7->attr = pSVar6->command_no;
        pSVar7->n_command = uVar1 + 1;

        pdVar5->dc_res_data_size = uVar2 + 0x40;
        pSVar8->total_size = pSVar8->total_size + uVar2;
        pSVar8->n_command = pSVar8->n_command + 1;

        dsc->dsc_res_info =
            (a53_u8 *)((a53_u64)&pSVar7->self_size + (a53_u64)uVar2);

        if (deci5s_sdbgp_context_check_overflow(dsc, isize) == 0) {
            printf_low(
                "%d:%s:OVERFLOW AGAIN\n",
                (a53_u64)mp4_get_cpu(),
                "deci5s_sdbgp_context_alloc_res_info");
            return -1;
        }
    }

    /* Normal allocation */
    pdVar5 = dsc->dsc_dc;
    pSVar8 = dsc->dsc_res_sdbgp;

    dsc->dsc_res_info =
        pdVar5->dc_res_ptr + pdVar5->dc_res_data_size;
    pdVar5->dc_res_data_size =
        pdVar5->dc_res_data_size + isize;
    pSVar8->total_size = pSVar8->total_size + isize;
    dsc->dsc_res_command->total_size =
        dsc->dsc_res_command->total_size + isize;

    return 0;
}
