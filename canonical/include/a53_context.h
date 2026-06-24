#ifndef A53_CONTEXT_H
#define A53_CONTEXT_H

#include <stddef.h>

#include "a53_abi.h"

/* ---- IOMMU map info struct (shared between msi.c, syshub.c, gic.c) ---- */
typedef struct {
    a53_u64 mimi_pa;
    a53_u64 mimi_pa_base;
    a53_u64 mimi_iommu_addr;
    a53_u64 mimi_physical_size;
    a53_u64 mimi_iommu_size;
    a53_u64 mimi_syshub_base;
} mp4_iommu_map_info_t;

/* ---- Main MP4 parameter block at 0x88000c00 ---- */
typedef struct {
    a53_u32 mm4p_self_size;
    a53_u32 mm4p_magic;
    a53_u32 mm4p_flags;
    a53_u32 mm4p_reserved;
    a53_u64 mm4p_cyclecount;
    a53_u64 mm4p_msi_address;
    a53_u32 mm4p_msi_vector_loader0;
    a53_u32 mm4p_pad1;
    a53_u32 mm4p_msi_io;
    a53_u32 mm4p_pad2;
    a53_u32 mm4p_msi_mm;
    a53_u32 mm4p_pad3;
    a53_u32 mm4p_msi_vector_loader1;
    a53_u32 mm4p_pad4;
    a53_u64 mm4p_pasid_kernel;
    a53_u64 mm4p_mm_rings_ioma;
    a53_u64 mm4p_mapper_page_table_pa;
    a53_u64 mm4p_mapper_page_table_ioma;
    a53_u64 mm4p_iommu_command_buffer_pa;
    a53_u64 mm4p_iommu_command_buffer_size;
    a53_u8 _pad[0x78];
    mp4_iommu_map_info_t mm4p_iommu_mmio;
    mp4_iommu_map_info_t mm4p_sdma0_mmio;
    mp4_iommu_map_info_t mm4p_sdma1_mmio;
    mp4_iommu_map_info_t mm4p_sdma0_rb;
    mp4_iommu_map_info_t mm4p_sdma1_rb;
    mp4_iommu_map_info_t mm4p_g6_fix;
    mp4_iommu_map_info_t mm4p_mm_param;
    mp4_iommu_map_info_t mm4p_io_param;
    a53_u64 mm4p_mapper_private_ioma;
    a53_u64 mm4p_scf_buf_iommu_addr;
    a53_u64 mm4p_machine_part_number;
} main_mp4_param_t;

/* ---- GIC status struct ---- */
typedef struct {
    a53_u32 gs_count;
    a53_u32 gsd_igroupr[4];
    a53_u32 gsd_isenabler[8];
    a53_u32 gsd_ispendr[8];
    a53_u32 gsd_isactiver[8];
} gic_status;

typedef struct mp4_debug_status {
    a53_u64 mds_magic1;
    a53_u64 mds_vector;
    a53_u64 mds_gpr[31];
    a53_u64 mds_sp;
    a53_u64 mds_esr_el3;
    a53_u64 mds_elr_el3;
    a53_u64 mds_far_el3;
    a53_u64 mds_elr_mode;
    a53_u64 mds_current_el;
    a53_u64 mds_daif;
    a53_u64 mds_nzcv;
    a53_u64 mds_spsr;
    a53_u64 mds_esr;
    a53_u64 mds_far;
    a53_u64 mds_tpidrro_el0;
    a53_u64 mds_esr_el32;
    a53_u64 mds_158;
    a53_u64 mds_160;
    a53_u64 mds_168;
    a53_u64 mds_170;
    a53_u64 mds_178;
    a53_u64 mds_1st_vector;
    a53_u64 mds_1st_el;
    a53_u64 mds_1st_spsr;
    a53_u64 mds_1st_esr;
    a53_u64 mds_1st_elr;
    a53_u64 mds_1st_x0;
    a53_u64 mds_1st_x1;
    a53_u64 mds_218;
    a53_u64 mds_magic2;
    a53_u64 mds_self_size;
    a53_u64 mds_id;
    a53_u64 mds_version;
    a53_u64 mds_mbox_t2c_count;
    a53_u64 mds_mbox_t2c;
    a53_u64 mds_ttyp_buffer_offset;
    a53_u64 mds_ttyp_buffer_size;
    a53_u64 mds_ttyp_buffer_last;
    a53_u64 mds_ttyp_buffer_count;
    a53_u64 mds_phase;
    a53_u64 mds_magic3;
} mp4_debug_status_t;

typedef struct sttyp_putchar_context sttyp_putchar_context_t;
typedef int (*sttyp_putchar_begin_t)(sttyp_putchar_context_t *context);
typedef int (*sttyp_putchar_char_t)(sttyp_putchar_context_t *context, int character);
typedef int (*sttyp_putchar_end_t)(sttyp_putchar_context_t *context);

struct sttyp_putchar_context {
    a53_u32 spc_cpu;
    char *spc_buf;
    a53_u64 spc_size;
    a53_u64 spc_count;
    mp4_debug_status_t *spc_mds_el0;
    a53_u8 *spc_pericom;
    sttyp_putchar_begin_t spc_begin;
    sttyp_putchar_char_t spc_putchar;
    sttyp_putchar_end_t spc_end;
};

typedef struct dev_context {
    mp4_debug_status_t *dc_debug_status;
    struct dev_context *dc_dev_context_el0;
    struct dev_context *dc_dev_context_el1;
    struct dev_context *dc_dev_context_el2;
    a53_u32 dc_exception_nest;
    int (*dc_putchar_low_hook)(int character);
    sttyp_putchar_context_t *dc_sttyp_putchar_context;
} dev_context_t;

_Static_assert(sizeof(mp4_debug_status_t) == 568, "DWARF layout mismatch: mp4_debug_status_t");
_Static_assert(offsetof(mp4_debug_status_t, mds_gpr) == 16, "DWARF offset mismatch: mds_gpr");
_Static_assert(offsetof(mp4_debug_status_t, mds_ttyp_buffer_offset) == 520, "DWARF offset mismatch: ttyp offset");
_Static_assert(sizeof(sttyp_putchar_context_t) == 72, "DWARF layout mismatch: sttyp_putchar_context_t");
_Static_assert(offsetof(sttyp_putchar_context_t, spc_pericom) == 40, "DWARF offset mismatch: spc_pericom");
_Static_assert(sizeof(dev_context_t) == 56, "DWARF layout mismatch: dev_context_t");
_Static_assert(offsetof(dev_context_t, dc_putchar_low_hook) == 40, "DWARF offset mismatch: dc_putchar_low_hook");

dev_context_t *get_dev_context(void);
int spc_begin(sttyp_putchar_context_t *context);
int spc_putchar(sttyp_putchar_context_t *context, int character);
int spc_end(sttyp_putchar_context_t *context);

int putchar(int c);
sttyp_putchar_context_t *sttyp_putchar_context_get(void);
void set_sttyp_putchar_context(sttyp_putchar_context_t *spc);
int putchar_sttyp_end(void);

void aarch64_DC_CVAC_range_bs(void *base, a53_u64 length);

a53_u32 mp4_get_cpu(void);
char *mp4_basename(char *f);

void pericom_putchar(a53_u8 *base, int c);
int putchar_titania_uart_el3(int c);

void aarch64_DC_CVAC_range(void *base, a53_u64 vsize);
a53_u32 dvm_read_mailbox(a53_u32 no);
int dvm_init(void);
int gic_v2m_init(void);
a53_u32 mp4_timer_get_cnt(a53_u32 id);
int mp4_timer_init(void);

int putchar_low(int c);
int putchar_cp(int c);
int printf_low(char *format, ...);
int printf_cp(char *format, ...);
int arm_timer_init(void);
int cp_param_check(a53_u32 bit);
int cp_param_init(void);
void el3_assert(char *file, char *func, a53_u32 line, int c, char *cstr);

typedef struct aarch64_frame {
    a53_u64 af_pc;
    a53_u64 af_fp;
    a53_u64 af_sp;
} aarch64_frame_t;

mp4_debug_status_t *mp4_debug_status_get(void);
a53_u64 mp4_debug_status_get_reg(a53_u32 regid);
int mp4_debug_status_get_frame(aarch64_frame_t *af);
void mp4_debug_status_show(void);
int mp4_debug_status_putchar(int c);
void mp4_debug_status_init(void);
void mp4_debug_status_exit(void);

int write_EL3(char *msg, a53_u64 len);
int puts_EL3(char *msg);
int putchar_pericom(int c);

int printf_sttyp(char *format, ...);
int printf_titania_uart_el0(char *format, ...);

void aarch64_print_CurrentEL(void);
void aarch64_print_SPSR_EL1(void);
void aarch64_print_SPSR_EL2(void);
void aarch64_print_SPSR_EL3(void);
void aarch64_print_ESR_EL1(void);
void aarch64_print_ESR_EL2(void);
void aarch64_print_ESR_EL3(void);
void aarch64_print_ISS_instruction_abort(a53_u32 iss);
void aarch64_print_HCR_EL2(void);
void aarch64_print_SCR_EL3(void);
void aarch64_print_SCTLR_EL1(void);
void aarch64_print_SCTLR_EL2(void);
void aarch64_print_SCTLR_EL3(void);
a53_u64 aarch64_read_ELR(void);
a53_u64 aarch64_read_ESR(void);
a53_u64 aarch64_read_FAR(void);
a53_u64 aarch64_address_translation_read(void *va);
a53_u64 aarch64_address_translation_write(void *va);
void aarch64_ccahe_op_init(void);
int aarch64_CISW_all(void);

int svc_EL3(a53_u32 esr_el1, mp4_debug_status_t *status);

int smnif_init(void);

a53_u64 c2pmsg_addr64(a53_u32 ui);
a53_u32 c2pmsg_read(a53_u32 no);
void c2pmsg_write(a53_u32 no, a53_u32 data);
void c2pmsg_init(void);

void msi_write_scratch0(a53_u32 v);
void msi_write_scratch1(a53_u32 v);
void msi_write_scratch2(a53_u32 v);
void msi_write_scratch3(a53_u32 v);
void msi_write_p2c_command(a53_u32 core, a53_u32 v);
a53_u32 msi_read_c2p_command(a53_u32 core);
a53_u32 msi_read_c2p_arg1(a53_u32 core);
void msi_write_c2p_ack(a53_u32 core, a53_u32 command);
int msi_send_command_nosync(a53_u32 core, a53_u32 command0);
int msi_send_command_sync(a53_u32 core, a53_u32 command);
a53_u32 msi_get_prev(a53_u32 core);
a53_u32 msi_read_p2c_ack(a53_u32 core);
int msi_send_write_sig(a53_u32 core, a53_u32 arg1, a53_u32 arg2, a53_u32 arg3, a53_u32 hint0);
int msi_has_internal_qaf(void);
a53_u32 msi_get_msi_address32(void);
a53_u32 msi_get_msi_offset(void);
a53_u16 msi_get_vector_for_mm(void);
a53_u16 msi_get_vector_for_io(void);
a53_u16 msi_get_vector_1st_core(void);
a53_u16 msi_get_vector_2nd_core(void);
main_mp4_param_t *msi_get_main_param(void);
void mp4_iommu_map_info_print(mp4_iommu_map_info_t *mimi, char *member);
int msi_get_info_by_1st_core(void);
int msi_get_info_by_2nd_core(void);
int msi_wait_command(a53_u32 cpu);
int msi_test(void);

int dev_pmu_init(void);
int dev_pmu_setup_default(void);
int dev_pmu_report(void);

a53_u32 gic_read_GICC_IAR(void);
void gic_write_GICC_EOIR(a53_u32 v);
a53_u32 gic_read_GICC_RPR(void);
a53_u32 gic_read_GICC_HPPIR(void);
int gic_check(void);
int gic_init_by_1st_core(void);
int gic_init_by_2nd_core(void);
void gic_sgi1(void);
void gic_core1(void);
a53_u64 aarch64_read_DAIF(void);

void writel(a53_u64 addr, a53_u32 val);
a53_u32 readl(a53_u64 addr);
int syshub_init(void);
void mp4_iommu_map_info_printf(mp4_iommu_map_info_t *mimi, char *name);
int syshub_init_after_main_param(void);
int syshub_init_sdma(void);
int syshub_init_for_io(a53_u64 base);
void syshub_tlb_get(a53_u32 tlb, a53_u32 *tlb0, a53_u32 *tlb1, a53_u32 *tlb2, a53_u32 *tlb3, a53_u32 *sub, a53_u32 *attr1);

extern a53_u32 g_L1D_NumSets;
extern a53_u32 g_L1D_Associativity;
extern a53_u32 g_L1I_NumSets;
extern a53_u32 g_L1I_Associativity;
extern a53_u32 g_L2D_NumSets;
extern a53_u32 g_L2D_Associativity;

/* =========================================================================
 * MMU types
 * ========================================================================= */

typedef a53_u64 pte_t;

typedef enum {
    map_mode_rw_rw = 0,
    map_mode_rx_rx = 1,
    map_mode_ro    = 2,
    map_mode_rw    = 3
} mmu_map_mode_t;

typedef enum {
    mem_type_memory = 0,
    mem_type_so     = 1
} mmu_mem_type_t;

typedef enum {
    mmu_op_map   = 0,
    mmu_op_unmap = 1
} mmu_op_t;

typedef enum {
    mmu_access_read_ok      = 0,
    mmu_access_read_ng      = 1,
    mmu_access_write_ok     = 2,
    mmu_access_write_ng     = 3,
    mmu_access_el0_read_ok  = 4,
    mmu_access_el0_read_ng  = 5,
    mmu_access_el0_write_ok = 6,
    mmu_access_el0_write_ng = 7
} mmu_access_check_t;

typedef struct mmu_page_table {
    a53_u8  mpt_el;
    a53_u8  mpt_level;
    a53_u64 mpt_vbase;
    pte_t  *mpt_table;
    a53_u64 mpt_table_pbase;
} mmu_page_table_t;

typedef struct mmu_page_table_mgr {
    mmu_page_table_t *mptm_mpt;
    a53_u32           mptm_mpt_count;
    a53_u32           mptm_mpt_max;
    a53_u32           mptm_table_count;
    a53_u32           mptm_table_max;
    pte_t            *mptm_ptr_cur;
    pte_t            *mptm_ptr_begin;
    pte_t            *mptm_ptr_end;
} mmu_page_table_mgr_t;

typedef struct mmu_el0_common {
    mmu_page_table_t *mec_level1_00000000;
    mmu_page_table_t *mec_level2_00000000;
    mmu_page_table_t *mec_level3_00000000;
    mmu_page_table_t *mec_level3_02000000;
    mmu_page_table_t *mec_level3_02200000;
    mmu_page_table_t *mec_level3_03000000;
} mmu_el0_common_t;

typedef struct mmu_el0_mm {
    mmu_el0_common_t  mem_mec;
    mmu_page_table_t *mem_level3_03200000;
    mmu_page_table_t *mem_level3_03400000;
    mmu_page_table_t *mem_level3_03A00000;
    mmu_page_table_t *mem_level3_04000000;
    mmu_page_table_t *mem_level3_04800000;
    mmu_page_table_t *mem_level3_04C00000;
    mmu_page_table_t *mem_level3_04E00000;
    mmu_page_table_t *mem_level3_05000000;
} mmu_el0_mm_t;

typedef struct mmu_el0_io {
    mmu_el0_common_t  mei_mec;
    mmu_page_table_t *mei_level2_40000000;
    mmu_page_table_t *mei_level3_03600000;
    mmu_page_table_t *mei_level3_03800000;
    mmu_page_table_t *mei_level3_03C00000;
    mmu_page_table_t *mei_level3_04200000;
    mmu_page_table_t *mei_level3_04400000;
    mmu_page_table_t *mei_level3_04A00000;
    mmu_page_table_t *mei_level3_04E00000;
    mmu_page_table_t *mei_level3_05200000;
} mmu_el0_io_t;

/* =========================================================================
 * Boot / layout types
 * ========================================================================= */

typedef int (*printf_func_t)(char *format, ...);

typedef struct el3_jmp_buf el3_jmp_buf;

typedef struct layout_section {
    a53_u64 msi_begin;
    a53_u64 msi_end;
    a53_u64 msi_page_size;
    a53_u64 msi_sram;
    a53_u64 msi_pa;
} layout_section_t;

typedef struct layout {
    layout_section_t msl_loader_el3_text;
    layout_section_t msl_loader_el3_data;
    layout_section_t msl_loader_dev_text;
    layout_section_t msl_loader_dev_data;
    layout_section_t msl_loader_el2;
    layout_section_t msl_loader_el1;
    layout_section_t msl_controller_dev_text;
    layout_section_t msl_controller_dev_data;
} layout_t;

typedef struct el3_param {
    void            *core0_main;
    dev_context_t   *core0_dev_context;
    a53_u64         *log0;
    void            *core1_main;
    dev_context_t   *core1_dev_context;
    a53_u64         *log1;
} el3_param_t;

typedef union {
    a53_u64 asU64[16];
} boot_config_field_0_t;

typedef struct IoController_BootConfiguration {
    boot_config_field_0_t field_0;
} IoController_BootConfiguration;

typedef struct MmController_BootConfiguration {
    boot_config_field_0_t field_0;
} MmController_BootConfiguration;

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

typedef struct deci_target_ch_fix {
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
} deci_target_ch_fix_t;

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

/* Vtable for DECI target metadata */
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

/* =========================================================================
 * Boot config function prototypes
 * ========================================================================= */

IoController_BootConfiguration *IoController_BootConfiguration_get(void);
MmController_BootConfiguration *MmController_BootConfiguration_get(void);
int boot_config_io_io(IoController_BootConfiguration *dst, IoController_BootConfiguration *src);
a53_u64 bits_27_12(a53_u64 v);
a53_u64 bits_18_12(a53_u64 v);
int boot_config_dram_entry(IoController_BootConfiguration *dst, a53_u64 addr0, a53_u64 newbase);
int boot_config_mm_phase1(void);
int boot_config_io_phase1(void);
int boot_config_mm_phase2(a53_u8 **out);
int boot_config_io_phase2(a53_u8 **out);

/* =========================================================================
 * MMU function prototypes
 * ========================================================================= */

a53_u64 mmu_va_to_pa(void *va);
int mmu_page_table_init_table(mmu_page_table_t *mpt);
int mmu_page_table_init(mmu_page_table_t *mpt, a53_u8 el, a53_u8 level,
    a53_u64 vaddr, pte_t *pte0);
a53_u64 mmu_page_table_get_entry_size(mmu_page_table_t *mpt);
a53_u64 mmu_page_table_get_vbase(mmu_page_table_t *mpt, a53_u64 va);
pte_t *mmu_page_table_get_ppte(mmu_page_table_t *mpt, a53_u64 va);
int mmu_page_table_set_table(mmu_page_table_t *mpt, mmu_page_table_t *link_mpt);
int mmu_page_table_check_be(mmu_page_table_t *mpt, a53_u64 begin, a53_u64 end);
int mmu_page_table_sync_all(mmu_page_table_t *mpt);
int mmu_page_table_access_check_read(mmu_page_table_t *mpt, a53_u64 va, a53_u64 *pa);
int mmu_page_table_access_check_write(mmu_page_table_t *mpt, a53_u64 va, a53_u64 *pa);
int mmu_page_table_access_check_el0_read(mmu_page_table_t *mpt, a53_u64 va, a53_u64 *pa);
int mmu_page_table_access_check_el0_write(mmu_page_table_t *mpt, a53_u64 va, a53_u64 *pa);
int mmu_page_table_access_check_range_be(mmu_page_table_t *mpt, a53_u64 vbegin,
    a53_u64 vend, mmu_access_check_t type);
int mmu_page_table_op_range_be(mmu_page_table_t *mpt, mmu_op_t op,
    a53_u64 vbegin, a53_u64 pbegin, a53_u64 vend,
    mmu_map_mode_t mode, mmu_mem_type_t mem);
int mmu_page_table_map_range_be(mmu_page_table_t *mpt, a53_u64 vbegin,
    a53_u64 pbegin, a53_u64 vend, mmu_map_mode_t mode, mmu_mem_type_t mem);
int mmu_page_table_map_range_bs(mmu_page_table_t *mpt, a53_u64 vbegin,
    a53_u64 pbegin, a53_u64 vsize, mmu_map_mode_t mode, mmu_mem_type_t mem);
void mmu_page_table_show_map(a53_u32 el, a53_u64 map_va, a53_u64 map_size,
    a53_u64 map_pa0, a53_u64 map_pte, a53_u64 pte_vsize);
int mmu_page_table_get_map(mmu_page_table_t *mpt);
mmu_page_table_t *mmu_page_table_mgr_alloc(a53_u8 el, a53_u8 level, a53_u64 va);
void mmu_init_phase1(void);
void mmu_init_phase2a(void);
void mmu_init_phase2b(void);
void mmu_init_phase3(void);
void mmu_init_phase4(void);
int mmu_el3_level2_0_unmap_range_be(a53_u64 vbegin, a53_u64 vend);
int mmu_el3_level2_0_op_range_be(a53_u64 vbegin, mmu_op_t op,
    a53_u64 pbegin, a53_u64 vend, mmu_map_mode_t mode, mmu_mem_type_t mem);

/* =========================================================================
 * EL0 support function prototypes
 * ========================================================================= */

int el0_support(el3_param_t *param, a53_u32 cp_param2);
a53_u8 *el0_va_to_el3_va(a53_u8 *el0_va0);
int mmu_el0_common_phase1(mmu_el0_common_t *mec);
int mmu_el0_common_phase2(mmu_el0_common_t *mec);
int mmu_el0_common_get_map_low(mmu_el0_common_t *mec);
int mmu_el0_common_get_map_high(mmu_el0_common_t *mec);

/* =========================================================================
 * Boot function prototypes
 * ========================================================================= */

void el3_print_common(void);
void *el3_jmpbuf_enter(el3_jmp_buf *n);
void el3_jmpbuf_exit(el3_jmp_buf *n);
int el3_boot(a53_u64 *log, printf_func_t printf_func, a53_u32 cp_param2);
void el3_serror_handler(a53_u64 x0, a53_u64 vector);

/* =========================================================================
 * DECI5S channel fix (used by deci5s_mp4.c and deci_target_mp4.c)
 * ========================================================================= */

int deci5s_ch_fix_send_request(deci5s_ch_fix_t *d5cf, a53_u32 psize, a53_u32 hint);

/* =========================================================================
 * Additional aarch64 types and prototypes
 * ========================================================================= */

typedef struct {
    a53_u32 bit;
    a53_u32 mask;
    const char *name;
} el3_reg_bit_name32;

typedef enum {
    cache_op_isw = 0,
    cache_op_csw = 1,
} cache_op_sw_type_t;

void el3_reg_bit_name32_print(const el3_reg_bit_name32 *p, a53_u32 v);
int aarch64_cache_op_set_way(cache_op_sw_type_t op, a53_u32 level);

/* =========================================================================
 * GIC additional prototypes
 * ========================================================================= */

a53_u32 gic_enable_irq(a53_u32 irq, a53_u32 cpu);
int gic_print_reg(char *name, a53_u32 v);
int gic_print_gicd_reg(char *name, a53_u32 offset);
int gic_print_gicc_reg(char *name, a53_u32 offset);
a53_u32 gic_status_check(gic_status *gs);

/* =========================================================================
 * putchar additional prototypes
 * ========================================================================= */

int putchar_el0_direct(int c);
int putchar_sttyp(int c);
int putchar_sttyp_begin(void);
int putchar_titania_uart_el0(int c);

/* =========================================================================
 * printf hooks
 * ========================================================================= */

int putchar_sttyp_hook(void *pfd, int ch);
int putchar_titania_uart_hook_el0(void *pfd, int ch);
int putchar_low_hook(void *pfd, int ch);
int putchar_cp_hook(void *pfd, int ch);

/* =========================================================================
 * dev_context_init
 * ========================================================================= */

int dev_context_init(dev_context_t *dc, sttyp_putchar_context_t *spc,
    a53_u32 cpu, a53_u32 cp_param2,
    char *buf, a53_u64 len, mp4_debug_status_t *mds);

/* =========================================================================
 * PMU additional prototypes
 * ========================================================================= */

void pmu_print_count(a53_u32 type, a53_u32 count);

/* =========================================================================
 * puts
 * ========================================================================= */

int puts(char *s);

/* =========================================================================
 * check_consistency
 * ========================================================================= */

int check_consistency(void);

/* =========================================================================
 * mp4_debug_status_c_set
 * ========================================================================= */

void mp4_debug_status_c_set(void);

#endif
