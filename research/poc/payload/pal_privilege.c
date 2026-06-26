/* PoC Chain — Privilege Escalation + Physical Memory Scanner (PS5 kernel helper)
 *
 * Adapted from memDBG/src/privilege/privilece.c (GPL-3.0-or-later) and
 * vda_probe.c (GPL-3.0-or-later).
 *
 * Uses kernel_copyout on the kernel's DMAP (direct physical map) to read
 * from arbitrary physical addresses after jailbreak.
 *
 * Architecture research (PSDevWiki + SDK vmparam.h):
 *   PS5 FreeBSD VM layout:
 *     0xfffff80000000000 - 0xfffffbffffffffff   4TB Direct Map (DMAP)
 *     0xfffffe0000000000 - 0xffffffffffffffff   2TB Kernel Map
 *
 * NOTE: A53 SRAM (0x88000000-0x88FFFFFF) lives on a separate internal bus
 * and is NOT mapped in the x86 CPU's DMAP. Only main GDDR6 DRAM is mapped.
 * For A53 communication, use /dev/a53mm IOCTL interface (future work).
 */

#include "pal_privilege.h"
#include "poc.h"

#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/sysctl.h>

/* Conditional compilation: PS5 (x86_64-sie-ps5) vs no-op fallback. */
#if defined(PLATFORM_PS5) || defined(PS5) || defined(__PROSPERO__)
#include <ps5/kernel.h>
#define PAL_PRIVILEGE_HAS_PS5 1
#else
#define PAL_PRIVILEGE_HAS_PS5 0
#endif

#if PAL_PRIVILEGE_HAS_PS5

/* ---- Physical memory probe & DMAP ---- */

/* Cached DMAP base after successful probe. 0 = not yet probed.
 * Set to UINTPTR_MAX after failed probe to avoid re-probing. */
static uint64_t g_dmap_base = 0;
#define DMAP_PROBE_FAILED  ((uint64_t)~0ULL)
static poc_dmap_probe_result_t g_dmap_probe_result = POC_DMAP_PROBE_NOT_RUN;
static int g_dmap_probe_rc = 0;
static int g_kernel_copyout_available = -1; /* -1=untested, 0=no, 1=yes */

/* The PS5 SDK's amd64 machine/vmparam.h documents the DMAP layout:
 *   0xfffff80000000000 – 0xfffffbffffffffff   4TB Direct Map (DMAP)
 *   0xfffffe0000000000 – 0xffffffffffffffff   2TB Kernel Map
 *
 * DMAP_MIN_ADDRESS = KVADDR(DMPML4I, 0, 0, 0) — this is a compile-time
 * constant derived from page-table level indices, NOT affected by kASLR.
 * The kernel text slides independently in the 2TB Kernel Map slot.
 */
#define DMAP_BASE_SDK 0xfffff80000000000ULL

/* ---- kernel_copyout diagnostic pre-check ----
 *
 * Before attempting DMAP probe, verify that kernel_copyout works at all
 * by reading a KNOWN kernel virtual address. The SDK exports
 * KERNEL_ADDRESS_TEXT_BASE which points to the kernel's .text section.
 * If this read fails, the issue is authid/caps/jailbreak, NOT DMAP.
 */
static int kernel_copyout_works(void)
{
    uint32_t test_word = 0;
    int rc;

    if (g_kernel_copyout_available != -1)
        return g_kernel_copyout_available;

    /* Read the first 4 bytes of kernel .text — should always return
     * a valid instruction word on any PS5 firmware. */
    rc = kernel_copyout(KERNEL_ADDRESS_TEXT_BASE, &test_word, sizeof(test_word));
    g_kernel_copyout_available = (rc == 0) ? 1 : 0;
    return g_kernel_copyout_available;
}

/* ---- Candidate DRAM physical addresses for DMAP probe ----
 *
 * On PS5 (AMD Zen 2 + GDDR6), the physical memory layout is non-standard.
 * Unlike standard x86 PCs where DRAM starts at 0x100000 (1MB), the PS5
 * reserves the low 4GB physical address space for GPU MMIO, PCIe, and
 * other on-chip peripherals. The CPU-accessible GDDR6 system RAM is at
 * a HIGH physical address (typically above 4GB or 8GB).
 *
 * We try multiple candidates to find one that is:
 *   (a) mapped in the DMAP window, AND
 *   (b) readable (returns a stable value across repeated reads).
 *
 * The addresses below are educated guesses based on AMD APU memory maps
 * and PS5 firmware analysis. If all fail, kernel_copyout is not functional
 * with the current authid or the jailbreak doesn't expose it.
 */
/* SAFETY: On PS5, the entire 0-4GB physical address space is reserved for
 * GPU MMIO, PCIe config space, and on-chip peripherals. Reading these
 * ranges through the write-back cached DMAP triggers Machine Check
 * Exceptions (MCE) → immediate kernel panic.
 *
 * Only probe DRAM candidates at or above the 4GB boundary. All addresses
 * below 4GB are removed to prevent hardware-level aborts. */
static const uint64_t k_dram_candidates[] = {
    /* Above 4GB boundary — typical for GPU-dedicated memory systems */
    0x0000000100000000ULL,    /* 4GB    — above 32-bit boundary */
    0x0000000200000000ULL,    /* 8GB    */
    0x0000000300000000ULL,    /* 12GB   */

    /* PS5-specific: GDDR6 mapped high in physical address space */
    0x0000001000000000ULL,    /* 64GB   — some AMD APUs place DRAM here */
    0x0000002000000000ULL,    /* 128GB  */
};

#define NUM_DRAM_CANDIDATES \
    (sizeof(k_dram_candidates) / sizeof(k_dram_candidates[0]))

/* Probe the DMAP by trying each DRAM candidate address.
 * Returns 0 on success (DMAP validated), -1 on failure. */
static int probe_dmap(void)
{
    uint64_t test_val, test_val2;

    /* If previously failed, don't retry */
    if (g_dmap_base == DMAP_PROBE_FAILED) return -1;
    /* If already probed successfully, skip */
    if (g_dmap_base != 0) return 0;

    /* Step 1: verify kernel_copyout works on a known kernel VA */
    if (!kernel_copyout_works()) {
        g_dmap_probe_result = POC_DMAP_PROBE_KERNEL_COPYOUT_FAILED;
        g_dmap_probe_rc = -1;
        goto fail;
    }

    /* Step 2: try each DRAM candidate address */
    for (size_t i = 0; i < NUM_DRAM_CANDIDATES; i++) {
        uint64_t candidate_pa = k_dram_candidates[i];
        uint64_t probe_kva = DMAP_BASE_SDK | candidate_pa;
        int rc1, rc2;

        /* First read — verify address is mapped in DMAP */
        rc1 = kernel_copyout((intptr_t)probe_kva,
                             &test_val, sizeof(test_val));
        if (rc1 != 0) continue;  /* This PA not in DMAP, try next */

        /* Second read — verify stability (value shouldn't change
         * between two back-to-back reads of the same location) */
        rc2 = kernel_copyout((intptr_t)probe_kva,
                             &test_val2, sizeof(test_val2));
        if (rc2 != 0) continue;

        if (test_val != test_val2) continue;  /* unstable, skip */

        /* Both reads succeeded and match — DMAP validated */
        g_dmap_base = DMAP_BASE_SDK;
        g_dmap_probe_result = POC_DMAP_PROBE_VALIDATED;
        g_dmap_probe_rc = 0;
        return 0;
    }

    /* All candidates exhausted — DMAP not accessible */
    g_dmap_probe_result = POC_DMAP_PROBE_ALL_CANDIDATES_FAILED;
    g_dmap_probe_rc = -1;

fail:
    g_dmap_base = DMAP_PROBE_FAILED;
    return -1;
}

/* ---- Debug status magic values (mp4_debug_status_t) ---- */
#define DS_MAGIC1_VAL   0xcbb3d18a1aa5daefULL
#define DS_MAGIC2_VAL   0x1aa5daef675a1801ULL
#define DS_MAGIC3_VAL   0x8501dda72c7b400eULL
#define DS_OFF_MAGIC1   offsetof(poc_mailbox_t, mds_magic1)
#define DS_OFF_MAGIC2   offsetof(poc_mailbox_t, mds_magic2)
#define DS_OFF_MAGIC3   offsetof(poc_mailbox_t, mds_magic3)
#define DS_STRUCT_SIZE  sizeof(poc_mailbox_t)
#define DS_SCAN_CHUNK_SIZE 0x10000UL

/* Read-only scan workspace.  Keep the structure-sized overlap so a debug
 * status block crossing a chunk boundary is still recognized. */
static uint8_t g_debug_status_scan_buffer[DS_SCAN_CHUNK_SIZE + DS_STRUCT_SIZE];

/* ---- Full capabilities ---- */
static const uint8_t k_full_caps[16] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

/* Authid used for privilege escalation.
 *
 * The value 0x4801000000000013 was the original PoC authid but may not
 * grant access to kernel_copyout for DMAP addresses.
 *
 * Changed to 0x4800000000010003 (matches working code from vda_probe.c
 * by cheburek3000 / Ghostpad project). This authid is known to grant
 * sufficient privilege for kernel debug operations including copyout
 * from kernel address space.
 *
 * If kernel_copyout still fails with this authid, the jailbreak/kstuff
 * implementation may not expose kernel_copyout for this firmware version.
 */
#define PAL_PRIVILEGE_AUTHID 0x4800000000010003ULL

/* ---- Helpers ---- */
static bool pid_alive(pid_t pid) { return kill(pid, 0) == 0; }

/* ---- Public API ---- */

bool poc_privilege_supported(void) { return true; }

int poc_privilege_jailbreak_self(void)
{
    pid_t pid = getpid();
    int failures = 0;
    intptr_t rootv, fd;
    poc_ucred_backup_t backup;

    memset(&backup, 0, sizeof(backup));

    backup.authid = kernel_get_ucred_authid(pid);
    if (kernel_get_ucred_caps(pid, backup.caps) != 0) return -1;
    backup.attrs = kernel_get_ucred_attrs(pid);
    backup.uid   = kernel_get_ucred_uid(pid);
    backup.ruid  = kernel_get_ucred_ruid(pid);
    backup.svuid = kernel_get_ucred_svuid(pid);
    backup.rgid  = kernel_get_ucred_rgid(pid);
    backup.svgid = kernel_get_ucred_svgid(pid);
    backup.proc_rootdir = kernel_get_proc_rootdir(pid);
    backup.proc_jaildir = kernel_get_proc_jaildir(pid);

    failures += kernel_set_ucred_uid(pid, 0) != 0;
    failures += kernel_set_ucred_ruid(pid, 0) != 0;
    failures += kernel_set_ucred_svuid(pid, 0) != 0;
    failures += kernel_set_ucred_rgid(pid, 0) != 0;
    failures += kernel_set_ucred_svgid(pid, 0) != 0;
    failures += kernel_set_ucred_authid(pid, PAL_PRIVILEGE_AUTHID) != 0;
    failures += kernel_set_ucred_caps(pid, k_full_caps) != 0;

    rootv = kernel_get_root_vnode();
    if (rootv != 0) {
        failures += kernel_set_proc_rootdir(pid, rootv) != 0;
        failures += kernel_set_proc_jaildir(pid, rootv) != 0;

        if (pid_alive(pid))
            fd = kernel_get_proc_filedesc(pid);
        else {
            fd = 0;
            failures++;
        }

        if (fd != 0) {
            backup.fd_rdir = kernel_getlong(fd + KERNEL_OFFSET_FILEDESC_FD_RDIR);
            backup.fd_jdir = kernel_getlong(fd + KERNEL_OFFSET_FILEDESC_FD_JDIR);
            backup.fd_modified = true;
            failures += kernel_setlong(fd + KERNEL_OFFSET_FILEDESC_FD_RDIR, rootv) != 0;
            failures += kernel_setlong(fd + KERNEL_OFFSET_FILEDESC_FD_JDIR, rootv) != 0;
        }
    } else {
        failures++;
    }

    if (failures != 0) {
        poc_privilege_restore_target(pid, &backup);
        return -1;
    }

    /* After successful jailbreak, probe the DMAP so subsequent
     * phys_read/write calls work immediately. */
    if (g_dmap_base == 0)
        (void)probe_dmap();

    return 0;
}

int poc_privilege_elevate_target(pid_t pid, poc_ucred_backup_t *backup)
{
    int failures = 0;
    intptr_t ucred;
    intptr_t rootv;

    if (pid <= 0 || backup == NULL) return -1;
    memset(backup, 0, sizeof(*backup));

    backup->authid = kernel_get_ucred_authid(pid);
    if (kernel_get_ucred_caps(pid, backup->caps) != 0) return -1;
    backup->attrs = kernel_get_ucred_attrs(pid);
    backup->uid   = kernel_get_ucred_uid(pid);
    backup->ruid  = kernel_get_ucred_ruid(pid);
    backup->svuid = kernel_get_ucred_svuid(pid);
    backup->rgid  = kernel_get_ucred_rgid(pid);
    backup->svgid = kernel_get_ucred_svgid(pid);
    backup->proc_rootdir = kernel_get_proc_rootdir(pid);
    backup->proc_jaildir = kernel_get_proc_jaildir(pid);
    backup->fd_rdir = 0;
    backup->fd_jdir = 0;

    if (pid_alive(pid))
        ucred = kernel_get_proc_ucred(pid);
    else { ucred = 0; failures++; }

    if (ucred != 0) {
        if (kernel_copyout(ucred + 0x10, &backup->ngroups,
                           sizeof(backup->ngroups)) == 0)
            backup->ngroups_valid = true;
    }

    failures += kernel_set_ucred_uid(pid, 0) != 0;
    failures += kernel_set_ucred_ruid(pid, 0) != 0;
    failures += kernel_set_ucred_svuid(pid, 0) != 0;
    failures += kernel_set_ucred_rgid(pid, 0) != 0;
    failures += kernel_set_ucred_svgid(pid, 0) != 0;

    if (ucred != 0) {
        uint32_t zero = 0;
        failures += kernel_copyin(&zero, ucred + 0x10, sizeof(zero)) != 0;
    }

    failures += kernel_set_ucred_authid(pid, PAL_PRIVILEGE_AUTHID) != 0;
    failures += kernel_set_ucred_caps(pid, k_full_caps) != 0;
    failures += kernel_set_ucred_attrs(pid, 0x80) != 0;

    rootv = kernel_get_root_vnode();
    if (rootv != 0) {
        intptr_t fd;

        failures += kernel_set_proc_rootdir(pid, rootv) != 0;
        failures += kernel_set_proc_jaildir(pid, rootv) != 0;

        if (pid_alive(pid))
            fd = kernel_get_proc_filedesc(pid);
        else { fd = 0; failures++; }

        if (fd != 0) {
            backup->fd_rdir = kernel_getlong(fd + KERNEL_OFFSET_FILEDESC_FD_RDIR);
            backup->fd_jdir = kernel_getlong(fd + KERNEL_OFFSET_FILEDESC_FD_JDIR);
            backup->fd_modified = true;
            failures += kernel_setlong(fd + KERNEL_OFFSET_FILEDESC_FD_RDIR, rootv) != 0;
            failures += kernel_setlong(fd + KERNEL_OFFSET_FILEDESC_FD_JDIR, rootv) != 0;
        }
    } else {
        failures++;
    }

    if (failures != 0) {
        poc_privilege_restore_target(pid, backup);
        return -1;
    }

    return 0;
}

void poc_privilege_restore_target(pid_t pid, const poc_ucred_backup_t *backup)
{
    intptr_t ucred;

    if (pid <= 0 || backup == NULL) return;

    (void)kernel_set_ucred_authid(pid, backup->authid);
    (void)kernel_set_ucred_caps(pid, backup->caps);
    (void)kernel_set_ucred_attrs(pid, backup->attrs);
    (void)kernel_set_ucred_uid(pid, backup->uid);
    (void)kernel_set_ucred_ruid(pid, backup->ruid);
    (void)kernel_set_ucred_svuid(pid, backup->svuid);
    (void)kernel_set_ucred_rgid(pid, backup->rgid);
    (void)kernel_set_ucred_svgid(pid, backup->svgid);

    if (backup->proc_rootdir != 0)
        (void)kernel_set_proc_rootdir(pid, backup->proc_rootdir);
    if (backup->proc_jaildir != 0)
        (void)kernel_set_proc_jaildir(pid, backup->proc_jaildir);

    if (backup->fd_modified) {
        intptr_t fd = 0;
        if (pid_alive(pid))
            fd = kernel_get_proc_filedesc(pid);
        if (fd != 0) {
            kernel_setlong(fd + KERNEL_OFFSET_FILEDESC_FD_RDIR, backup->fd_rdir);
            kernel_setlong(fd + KERNEL_OFFSET_FILEDESC_FD_JDIR, backup->fd_jdir);
        }
    }

    if (backup->ngroups_valid && pid_alive(pid)) {
        ucred = kernel_get_proc_ucred(pid);
        if (ucred != 0)
            (void)kernel_copyin(&backup->ngroups, ucred + 0x10,
                                sizeof(backup->ngroups));
    }
}

/* ---- Physical memory access via DMAP ---- */

int poc_privilege_phys_read(uint64_t phys_addr, void *buffer, size_t size)
{
    if (!buffer || size == 0) return -1;

    /* Cache: already failed? */
    if (g_dmap_base == DMAP_PROBE_FAILED) return -1;
    /* Probe DMAP if not yet cached */
    if (g_dmap_base == 0 && probe_dmap() != 0) return -1;

    return kernel_copyout((intptr_t)(g_dmap_base + phys_addr), buffer, size);
}

int poc_privilege_phys_write(uint64_t phys_addr, const void *buffer, size_t size)
{
    if (!buffer || size == 0) return -1;

    /* Cache: already failed? */
    if (g_dmap_base == DMAP_PROBE_FAILED) return -1;
    if (g_dmap_base == 0 && probe_dmap() != 0) return -1;

    return kernel_copyin(buffer, (intptr_t)(g_dmap_base + phys_addr), size);
}

poc_dmap_state_t poc_privilege_dmap_state(void)
{
    if (g_dmap_base == 0) return POC_DMAP_UNPROBED;
    if (g_dmap_base == DMAP_PROBE_FAILED) return POC_DMAP_UNAVAILABLE;
    return POC_DMAP_READY;
}

uint64_t poc_privilege_dmap_base(void)
{
    return poc_privilege_dmap_state() == POC_DMAP_READY ? g_dmap_base : 0;
}

poc_dmap_probe_result_t poc_privilege_dmap_probe_result(void)
{
    return g_dmap_probe_result;
}

int poc_privilege_dmap_probe_rc(void)
{
    return g_dmap_probe_rc;
}

/* Return whether kernel_copyout works on known kernel addresses (pre-check). */
int poc_privilege_kernel_copyout_available(void)
{
    if (g_kernel_copyout_available == -1) {
        (void)kernel_copyout_works();
    }
    return g_kernel_copyout_available;
}

/* ---- Physical memory mapping (mmap fallback for DRAM, kernel_copyout for any) ---- */

void *poc_privilege_map_phys(uint64_t phys_addr, size_t size, int prot_flags)
{
    /* For DRAM-backed ranges, use standard mmap with MAP_SHARED.
     * For MMIO/SRAM ranges, we'd need pmap_mapdev or custom pt_mmap.
     * Fall back to the DMAP + kernel_copyout approach via a local buffer. */
    (void)phys_addr; (void)size; (void)prot_flags;
    return NULL;  /* Use phys_read/phys_write for now */
}

int poc_privilege_unmap_phys(void *va, size_t size)
{
    (void)va; (void)size;
    return -1;
}

/* ---- Memory scanner: find debug status by magic values ---- */

/* Scan a physical address range for the three debug status magic values.
 * Returns the physical address of mp4_debug_status_t (where magic1 is at
 * the expected offset), or 0 if not found.
 *
 * IMPORTANT ARCHITECTURE NOTE:
 * The A53 debug_status structure lives in the A53's internal SRAM
 * (at A53 virtual address 0xEC000000). On the PS5, this SRAM is on
 * a separate internal bus within the AMD APU and is NOT mapped in
 * the x86 CPU's DMAP (which only covers main GDDR6 DRAM).
 *
 * Therefore, scanning for magic values via DMAP-based phys_read will
 * NOT find the debug_status structure. The only way to access A53 SRAM
 * from the x86 main CPU is via the /dev/a53mm kernel driver IOCTLs:
 *   - A53MM_MAPPER_QUERY_PA  — query physical addresses for A53 buffers
 *   - A53MM_GIVE_DIRECT_MEM_TO_MAPPER — share memory with A53
 *
 * This function is retained for future use when /dev/a53mm IOCTL support
 * is added, or for reading shared DRAM-based A53 structures.
 */
uint64_t poc_privilege_scan_debug_status(void)
{
    /* Candidate physical address ranges for A53 debug status.
     * NOTE: 0xEC000000 is an A53 VIRTUAL address (from debug_status.c),
     * NOT a physical address on the x86_64 bus — do NOT add it here.
     *
     * The A53 SRAM (where debug_status lives) is on the AMD APU's
     * internal coherent bridge and may be visible at a different
     * physical address range from the x86 side. Without knowing the
     * exact x86-side PA, scanning is best-effort only.
     */
    /* SAFETY: On PS5 (AMD Zen 2 + GDDR6), the x86 DMAP uses write-back
     * caching. Accessing any MMIO region through a WB-cached mapping
     * triggers an immediate Machine Check Exception → kernel panic.
     *
     * Known-dangerous ranges that MUST NOT be added here:
     *   0x03000000-0x03100000  MSI doorbells / scratch registers
     *   0x030C0000-0x030D0000  DVM mailbox (MMIO)
     *   0x03230000-0x03240000  SysHub IOMMU (MMIO)
     *   0x40000000-0xC0000000  GPU MMIO / PCIe config hole
     *   0x80000000-0x90000000  A53 SRAM (separate APU bus, not in DMAP)
     *   0x88000000-0x88FFFFFF  A53 DRAM (separate APU bus)
     *   0xEC000000-0xEC1FFFFF  A53 debug status (separate APU bus)
     *
     * For now, scan_ranges is intentionally empty. Accessing A53-owned
     * memory from the x86 main CPU requires /dev/a53mm IOCTLs:
     *   - A53MM_MAPPER_QUERY_PA
     *   - A53MM_GIVE_DIRECT_MEM_TO_MAPPER
     *   - A53MM_CALL_INDIRECT_BUFFER
     *
     * Until those IOCTL numbers are reverse-engineered, physical scanning
     * is not safe.  This function will return 0 (not found) gracefully. */
    static const uint64_t scan_ranges[][2] = {
        /* Intentionally empty — no safe ranges known yet.
         * Add validated A53 DRAM ranges here once /dev/a53mm IOCTL
         * support is available. */
    };

    uint64_t magic1, magic2, magic3;

    /* Cache: already failed? */
    if (g_dmap_base == DMAP_PROBE_FAILED) return 0;
    if (g_dmap_base == 0 && probe_dmap() != 0) return 0;

    /* For each candidate PA range, copy a bounded block to local memory and
     * scan it 8-byte aligned. */
    for (size_t r = 0; r < sizeof(scan_ranges) / sizeof(scan_ranges[0]); r++) {
        uint64_t start = scan_ranges[r][0] & ~7ULL;
        uint64_t end   = scan_ranges[r][1];

        for (uint64_t pa = start; pa < end; pa += DS_SCAN_CHUNK_SIZE) {
            size_t bytes = (size_t)(end - pa);
            int ret;

            if (bytes > sizeof(g_debug_status_scan_buffer)) {
                bytes = sizeof(g_debug_status_scan_buffer);
            }
            if (bytes < DS_STRUCT_SIZE) break;

            ret = kernel_copyout((intptr_t)(g_dmap_base + pa),
                                 g_debug_status_scan_buffer, bytes);
            if (ret != 0) break;  /* This physical range is not DMAP-visible. */

            for (size_t off = 0; off + DS_STRUCT_SIZE <= bytes;
                 off += sizeof(uint64_t)) {
                memcpy(&magic1, g_debug_status_scan_buffer + off + DS_OFF_MAGIC1,
                       sizeof(magic1));
                if (magic1 == DS_MAGIC1_VAL) {
                    memcpy(&magic2, g_debug_status_scan_buffer + off + DS_OFF_MAGIC2,
                           sizeof(magic2));
                    memcpy(&magic3, g_debug_status_scan_buffer + off + DS_OFF_MAGIC3,
                           sizeof(magic3));

                    if (magic2 == DS_MAGIC2_VAL && magic3 == DS_MAGIC3_VAL) {
                        return pa + off;  /* All three magic values confirmed. */
                    }
                }
            }
        }
    }

    return 0;  /* Not found in any scanned range */
}

/* Copy the full debug status structure from physical memory into a buffer.
 * Returns 0 on success, -1 on failure. */
int poc_privilege_read_debug_status(uint64_t ds_pa, poc_mailbox_t *out)
{
    if (!out || ds_pa == 0) return -1;

    /* Cache: already failed? */
    if (g_dmap_base == DMAP_PROBE_FAILED) return -1;
    if (g_dmap_base == 0 && probe_dmap() != 0) return -1;

    /* Read the entire structure in one kernel_copyout call. */
    return kernel_copyout((intptr_t)(g_dmap_base + ds_pa),
                          out, sizeof(poc_mailbox_t));
}

/* ---- Diagnostics (sysctl-based physical memory info) ---- */

/* Get total physical memory from sysctl hw.physmem.
 * Returns 0 on failure (e.g., sysctl not available). */
uint64_t poc_privilege_get_physmem(void)
{
    unsigned long physmem = 0;
    size_t len = sizeof(physmem);
    int mib[] = {CTL_HW, HW_PHYSMEM};

    if (sysctl(mib, 2, &physmem, &len, NULL, 0) == 0)
        return (uint64_t)physmem;
    return 0;
}

/* Get available physical memory from sysctl hw.usermem.
 * Returns 0 on failure. */
uint64_t poc_privilege_get_usermem(void)
{
    unsigned long usermem = 0;
    size_t len = sizeof(usermem);
    int mib[] = {CTL_HW, HW_USERMEM};

    if (sysctl(mib, 2, &usermem, &len, NULL, 0) == 0)
        return (uint64_t)usermem;
    return 0;
}

/* Get number of candidates tried in the last DMAP probe */
int poc_privilege_dmap_num_candidates_tried(void)
{
    return (int)NUM_DRAM_CANDIDATES;
}

#else /* !PAL_PRIVILEGE_HAS_PS5 — no-op stubs */

bool poc_privilege_supported(void) { return false; }

int poc_privilege_jailbreak_self(void) { return 0; }

int poc_privilege_elevate_target(pid_t pid, poc_ucred_backup_t *backup)
{
    (void)pid;
    if (backup != NULL) memset(backup, 0, sizeof(*backup));
    return 0;
}

void poc_privilege_restore_target(pid_t pid, const poc_ucred_backup_t *backup)
{
    (void)pid; (void)backup;
}

void *poc_privilege_map_phys(uint64_t phys_addr, size_t size, int prot_flags)
{
    (void)phys_addr; (void)size; (void)prot_flags;
    return NULL;
}

int poc_privilege_unmap_phys(void *va, size_t size)
{
    (void)va; (void)size;
    return -1;
}

int poc_privilege_phys_read(uint64_t phys_addr, void *buffer, size_t size)
{
    (void)phys_addr; (void)buffer; (void)size;
    return -1;
}

int poc_privilege_phys_write(uint64_t phys_addr, const void *buffer, size_t size)
{
    (void)phys_addr; (void)buffer; (void)size;
    return -1;
}

poc_dmap_state_t poc_privilege_dmap_state(void)
{
    return POC_DMAP_UNAVAILABLE;
}

uint64_t poc_privilege_dmap_base(void)
{
    return 0;
}

poc_dmap_probe_result_t poc_privilege_dmap_probe_result(void)
{
    return POC_DMAP_PROBE_NOT_RUN;
}

int poc_privilege_dmap_probe_rc(void)
{
    return -1;
}

int poc_privilege_kernel_copyout_available(void)
{
    return 0;
}

uint64_t poc_privilege_scan_debug_status(void)
{
    return 0;
}

int poc_privilege_read_debug_status(uint64_t ds_pa, poc_mailbox_t *out)
{
    (void)ds_pa; (void)out;
    return -1;
}

uint64_t poc_privilege_get_physmem(void)
{
    return 0;
}

uint64_t poc_privilege_get_usermem(void)
{
    return 0;
}

int poc_privilege_dmap_num_candidates_tried(void)
{
    return 0;
}

#endif /* PAL_PRIVILEGE_HAS_PS5 */
