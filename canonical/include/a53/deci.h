#ifndef A53_DECI_H
#define A53_DECI_H

#include "a53_abi.h"

/* =========================================================================
 * DECI SHM magic constants
 * ========================================================================= */

#define DECI_SHM_NODE_MAGIC1_CP              0xc8f7e343U
#define DECI_SHM_NODE_MAGIC1_MP4             0x8e7e62d6U
#define DECI_SHM_NODE_MAGIC1_MP3             0x8fec3303U
#define DECI_SHM_NODE_MAGIC_MAIN             0x812c4d3dU
#define DECI_SHM_MAGIC_MAIN                  0x812c4d3dU
#define DECI_SHM_MAGIC_MP3                   0x8fec3303U
#define DECI_SHM_MAGIC_SYCORAX               0xfbdf45e4U
#define DECI_SHM_MAGIC_CP                    0xf470b785U
#define DECI_SHM_MAGIC_MP4_OTHER             0x43fe7688U
#define DECI_SHM_MAGIC_SYNC                  0x87661e49U
#define DECI_SHM_CH_NODE_FIX_MAGIC_CP_TO_MP4 0xcb9b4abaU
#define DECI_SHM_CH_NODE_RING_MAGIC_CP_TO_MP4 0x8e154c2aU

/* =========================================================================
 * DECI shared memory types
 * ========================================================================= */

typedef struct deci_shm_common {
    a53_u32 dsc_magic1;
    a53_u32 dsc_self_size;
    a53_u32 dsc_offset;
    a53_u32 dsc_pad;
} deci_shm_common_t;

typedef struct deci_shm_common_v2 {
    a53_u32 dsc2_self_size;
    a53_u32 dsc2_n_node;
    a53_u32 dsc2_n_buf_size;
    a53_u32 dsc2_pad;
} deci_shm_common_v2_t;

typedef struct deci_shm_mbox {
    a53_u32 dsm_mbox;
    a53_u32 dsm_sig_no;
    a53_u32 dsm_sig_dst;
    a53_u32 dsm_sig_bit;
} deci_shm_mbox_t;

typedef struct deci_shm_node {
    a53_u32 dsn_self_size;
    a53_u32 dsn_magic1;
    a53_u32 dsn_n_ch_fix;
    a53_u32 dsn_n_ch_ring;
    a53_u32 dsn_ch_offset;
} deci_shm_node_t;

typedef struct deci_shm_common_target {
    a53_u32 dsct_magic;
    a53_u32 dsct_node_offset;
    a53_u32 dsct_base_ch_fix;
    a53_u32 dsct_base_ch_ring;
} deci_shm_common_target_t;

typedef struct deci_shm_ch_node_fix {
    a53_u32         dscnf_self_size;
    a53_u32         dscnf_magic;
    deci_shm_mbox_t dscnf_mbox;
    a53_u32         dscnf_buf_spec_offset_cmd;
    a53_u32         dscnf_buf_spec_offset_res;
    a53_u32         dscnf_buf_status_cmd;
    a53_u32         dscnf_buf_data_size_cmd;
    a53_u32         dscnf_buf_status_res;
    a53_u32         dscnf_buf_data_size_res;
} deci_shm_ch_node_fix_t;

typedef struct deci_shm_ch_node_ring {
    a53_u32 dscnr_self_size;
    a53_u32 dscnr_magic;
} deci_shm_ch_node_ring_t;

typedef struct deci_shm_buf {
    a53_u32 dsb_self_size;
    a53_u32 dsb_magic;
    a53_u32 dsb_id;
    a53_u32 dsb_buf_offset;
    a53_u32 dsb_buf_size;
} deci_shm_buf_t;

typedef struct deci_shm_mp4 {
    void    *dsm4_firm;
    a53_u8  *dsm4_shm_common;
    a53_u32  dsm4_cp_param0;
    a53_u32  dsm4_cp_param2;
} deci_shm_mp4_t;

typedef struct deci_sig_mp4 {
    a53_u64 dsim_base;
    a53_u64 dsim_msi;
    a53_u64 dsim_sig1;
    a53_u64 dsim_sig2;
    a53_u64 dsim_sig3;
} deci_sig_mp4_t;

/* =========================================================================
 * DECI target types
 * ========================================================================= */

typedef struct deci_target_ch_fix deci_target_ch_fix_t;
typedef struct deci_target_ch_ring deci_target_ch_ring_t;
typedef struct deci_target_md deci_target_md_t;

typedef struct deci_target {
    a53_u32                  dts_n_ch_fix;
    a53_u32                  dts_n_ch_ring;
    deci_target_ch_fix_t   **dts_ch_fix_vec;
    deci_target_ch_ring_t  **dts_ch_ring_vec;
    deci_target_md_t        *dts_md;
} deci_target_t;

/* =========================================================================
 * DECI5S types
 * ========================================================================= */

typedef struct deci5s_ch_fix {
    deci_target_ch_fix_t *d5cf_low;
    a53_u32               d5cf_status;
    a53_u32               d5cf_id;
    a53_u32               d5cf_node;
    a53_u32               d5cf_sdbgp;
    a53_u32               d5cf_sig_bit;
} deci5s_ch_fix_t;

typedef struct deci5s_ch_ring {
    a53_u32 d5cr_status;
} deci5s_ch_ring_t;

typedef struct deci5s_sttyp {
    a53_u32 d5ss_seq_no;
} deci5s_sttyp_t;

typedef struct deci5s_sdbgp_command_spec deci5s_sdbgp_command_spec_t;
typedef struct deci5s_sdbgp_context deci5s_sdbgp_context_t;

typedef struct deci5s_sdbgp_command_spec {
    a53_u32 dscs_type;
    a53_u32 dscs_res_size;
    void  (*dscs_func)(deci5s_sdbgp_context_t *dsc);
} deci5s_sdbgp_command_spec_t;

typedef struct deci5s {
    deci5s_ch_fix_t                *d5s_ch_fix[2];
    deci5s_ch_ring_t               *d5s_ch_ring[2];
    a53_u32                         d5s_status;
    deci5s_sdbgp_command_spec_t     d5s_sdbgp_command_spec[32];
} deci5s_t;

typedef struct deci5s_context {
    deci5s_t            *dc_d5s;
    deci5s_ch_fix_t     *dc_ch_fix;
    deci5s_ch_ring_t    *dc_ch_ring;
    a53_u32              dc_mode;
    a53_u32              dc_id;
    a53_u32              dc_result;
    a53_u8              *dc_cmd_ptr;
    a53_u32              dc_cmd_size;
    a53_u8              *dc_res_ptr;
    a53_u32              dc_res_max;
    a53_u32              dc_res_data_size;
} deci5s_context_t;

/* SceDeci5s protocol header types */
typedef struct SceDeci5sHeader {
    a53_u32 signature;
    a53_u32 self_size;
    a53_u32 packet_size;
    a53_u32 src;
    a53_u32 dst;
    a53_u32 protocol_id;
    a53_u32 attr;
    a53_u32 user_data;
    a53_u64 timestamp;
} SceDeci5sHeader;

typedef struct SceDeci5sSdbgpHeader {
    a53_u32 self_size;
    a53_u32 total_size;
    a53_u32 sequence_no;
    a53_u32 packet_no;
    a53_u32 attr;
    a53_u32 n_command;
} SceDeci5sSdbgpHeader;

typedef struct SceDeci5sSdbgpCommand {
    a53_u32 self_size;
    a53_u32 total_size;
    a53_u32 sequence_no;
    a53_u32 packet_no;
    a53_u32 attr;
    a53_u32 n_command;
    a53_u32 command_no;
    SceDeci5sHeader deci5s;
} SceDeci5sSdbgpCommand;

typedef struct SceDeci5sSdbgpCmd {
    SceDeci5sSdbgpHeader sdbgp;
} SceDeci5sSdbgpCmd;

typedef struct SceDeci5sSdbgpRes {
    SceDeci5sSdbgpHeader sdbgp;
} SceDeci5sSdbgpRes;

typedef struct SceDeci5sDcmpProtocolInfo {
    a53_u32 self_size;
    a53_u32 protocolNumber;
    a53_u32 protocolVersion;
    a53_u32 protocolVersionLimit;
    a53_u32 targetStatus;
    a53_u32 hostStatus;
} SceDeci5sDcmpProtocolInfo;

typedef struct deci5s_sdbgp_context {
    deci5s_context_t            *dsc_dc;
    SceDeci5sSdbgpCmd           *dsc_cmd;
    SceDeci5sHeader             *dsc_cmd_deci5s;
    SceDeci5sSdbgpHeader        *dsc_cmd_sdbgp;
    SceDeci5sSdbgpCommand       *dsc_cmd_command;
    deci5s_sdbgp_command_spec_t *dsc_cmd_spec;
    SceDeci5sSdbgpRes           *dsc_res;
    SceDeci5sHeader             *dsc_res_deci5s;
    SceDeci5sSdbgpHeader        *dsc_res_sdbgp;
    SceDeci5sSdbgpCommand       *dsc_res_command;
    a53_u8                      *dsc_res_info;
} deci5s_sdbgp_context_t;

typedef struct SceDeci5sSdbgpMp4PMUCountInfo {
    a53_u32 self_size;
    a53_u32 type;
    a53_u64 count;
} SceDeci5sSdbgpMp4PMUCountInfo;

/* DECI target metadata vtable */
typedef struct deci_target_md {
    char             *dtmd_name;
    a53_u32           dtmd_shm_node_target_magic1;
    a53_u32           dtmd_shm_ch_node_fix_cp_to_target_magic;
    a53_u32           dtmd_shm_ch_node_fix_target_to_cp_magic;
    deci_shm_common_t *(*dtmd_get_shm_common)(void);
    deci_shm_node_t   *(*dtmd_get_shm_node_target)(deci_shm_common_t *dsc);
    deci_shm_ch_node_fix_t *(*dtmd_get_shm_ch_fix_cp_to_target)(deci_shm_common_t *dsc, a53_u32 ui);
    deci_shm_ch_node_fix_t *(*dtmd_get_shm_ch_fix_target_to_cp)(deci_shm_common_t *dsc, a53_u32 ui);
    deci_shm_ch_node_ring_t *(*dtmd_get_shm_ch_ring_cp_to_target)(deci_shm_common_t *dsc, a53_u32 ui);
    deci_shm_ch_node_ring_t *(*dtmd_get_shm_ch_ring_target_to_cp)(deci_shm_common_t *dsc, a53_u32 ui);
    int (*dtmd_int_to_cp)(a53_u32 no, a53_u32 dst, a53_u32 bit, a53_u32 *mbox, a53_u32 val, a53_u32 hint0);
    int (*dtmd_wait_clear_target_to_cp)(a53_u32 no, a53_u32 dst, a53_u32 bit);
    int (*dtmd_clear_int_from_cp)(a53_u32 no, a53_u32 dst, a53_u32 bit);
    deci_target_t    *dtmd_dts;
} deci_target_md_t;

/* deci_target_ch_fix_t full definition (needs deci5s_ch_fix_t forward decl) */
struct deci_target_ch_fix {
    a53_u32                  dtcf_self_size;
    a53_u32                  dtcf_id;
    a53_u32                  dtcf_magic1;
    a53_u32                  dtcf_intr_count;
    a53_u32                  dtcf_mbox_req_count;
    a53_u32                  dtcf_mbox_free_count;
    a53_u32                  dtcf_mbox_nop_count;
    a53_u32                  dtcf_read_count;
    a53_u32                  dtcf_write_count;
    a53_u64                  dtcf_total_read_size;
    a53_u64                  dtcf_total_write_size;
    deci_shm_ch_node_fix_t  *dtcf_ch_fix_c2t;
    deci_shm_ch_node_fix_t  *dtcf_ch_fix_t2c;
    a53_u32                 *dtcf_mbox_c2t;
    a53_u32                  dtcf_sig_no_c2t;
    a53_u32                  dtcf_sig_dst_c2t;
    a53_u32                  dtcf_sig_bit_c2t;
    a53_u32                 *dtcf_mbox_t2c;
    a53_u32                  dtcf_sig_no_t2c;
    a53_u32                  dtcf_sig_dst_t2c;
    a53_u32                  dtcf_sig_bit_t2c;
    a53_u32                  dtcf_c2t_cmd_bid;
    a53_u8                  *dtcf_c2t_cmd_buf_ptr;
    a53_u32                  dtcf_c2t_cmd_buf_size;
    a53_u32                 *dtcf_c2t_cmd_status_ptr;
    a53_u32                 *dtcf_c2t_cmd_data_size_ptr;
    a53_u32                  dtcf_t2c_cmd_bid;
    a53_u8                  *dtcf_t2c_cmd_buf_ptr;
    a53_u32                  dtcf_t2c_cmd_buf_size;
    a53_u32                 *dtcf_t2c_cmd_status_ptr;
    a53_u32                 *dtcf_t2c_cmd_data_size_ptr;
    a53_u32                  dtcf_c2t_res_bid;
    a53_u8                  *dtcf_c2t_res_buf_ptr;
    a53_u32                  dtcf_c2t_res_buf_size;
    a53_u32                 *dtcf_c2t_res_status_ptr;
    a53_u32                 *dtcf_c2t_res_data_size_ptr;
    a53_u32                  dtcf_t2c_res_bid;
    a53_u8                  *dtcf_t2c_res_buf_ptr;
    a53_u32                  dtcf_t2c_res_buf_size;
    a53_u32                 *dtcf_t2c_res_status_ptr;
    a53_u32                 *dtcf_t2c_res_data_size_ptr;
    struct deci5s_ch_fix    *dtcf_d5cf;
    a53_u32                  dtcf_magic2;
};

/* =========================================================================
 * DECI5S function prototypes
 * ========================================================================= */

void deci5s_assert(char *file, char *func, a53_u32 line, int c, char *cstr);
char *deci5s_basename(char *f);
a53_u32 deci5s_get_cpu(void);
a53_u32 deci5s_roundup64(a53_u32 orig);
a53_u8 *deci5s_ch_fix_get_t2c_cmd_ptr(deci5s_ch_fix_t *d5cf);
deci5s_sdbgp_command_spec_t *deci5s_sdbgp_com_spec_vector_find(
    deci5s_sdbgp_command_spec_t *vec, a53_u32 type);
int deci5s_ch_fix_send_request(deci5s_ch_fix_t *d5cf, a53_u32 psize, a53_u32 hint);
int deci5s_sdbgp_context_alloc_res_info(deci5s_sdbgp_context_t *dsc, a53_u32 isize);
int deci5s_sdbgp_context_alloc_res_command(deci5s_sdbgp_context_t *dsc);
int deci5s_sdbgp_context_alloc_res_command_common(deci5s_sdbgp_context_t *dsc,
    a53_u32 type, a53_u32 csize);
void deci5s_context_init(deci5s_context_t *dc, a53_u32 mode, a53_u32 id);
int deci5s_context_handle_packet(deci5s_context_t *dc);
int deci5s_context_handle_dcmp_packet(deci5s_context_t *dc);
int deci5s_mp4_start(a53_u32 core);
int deci5s_send_sttyp(char *msg, a53_u64 len);
int deci5s_mp4_panic_and_loop(a53_u32 cpu, a53_u64 pc);
int deci5s_context_check_overflow(deci5s_context_t *dc, a53_u32 csize);
void deci5s_dcmp_protocol_info_print(SceDeci5sDcmpProtocolInfo *info);
void deci5s_header_init_p_cmd(SceDeci5sHeader *p, a53_u32 psize, SceDeci5sHeader *cmd);
void deci5s_header_init_psdp(SceDeci5sHeader *deci5s, a53_u32 packet_size,
    a53_u32 src, a53_u32 dst, a53_u32 protocol_id);
a53_u64 deci5s_timestamp(void);

/* =========================================================================
 * DECI SHM function prototypes
 * ========================================================================= */

a53_u16 deci_shm_mbox_get_op0(a53_u32 mbox);
a53_u16 deci_shm_mbox_get_op1(a53_u32 mbox);
a53_u32 deci_shm_make_mbox0(a53_u16 type, a53_u32 bid);
a53_u16 deci_shm_make_mbox_16b(a53_u16 type, a53_u32 bid);
a53_u32 deci_shm_make_mbox01(a53_u16 type0, a53_u32 bid0, a53_u16 type1, a53_u32 bid1);
a53_u8 *deci_shm_common_get_ptr(deci_shm_common_t *dsc, a53_u32 off);
int deci_shm_common_check(deci_shm_common_t *dsc);
deci_shm_node_t *deci_shm_common_v2_get_node_cp(deci_shm_common_v2_t *dsc2);
deci_shm_common_target_t *deci_shm_common_v2_find_target(deci_shm_common_v2_t *dsc2, a53_u32 magic);
deci_shm_node_t *deci_shm_common_v2_get_node_main(deci_shm_common_v2_t *dsc2);
deci_shm_node_t *deci_shm_common_v2_get_node_sycorax(deci_shm_common_v2_t *dsc2);
int deci_shm_common_get_version(deci_shm_common_t *dsc);
a53_u32 deci_shm_common_get_offset(deci_shm_common_t *dsc, a53_u8 *ptr);
deci_shm_node_t *deci_shm_common_get_node_cp(deci_shm_common_t *dsc);
deci_shm_node_t *deci_shm_common_get_node(deci_shm_common_t *dsc, a53_u32 magic);
deci_shm_node_t *deci_shm_common_get_node_main(deci_shm_common_t *dsc);
deci_shm_node_t *deci_shm_common_get_node_mp3(deci_shm_common_t *dsc);
deci_shm_node_t *deci_shm_common_get_node_mp4(deci_shm_common_t *dsc);
deci_shm_ch_node_fix_t *deci_shm_common_get_ch_fix_cp_to_mp4(deci_shm_common_t *dsc, a53_u32 ui);
deci_shm_ch_node_fix_t *deci_shm_common_get_ch_fix_mp4_to_cp(deci_shm_common_t *dsc, a53_u32 ui);
deci_shm_ch_node_ring_t *deci_shm_common_get_ch_ring_cp_to_mp4(deci_shm_common_t *dsc, a53_u32 ui);
deci_shm_ch_node_ring_t *deci_shm_common_get_ch_ring_mp4_to_cp(deci_shm_common_t *dsc, a53_u32 ui);
int deci_shm_buf_check(deci_shm_buf_t *buf);
int deci_shm_node_check_with_magic(deci_shm_node_t *dsn, a53_u32 magic);
int deci_shm_ch_node_fix_check_with_magic(deci_shm_ch_node_fix_t *dscnf, a53_u32 magic);
int deci_shm_ch_node_ring_check(deci_shm_ch_node_ring_t *dscnr);

/* =========================================================================
 * DECI SHM MP4 function prototypes
 * ========================================================================= */

deci_shm_common_t *deci_shm_mp4_common(void);
int deci_shm_mp4_start(a53_u32 core);

/* =========================================================================
 * DECI SIG MP4 function prototypes
 * ========================================================================= */

void deci_mp4_sig1_write_int_to_sycorax(a53_u32 bit, a53_u32 *dst, a53_u32 val);
a53_u32 deci_mp4_sig2_read_int_from_emc(void);
a53_u32 deci_mp4_sig3_read_int_from_emc(void);
void deci_mp4_sig3_clear_int_from_emc(a53_u32 v);
int deci_sig_mp4_start(void);

/* =========================================================================
 * DECI TARGET MP4 function prototypes
 * ========================================================================= */

int deci_target_ch_fix_send_request(deci_target_ch_fix_t *dtcf, a53_u32 psize, a53_u32 hint);
int deci_target_ch_fix_write_mbox(deci_target_ch_fix_t *dtcf, a53_u32 mbox, a53_u32 hint);
int deci_target_ch_fix_send_reply(deci_target_ch_fix_t *dtcf, deci5s_context_t *dc);
int deci_target_ch_fix_handle_irq_poll(deci_target_ch_fix_t *dtcf);
int deci_target_ch_fix_handle_intr(deci_target_ch_fix_t *dtcf);
int deci_target_mp4_intr_with_cpu(a53_u32 cpu, a53_u32 bits);
deci_target_t *deci_target_get(void);
deci_target_ch_fix_t *deci_target_get_ch_fix(deci_target_t *dts, a53_u32 no);
int deci_target_mp4_intr(a53_u32 bits);
int deci_target_mp4_poll(void);
int deci_target_start(void);
int deci_target_up(deci_target_t *dts, a53_u32 core);
deci_target_md_t *deci_target_get_md_variable(deci_target_t *dts);
deci_target_ch_fix_t *deci_target_mp4_get_ch_fix(a53_u32 id);
int deci_target_mp4_start(a53_u32 core);
int deci_target_mp4_up(a53_u32 core);
deci_target_md_t *deci_target_get_md(void);
int deci_target_ch_fix_handle_op_intr(deci_target_ch_fix_t *dtcf, a53_u16 op);

/* ---- MP4 target vtable implementations (forward declared for MD_SETUP) ---- */

int deci_target_mp4_int_to_cp(a53_u32 no, a53_u32 dst, a53_u32 bit,
                               a53_u32 *mbox, a53_u32 val, a53_u32 hint0);
int deci_target_mp4_clear_int_from_cp(a53_u32 no, a53_u32 dst, a53_u32 bit);
int deci_target_mp4_wait_clear_target_to_cp(a53_u32 no, a53_u32 dst, a53_u32 bit);
deci_shm_common_t *deci_target_mp4_get_shm_common(void);
deci_shm_node_t *deci_target_mp4_get_shm_node_target(deci_shm_common_t *dsc);
deci_shm_ch_node_fix_t *deci_target_mp4_get_shm_ch_fix_cp_to_target(
    deci_shm_common_t *dsc, a53_u32 ui);
deci_shm_ch_node_fix_t *deci_target_mp4_get_shm_ch_fix_target_to_cp(
    deci_shm_common_t *dsc, a53_u32 ui);
deci_shm_ch_node_ring_t *deci_target_mp4_get_shm_ch_ring_cp_to_target(
    deci_shm_common_t *dsc, a53_u32 ui);
deci_shm_ch_node_ring_t *deci_target_mp4_get_shm_ch_ring_target_to_cp(
    deci_shm_common_t *dsc, a53_u32 ui);

/* ---- DECI global variables (shared across deci_shm_mp4.c / deci_target_mp4.c) ---- */

extern deci_shm_mp4_t g_deci_shm_mp4_data;
extern deci_shm_mp4_t *g_deci_shm_mp4;
extern deci_target_t g_deci_target_data;
extern deci_target_t *g_deci_target;
extern deci_target_md_t g_deci_target_md;
extern deci_target_ch_fix_t *g_vecp_deci_target_ch_fix[2];
extern deci_target_ch_fix_t g_vec_deci_target_ch_fix[2];

#endif
