#ifndef POC_H
#define POC_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* ---- PoC Command Codes (mirror from a53/poc_bridge.h) ---- */
#define POC_CMD_PEEK_MEM        1
#define POC_CMD_POKE_MEM        2
#define POC_CMD_READ_SYSREG     3
#define POC_CMD_WRITE_SYSREG    4
#define POC_CMD_GET_TLB         5
#define POC_CMD_SET_TLB         6
#define POC_CMD_VA_TO_PA        7
#define POC_CMD_EL0_VA_TO_PA    8
#define POC_CMD_GET_MAIN_PARAM  9
#define POC_CMD_GET_DEBUG_STAT  10
#define POC_CMD_GET_PAGE_TABLE  11
#define POC_CMD_MAP_PAGE        12
#define POC_CMD_TLBI_SYNC       13
#define POC_CMD_CACHE_CLEAN     14
#define POC_CMD_READ_MSI_PARAM  15

/* System register IDs */
#define POC_REG_SCTLR_EL1   0x0101
#define POC_REG_SCTLR_EL2   0x0202
#define POC_REG_SCTLR_EL3   0x0303
#define POC_REG_SCR_EL3     0x0304
#define POC_REG_HCR_EL2     0x0205
#define POC_REG_TCR_EL1     0x0106
#define POC_REG_TTBR0_EL1   0x0107
#define POC_REG_TTBR0_EL3   0x0308
#define POC_REG_VBAR_EL1    0x0109
#define POC_REG_VBAR_EL2    0x020a
#define POC_REG_VBAR_EL3    0x030b
#define POC_REG_MAIR_EL1    0x010c
#define POC_REG_MAIR_EL3    0x030d
#define POC_REG_SPSR_EL3    0x030e
#define POC_REG_ELR_EL3     0x030f
#define POC_REG_DAIF        0x0010
#define POC_REG_PMCR_EL0    0x0011
#define POC_REG_MDCR_EL2    0x0212
#define POC_REG_MDCR_EL3    0x0313
#define POC_REG_CPACR_EL1   0x0114
#define POC_REG_CPUECTLR_EL1 0x0115

/* ---- Debug Status mailbox layout ----
 *
 * This is the DWARF-verified mp4_debug_status_t layout recovered in
 * canonical/include/a53/loader.h.  Keep the field order and the assertions
 * below in sync with that source: this structure is used only to decode a
 * read-only snapshot on the x86_64 payload path.
 */
typedef struct {
    uint64_t mds_magic1;        /* offset 0x000 */
    uint64_t mds_vector;        /* offset 0x008 */
    uint64_t mds_gpr[31];       /* offset 0x010: GPR x0-x30 */
    uint64_t mds_sp;            /* offset 0x108 */
    uint64_t mds_esr_el3;       /* offset 0x110 */
    uint64_t mds_elr_el3;       /* offset 0x118 */
    uint64_t mds_far_el3;       /* offset 0x120 */
    uint64_t mds_elr_mode;      /* offset 0x128 */
    uint64_t mds_current_el;    /* offset 0x130 */
    uint64_t mds_daif;          /* offset 0x138 */
    uint64_t mds_nzcv;          /* offset 0x140 */
    uint64_t mds_spsr;          /* offset 0x148 */
    uint64_t mds_esr;           /* offset 0x150 */
    uint64_t mds_far;           /* offset 0x158 */
    uint64_t mds_tpidrro_el0;   /* offset 0x160 */
    uint64_t mds_esr_el32;      /* offset 0x168 */
    uint64_t mds_158;           /* offset 0x170 */
    uint64_t mds_160;           /* offset 0x178 */
    uint64_t mds_168;           /* offset 0x180 */
    uint64_t mds_170;           /* offset 0x188 */
    uint64_t mds_178;           /* offset 0x190 */
    uint64_t mds_1st_vector;    /* offset 0x198 */
    uint64_t mds_1st_el;        /* offset 0x1A0 */
    uint64_t mds_1st_spsr;      /* offset 0x1A8 */
    uint64_t mds_1st_esr;       /* offset 0x1B0 */
    uint64_t mds_1st_elr;       /* offset 0x1B8 */
    uint64_t mds_1st_x0;        /* offset 0x1C0 */
    uint64_t mds_1st_x1;        /* offset 0x1C8 */
    uint64_t mds_218;           /* offset 0x1D0 */
    uint64_t mds_magic2;        /* offset 0x1D8 */
    uint64_t mds_self_size;     /* offset 0x1E0 */
    uint64_t mds_id;            /* offset 0x1E8 */
    uint64_t mds_version;       /* offset 0x1F0 */
    uint64_t mds_mbox_t2c_count;/* offset 0x1F8 */
    uint64_t mds_mbox_t2c;      /* offset 0x200 */
    uint64_t mds_ttyp_buffer_offset;  /* offset 0x208 */
    uint64_t mds_ttyp_buffer_size;    /* offset 0x210 */
    uint64_t mds_ttyp_buffer_last;    /* offset 0x218 */
    uint64_t mds_ttyp_buffer_count;   /* offset 0x220 */
    uint64_t mds_phase;         /* offset 0x228 */
    uint64_t mds_magic3;        /* offset 0x230 */
} poc_mailbox_t;

_Static_assert(sizeof(poc_mailbox_t) == 0x238,
               "mp4_debug_status_t size mismatch");
_Static_assert(offsetof(poc_mailbox_t, mds_gpr) == 0x10,
               "mp4_debug_status_t GPR offset mismatch");
_Static_assert(offsetof(poc_mailbox_t, mds_magic2) == 0x1d8,
               "mp4_debug_status_t magic2 offset mismatch");
_Static_assert(offsetof(poc_mailbox_t, mds_magic3) == 0x230,
               "mp4_debug_status_t magic3 offset mismatch");

/* ---- API Functions ---- */

typedef enum {
    POC_BRIDGE_UNINITIALIZED = 0,
    POC_BRIDGE_TRANSPORT_READY,
    POC_BRIDGE_SNAPSHOT_ONLY,
    POC_BRIDGE_DISCOVERY_FAILED,
    POC_BRIDGE_UNSUPPORTED,
} poc_bridge_state_t;

/* Initialize the PoC bridge. Returns 0 only when a command transport exists. */
int poc_init(void);

/* Report whether the target has a real command transport or only a snapshot. */
poc_bridge_state_t poc_bridge_state(void);

/* Read a physical memory location. Returns 0xFFFFFFFFFFFFFFFF on error. */
uint64_t poc_peek(uint64_t phys_addr);

/* Write a value to physical memory. Returns 0 on success, 1 if blocked. */
int poc_poke(uint64_t phys_addr, uint64_t value);

/* Read a system register. Returns 0xFFFFFFFFFFFFFFFF on error. */
uint64_t poc_read_sysreg(uint32_t reg_id);

/* Write a system register. Returns 0 on success. */
int poc_write_sysreg(uint32_t reg_id, uint64_t value);

/* Read an IOMMU/Syshub TLB entry. Output pointers receive the 4 TLB words. */
int poc_get_tlb(uint32_t tlb_index,
                uint32_t *tlb0, uint32_t *tlb1,
                uint32_t *tlb2, uint32_t *tlb3,
                uint32_t *sub, uint32_t *attr1);

/* Write an IOMMU/Syshub TLB entry.
 *
 * Encoding contract (must match EL3 dispatcher table):
 *   seg_size  -> TLB0[31:24] page-size map:
 *                0x01=4K, 0x02=8K, 0x03=16K, 0x04=64K, 0x05=2M,
 *                0x06=4M, 0x07=8M, 0x08=16M, 0x09=32M, 0x0A=64M,
 *                0x0B=128M, 0x0C=256M, 0x0D=512M, 0x0E=1G, 0x0F=2G
 *   attr      -> TLB0[7:0] (syshub-specific attributes)
 *   phys_addr -> TLB1 (PA[39:32]) + TLB2 (PA[31:0])
 */
int poc_set_tlb(uint32_t tlb_index, uint64_t phys_addr,
                uint32_t seg_size, uint32_t attr);

/* Translate an EL3 VA to PA. Returns 0xFFFFFFFFFFFFFFFF on error. */
uint64_t poc_va_to_pa(void *va);

/* Translate an EL0 VA to PA (AT S1E0R). */
uint64_t poc_el0_va_to_pa(uint64_t el0_va);

/* Get the physical address of the main parameter block. */
uint64_t poc_get_main_param_base(void);

/* Get the physical address of the debug status mailbox. */
uint64_t poc_get_debug_status_base(void);

/* Get page-table bases (VBAR_EL3, TTBR0_EL3, TTBR0_EL1). */
int poc_get_page_tables(uint64_t *vbar_el3, uint64_t *ttbr0_el3,
                        uint64_t *ttbr0_el1);

/* Invalidate all TLBs (EL1 + EL3) and synchronize. */
int poc_tlbi_sync(void);

/* Clean a VA range from data cache. */
int poc_cache_clean(void *va, uint64_t len);

/* Read a field from the main parameter block at a given offset. */
uint64_t poc_read_msi_param(uint32_t offset);

/* Write a field to the main parameter block (volatile, SRAM-based). */
int poc_write_msi_param(uint32_t offset, uint64_t value);

/* Map modes for poc_map_page (only these are accepted). */
#define POC_MAP_RO 0
#define POC_MAP_RW 1

/* ---- "Unsupported on this build" sentinels ----
 *
 * These are the canonical values returned by every PoC API when the AArch64
 * EL3 dispatcher isn't reachable — i.e. on any target where poc_init() is
 * unable to verify the debug-status mailbox (currently: PS5 main CPU at
 * x86_64-sie-ps5; future: any non-AArch64 host).
 *
 * API contract:
 *   - All uint64_t-returning APIs (poc_peek, poc_va_to_pa, …) return
 *     POC_UNSUPP_PA when unsupported.
 *   - All uint32_t TLB words return POC_UNSUPP_TLB.
 *   - All int-returning APIs return POC_UNSUPP_INT.
 *
 * Callers should treat *any* sentinel value as "API not functional on this
 * build target" and route the operation to a no-op or surface the
 * condition via /data/poc/<file>.txt for offline analysis.
 */
#define POC_UNSUPP_PA  0xFFFFFFFFFFFFFFFFULL
#define POC_UNSUPP_TLB 0xFFFFFFFFU
#define POC_UNSUPP_INT (-1)

/* Map a 4K EL3 page (volatile — lost on reset). mode must be POC_MAP_RO or POC_MAP_RW. */
int poc_map_page(uint64_t va, uint64_t pa, int mode);

/* Get direct MMIO pointer to the debug status mailbox */
poc_mailbox_t *poc_get_mailbox(void);

#ifdef __cplusplus
}
#endif
#endif /* POC_H */
