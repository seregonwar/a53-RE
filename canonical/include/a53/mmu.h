#ifndef A53_MMU_H
#define A53_MMU_H

#include "a53_abi.h"

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

/* ---- Boot config types (used by mmu) ---- */
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

int el0_support(struct el3_param *param, a53_u32 cp_param2);
a53_u8 *el0_va_to_el3_va(a53_u8 *el0_va0);
int mmu_el0_common_phase1(mmu_el0_common_t *mec);
int mmu_el0_common_phase2(mmu_el0_common_t *mec);
int mmu_el0_common_get_map_low(mmu_el0_common_t *mec);
int mmu_el0_common_get_map_high(mmu_el0_common_t *mec);

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

#endif
