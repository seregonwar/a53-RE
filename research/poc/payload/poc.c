#include "poc.h"
#include "pal_privilege.h"
#include <stddef.h>  /* NULL */
#include <stdbool.h>
#include <string.h>

/* "Unsupported on this build" sentinels (POC_UNSUPP_PA/TLB/INT) live in
 * poc.h so any caller — current code, future code, host test harness —
 * sees the same canonical values regardless of which .c file defines them.
 * Do NOT redefine them here. */

/* Architecture dispatch.
 *
 * On AArch64 (the A53 EL0/EL1 path from the A53 PoC bridge design):
 *   - Use dmb sy + svc #0x152 + dmb to talk to the EL3 SVC handler.
 *   - Wire the ps5-payload-sdk style SVC dispatcher via mds_gpr[0..5].
 *
 * On x86_64 (the PS5 main CPU; SDK target x86_64-sie-ps5):
 *   - There is NO direct A53 mailbox access from the main CPU at EL0.
 *   - We implement a write-only no-op that lets the build pipeline work
 *     and lets /data/poc + notifications still execute (for smoke-test).
 *   - Returns sentinel values indicating "no EL3 dispatcher available"
 *     so callers can detect unsupported state.
 *
 * The #elif branch is picked at compile time via predefined macros
 * (__aarch64__, __x86_64__). The x86_64 fallback is intentional and
 * documented; do NOT remove unless an x86→A53 bridge syscall exists.
 */

/* Local read-only snapshot used only by the x86_64 discovery path.  A copied
 * structure never becomes a command mailbox: no x86_64-to-A53 transport is
 * implied by a successful physical read. */
#if defined(__x86_64__) || defined(__amd64__)
static poc_mailbox_t g_local_mailbox;
#endif
static poc_bridge_state_t g_bridge_state = POC_BRIDGE_UNINITIALIZED;

/* The debug status mailbox is mapped at EL0 VA 0x3A00000 (core0) or 0x3C00000 (core1).
 * We use a volatile pointer to the mailbox structure.
 * Access is validated by reading the magic fields in poc_init(). */
static poc_mailbox_t *g_mailbox = (poc_mailbox_t *)0x3A00000ULL;

/* Trigger SVC #0x152. Wire format:
 *   mds_gpr[0] = cmd                     (input + output)
 *   mds_gpr[1..5] = arg1..arg5          (input)
 *   mds_gpr[2] MUST be non-zero         (PoC dispatcher trigger flag)
 *
 * After return, mds_gpr[0] contains the command result.
 *
 * IMPORTANT: SVC from EL0 traps to EL1 first. The PS5 kernel must forward
 * it to EL3, or the payload must run at EL1. As an alternative, commands
 * can be sent by writing directly to the mailbox and triggering an IRQ.
 *
 * SAFETY: This SVC has a well-defined ABI — it only accesses the mailbox
 * and performs validated operations on volatile hardware state.
 */
/* Internal SVC dispatch. Returns the full 64-bit value placed by the
 * EL3 dispatcher in mds_gpr[0]. Commands returning a status code (0/1/-1)
 * fit cleanly into int, but PA-style returns (peek, get_main_param_base)
 * use the full 64 bits without truncation. */
static uint64_t poc_svc(uint64_t cmd,
                        uint64_t arg1, uint64_t arg2, uint64_t arg3,
                        uint64_t arg4, uint64_t arg5)
{
    g_mailbox->mds_gpr[0] = cmd;
    g_mailbox->mds_gpr[1] = arg1;
    g_mailbox->mds_gpr[2] = arg2;  /* dispatcher flag (non-zero = PoC) */
    g_mailbox->mds_gpr[3] = arg3;
    g_mailbox->mds_gpr[4] = arg4;
    g_mailbox->mds_gpr[5] = arg5;

#if defined(__aarch64__)
    /* ARM path: data-memory barrier, SVC #0x152, barrier again. */
    __asm__ volatile("dmb sy" ::: "memory");
    __asm__ volatile("svc #0x152" ::: "memory");
    __asm__ volatile("dmb sy" ::: "memory");
    return g_mailbox->mds_gpr[0];

#elif defined(__x86_64__) || defined(__amd64__)
    /* x86_64 fallback: write mailbox + mfence; no EL3 dispatcher reachable
     * from PS5 main CPU. Returns POC_UNSUPP_PA so callers surface
     * "unsupported on this build" via their sentinel checks. /data/poc +
     * notifications still operate so the chain produces analysis output.
     *
     * Note: clang+clang-cl define both __x86_64__ and __amd64__ on x86_64
     * targets; GCC defines only __x86_64__. The OR-across-both pattern is
     * intentional — don't strip either branch as "duplicate". */
    __asm__ volatile("mfence" ::: "memory");
    return POC_UNSUPP_PA;

#else
    /* Unknown arch: portable volatile read, no barrier, no syscall. */
    __asm__ volatile("" ::: "memory");
    return g_mailbox->mds_gpr[0];
#endif
}

/* ---- API Implementation ---- */

int poc_init(void)
{
#if defined(__x86_64__) || defined(__amd64__)
    /* On the PS5 main CPU (x86_64), the A53 debug-status address space is
     * not directly dereferenceable from the main CPU.
     *
     * After privilege escalation (uid=0, sandbox escape, debugger authid),
     * we have kernel_copyout access to the kernel's direct physical map
     * (DMAP). We scan physical memory for the known debug status magic
     * values, which works regardless of firmware version or SRAM address. */
    uint64_t ds_pa;

    g_mailbox = NULL;

    /* Only attempt scanning if privilege escalation was successful.
     * The DMAP probe happens lazily inside poc_privilege_scan_debug_status(). */
    if (!poc_privilege_supported()) {
        g_bridge_state = POC_BRIDGE_UNSUPPORTED;
        return POC_UNSUPP_INT;
    }

    ds_pa = poc_privilege_scan_debug_status();
    if (ds_pa == 0) {
        /* Not found in any accessible range. The A53 SRAM might not be
         * mapped in the kernel's DMAP, or this firmware uses different
         * magic values. Fall back gracefully. */
        g_bridge_state = POC_BRIDGE_DISCOVERY_FAILED;
        return POC_UNSUPP_INT;
    }

    /* Read the full structure from physical memory into our local buffer */
    memset(&g_local_mailbox, 0, sizeof(g_local_mailbox));
    if (poc_privilege_read_debug_status(ds_pa, &g_local_mailbox) != 0) {
        g_bridge_state = POC_BRIDGE_DISCOVERY_FAILED;
        return POC_UNSUPP_INT;
    }

    /* Verify all three magic fields from the copy */
    if (g_local_mailbox.mds_magic1 == 0xcbb3d18a1aa5daefULL &&
        g_local_mailbox.mds_magic2 == 0x1aa5daef675a1801ULL &&
        g_local_mailbox.mds_magic3 == 0x8501dda72c7b400eULL) {
        /* A copied debug-status block is evidence only. It is not a mapped
         * mailbox and cannot turn an x86_64 payload into an A53 command
         * transport. Keep g_mailbox NULL so all state-changing APIs remain
         * fail-closed. */
        g_bridge_state = POC_BRIDGE_SNAPSHOT_ONLY;
        return POC_UNSUPP_INT;
    }

    g_bridge_state = POC_BRIDGE_DISCOVERY_FAILED;
    return POC_UNSUPP_INT;
#else
    /* Verify the mailbox by checking ALL THREE magic fields (fail-closed).
     * mds_magic1 @ 0x000, mds_magic2 @ 0x1D8, mds_magic3 @ 0x230.
     * A corrupt mailbox matching any one magic would still be detected. */
    if (g_mailbox->mds_magic1 == 0xcbb3d18a1aa5daefULL &&
        g_mailbox->mds_magic2 == 0x1aa5daef675a1801ULL &&
        g_mailbox->mds_magic3 == 0x8501dda72c7b400eULL) {
        g_bridge_state = POC_BRIDGE_TRANSPORT_READY;
        return 0;  /* core0 valid */
    }
    g_mailbox = (poc_mailbox_t *)0x3C00000ULL;  /* try core1 */
    if (g_mailbox->mds_magic1 == 0xcbb3d18a1aa5daefULL &&
        g_mailbox->mds_magic2 == 0x1aa5daef675a1801ULL &&
        g_mailbox->mds_magic3 == 0x8501dda72c7b400eULL) {
        g_bridge_state = POC_BRIDGE_TRANSPORT_READY;
        return 0;  /* core1 valid */
    }
    g_mailbox = NULL;
    g_bridge_state = POC_BRIDGE_DISCOVERY_FAILED;
    return POC_UNSUPP_INT;  /* both cores failed */
#endif
}

poc_bridge_state_t poc_bridge_state(void)
{
    return g_bridge_state;
}

uint64_t poc_peek(uint64_t phys_addr)
{
    if (!g_mailbox) return POC_UNSUPP_PA;
    return poc_svc(POC_CMD_PEEK_MEM, phys_addr, 1, 0, 0, 0);
}

int poc_poke(uint64_t phys_addr, uint64_t value)
{
    if (!g_mailbox) return POC_UNSUPP_INT;
    return (int)poc_svc(POC_CMD_POKE_MEM, phys_addr, value, 0, 0, 0);
}

uint64_t poc_read_sysreg(uint32_t reg_id)
{
    if (!g_mailbox) return POC_UNSUPP_PA;
    return poc_svc(POC_CMD_READ_SYSREG, reg_id, 1, 0, 0, 0);
}

int poc_write_sysreg(uint32_t reg_id, uint64_t value)
{
    if (!g_mailbox) return POC_UNSUPP_INT;
    return (int)poc_svc(POC_CMD_WRITE_SYSREG, reg_id, value, 0, 0, 0);
}

int poc_get_tlb(uint32_t tlb_index,
                uint32_t *tlb0, uint32_t *tlb1,
                uint32_t *tlb2, uint32_t *tlb3,
                uint32_t *sub, uint32_t *attr1)
{
    if (!g_mailbox) return POC_UNSUPP_INT;

    g_mailbox->mds_gpr[0] = POC_CMD_GET_TLB;
    g_mailbox->mds_gpr[1] = tlb_index;
    g_mailbox->mds_gpr[2] = 1;  /* non-zero: use PoC dispatcher */
    g_mailbox->mds_gpr[3] = 0;
    g_mailbox->mds_gpr[4] = 0;
    g_mailbox->mds_gpr[5] = 0;

#if defined(__aarch64__)
    __asm__ volatile("dmb sy" ::: "memory");
    __asm__ volatile("svc #0x152" ::: "memory");
#elif defined(__x86_64__) || defined(__amd64__)
    __asm__ volatile("mfence" ::: "memory");
#else
    __asm__ volatile("" ::: "memory");
#endif

    /* EL3 dispatcher returns TLB results in gpr[1]-gpr[6] on AArch64.
     * Output pointers may be NULL on either path; caller dispatches on
     * the int return value. */
#if defined(__aarch64__)
    if (!tlb0) ; else if (g_mailbox) *tlb0  = (uint32_t)g_mailbox->mds_gpr[1];
    if (tlb1)  *tlb1  = (uint32_t)g_mailbox->mds_gpr[2];
    if (tlb2)  *tlb2  = (uint32_t)g_mailbox->mds_gpr[3];
    if (tlb3)  *tlb3  = (uint32_t)g_mailbox->mds_gpr[4];
    if (sub)   *sub   = (uint32_t)g_mailbox->mds_gpr[5];
    if (attr1) *attr1 = (uint32_t)g_mailbox->mds_gpr[6];
    return (int)g_mailbox->mds_gpr[0];
#else
    /* Not reachable from PS5 main CPU; fill outputs with sentinel to
     * avoid silent-zero footguns. See poc_get_page_tables comment. */
    if (tlb0)  *tlb0  = POC_UNSUPP_TLB;
    if (tlb1)  *tlb1  = POC_UNSUPP_TLB;
    if (tlb2)  *tlb2  = POC_UNSUPP_TLB;
    if (tlb3)  *tlb3  = POC_UNSUPP_TLB;
    if (sub)   *sub   = POC_UNSUPP_TLB;
    if (attr1) *attr1 = POC_UNSUPP_TLB;
    return POC_UNSUPP_INT;
#endif
}

int poc_set_tlb(uint32_t tlb_index, uint64_t phys_addr,
                uint32_t seg_size, uint32_t attr)
{
    if (!g_mailbox) return POC_UNSUPP_INT;
    return (int)poc_svc(POC_CMD_SET_TLB, tlb_index, phys_addr, seg_size, attr, 1);
}

uint64_t poc_va_to_pa(void *va)
{
    if (!g_mailbox) return POC_UNSUPP_PA;
    return poc_svc(POC_CMD_VA_TO_PA, (uint64_t)va, 1, 0, 0, 0);
}

uint64_t poc_el0_va_to_pa(uint64_t el0_va)
{
    if (!g_mailbox) return POC_UNSUPP_PA;
    return poc_svc(POC_CMD_EL0_VA_TO_PA, el0_va, 1, 0, 0, 0);
}

uint64_t poc_get_main_param_base(void)
{
    if (!g_mailbox) return POC_UNSUPP_PA;
    return poc_svc(POC_CMD_GET_MAIN_PARAM, 1, 0, 0, 0, 0);
}

uint64_t poc_get_debug_status_base(void)
{
    if (!g_mailbox) return POC_UNSUPP_PA;
    return poc_svc(POC_CMD_GET_DEBUG_STAT, 1, 0, 0, 0, 0);
}

int poc_get_page_tables(uint64_t *vbar_el3, uint64_t *ttbr0_el3,
                        uint64_t *ttbr0_el1)
{
    if (!g_mailbox) return POC_UNSUPP_INT;
#if defined(__aarch64__)
    poc_svc(POC_CMD_GET_PAGE_TABLE, 1, 0, 0, 0, 0);
    if (vbar_el3)   *vbar_el3   = g_mailbox->mds_gpr[0];
    if (ttbr0_el3)  *ttbr0_el3  = g_mailbox->mds_gpr[1];
    if (ttbr0_el1)  *ttbr0_el1  = g_mailbox->mds_gpr[2];
    return 0;
#else
    /* Not reachable from PS5 main CPU; fill outputs with the same
     * sentinel (0xFFFFFFFFFFFFFFFFULL) poc_peek/poc_get_main_param_base
     * return, so a future caller that forgets to check the int return
     * still sees an obviously invalid PA rather than a silently-zeroed
     * value that could be mistaken for a real address. */
    if (vbar_el3)   *vbar_el3   = 0xFFFFFFFFFFFFFFFFULL;
    if (ttbr0_el3)  *ttbr0_el3  = 0xFFFFFFFFFFFFFFFFULL;
    if (ttbr0_el1)  *ttbr0_el1  = 0xFFFFFFFFFFFFFFFFULL;
    return -1;
#endif
}

int poc_tlbi_sync(void)
{
    if (!g_mailbox) return POC_UNSUPP_INT;
    return (int)poc_svc(POC_CMD_TLBI_SYNC, 1, 0, 0, 0, 0);
}

int poc_cache_clean(void *va, uint64_t len)
{
    if (!g_mailbox) return POC_UNSUPP_INT;
    return (int)poc_svc(POC_CMD_CACHE_CLEAN, (uint64_t)va, len, 0, 0, 0);
}

uint64_t poc_read_msi_param(uint32_t offset)
{
    if (!g_mailbox) return POC_UNSUPP_PA;
    return poc_svc(POC_CMD_READ_MSI_PARAM, offset, 1, 0, 0, 0);
}

int poc_write_msi_param(uint32_t offset, uint64_t value)
{
    uint64_t base = poc_get_main_param_base();
    if (base == 0 || base == POC_UNSUPP_PA) return POC_UNSUPP_INT;
    return poc_poke(base + offset, value);
}

int poc_map_page(uint64_t va, uint64_t pa, int mode)
{
    if (!g_mailbox) return POC_UNSUPP_INT;
    /* Validate mode: only POC_MAP_RO (0) and POC_MAP_RW (1) are supported.
     * Any other value would invoke the dispatcher with an unhandled mode. */
    if (mode != POC_MAP_RO && mode != POC_MAP_RW) return POC_UNSUPP_INT;
    uint64_t rc = poc_svc(POC_CMD_MAP_PAGE, va, pa, (uint64_t)mode, 1, 0);
    return (rc == POC_UNSUPP_PA) ? POC_UNSUPP_INT : 0;
}

poc_mailbox_t *poc_get_mailbox(void)
{
    /* Only return the pointer if init succeeded (magic verified) */
    return g_mailbox;
}
