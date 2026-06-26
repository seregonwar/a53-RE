#ifndef A53_POC_BRIDGE_H
#define A53_POC_BRIDGE_H

#include <stdint.h>

/* ---- PoC Bridge Command Codes ----
 *
 * Extended SVC protocol on imm16=0x152.
 * When mds_gpr[2] == 0: original write_EL3 behaviour (backward compatible).
 * When mds_gpr[2] != 0: command dispatcher with mds_gpr[0] = command.
 *
 * ALL operations target VOLATILE (RAM-based) state — nothing writes NVM/flash.
 * This is a development bridge, equivalent to JTAG access.
 */

enum {
    POC_CMD_NONE            = 0,   /* original write_EL3 path */
    POC_CMD_PEEK_MEM        = 1,   /* read  physical memory  */
    POC_CMD_POKE_MEM        = 2,   /* write physical memory  */
    POC_CMD_READ_SYSREG     = 3,   /* read  system register  */
    POC_CMD_WRITE_SYSREG    = 4,   /* write system register  */
    POC_CMD_GET_TLB         = 5,   /* read  IOMMU/Syshub TLB */
    POC_CMD_SET_TLB         = 6,   /* write IOMMU/Syshub TLB */
    POC_CMD_VA_TO_PA        = 7,   /* EL3 VA→PA translation  */
    POC_CMD_EL0_VA_TO_PA    = 8,   /* EL0 VA→PA translation  */
    POC_CMD_GET_MAIN_PARAM  = 9,   /* get main_mp4_param_t *  */
    POC_CMD_GET_DEBUG_STAT  = 10,  /* get mp4_debug_status_t * */
    POC_CMD_GET_PAGE_TABLE  = 11,  /* get page-table base     */
    POC_CMD_MAP_PAGE        = 12,  /* add page-table mapping  */
    POC_CMD_TLBI_SYNC       = 13,  /* TLB invalidate + DSB+ISB */
    POC_CMD_CACHE_CLEAN     = 14,  /* DC CIVAC range          */
    POC_CMD_READ_MSI_PARAM  = 15,  /* read main param field   */
    POC_CMD_MAX
};

/* System-register encoding for POC_CMD_READ_SYSREG / POC_CMD_WRITE_SYSREG.
 * Low 16 bits encode the register; upper bits encode the exception level. */
enum {
    POC_REG_SCTLR_EL1   = 0x0101,
    POC_REG_SCTLR_EL2   = 0x0202,
    POC_REG_SCTLR_EL3   = 0x0303,
    POC_REG_SCR_EL3     = 0x0304,
    POC_REG_HCR_EL2     = 0x0205,
    POC_REG_TCR_EL1     = 0x0106,
    POC_REG_TTBR0_EL1   = 0x0107,
    POC_REG_TTBR0_EL3   = 0x0308,
    POC_REG_VBAR_EL1    = 0x0109,
    POC_REG_VBAR_EL2    = 0x020a,
    POC_REG_VBAR_EL3    = 0x030b,
    POC_REG_MAIR_EL1    = 0x010c,
    POC_REG_MAIR_EL3    = 0x030d,
    POC_REG_SPSR_EL3    = 0x030e,
    POC_REG_ELR_EL3     = 0x030f,
    POC_REG_DAIF        = 0x0010,
    POC_REG_PMCR_EL0    = 0x0011,
    POC_REG_MDCR_EL2    = 0x0212,
    POC_REG_MDCR_EL3    = 0x0313,
    POC_REG_CPACR_EL1   = 0x0114,
    POC_REG_CPUECTLR_EL1= 0x0115,
};

/* TLB operation descriptor for POC_CMD_SET_TLB */
typedef struct {
    uint32_t tlb_index;
    uint64_t phys_addr;
    uint32_t seg_size;
    uint32_t attr;
} poc_tlb_op_t;

/* Page-map descriptor for POC_CMD_MAP_PAGE */
typedef struct {
    uint64_t va;
    uint64_t pa;
    uint64_t size;
    uint32_t el;        /* 1=EL1, 3=EL3 */
    uint32_t mode;      /* 0=RO, 1=RW */
    uint32_t mem_type;  /* 0=device, 1=normal */
} poc_map_op_t;

/* ---- Payload-side visible base addresses ---- */
#define POC_DEBUG_STATUS_BASE_CORE0  (0x3a00000ULL)   /* EL0 VA for 0xEC000000 */
#define POC_DEBUG_STATUS_BASE_CORE1  (0x3c00000ULL)   /* EL0 VA for 0xEC100000 */
#define POC_MAIN_PARAM_BASE_CORE0    (0x88500000ULL)  /* phys, mapped via IOMMU */

#endif /* A53_POC_BRIDGE_H */
