#ifndef PAL_PRIVILEGE_H
#define PAL_PRIVILEGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

/* The debug status mailbox type — needed for read_debug_status().
 * poc.h defines poc_mailbox_t. */
#include "poc.h"

typedef enum {
    POC_DMAP_UNPROBED = 0,
    POC_DMAP_READY,
    POC_DMAP_UNAVAILABLE,
} poc_dmap_state_t;

typedef enum {
    POC_DMAP_PROBE_NOT_RUN = 0,
    POC_DMAP_PROBE_VALIDATED,
    POC_DMAP_PROBE_FIRST_READ_FAILED,
    POC_DMAP_PROBE_SECOND_READ_FAILED,
    POC_DMAP_PROBE_UNSTABLE,
    POC_DMAP_PROBE_KERNEL_COPYOUT_FAILED,
    POC_DMAP_PROBE_ALL_CANDIDATES_FAILED,
} poc_dmap_probe_result_t;

/* ---- Privilege escalation via kernel helpers (ps5-payload-sdk kernel.h) ----
 *
 * These functions use the PS5 kernel API exported by the bootloader/jailbreak
 * (kstuff/prospero) to elevate the current process to root and escape the
 * sandbox. After escalation, the process can map physical memory (via
 * kernel_copyout/copyin + pt_mmap) and access the A53 SRAM mailbox.
 *
 * Requires: <ps5/kernel.h> from ps5-payload-sdk (included via -isysroot).
 *           The loader must export kernel helper symbols.
 */

/* ---- Ucred backup (for rollback) ---- */
typedef struct {
    uint64_t authid;
    uint8_t  caps[16];
    uint64_t attrs;
    uid_t    uid;
    uid_t    ruid;
    uid_t    svuid;
    gid_t    rgid;
    gid_t    svgid;
    intptr_t proc_rootdir;
    intptr_t proc_jaildir;
    /* filedesc root/jail vnode reference */
    intptr_t fd_rdir;
    intptr_t fd_jdir;
    bool     fd_modified;
    bool     ngroups_valid;
    uint32_t ngroups;
} poc_ucred_backup_t;

/* ---- Public API ---- */

/* Check if privilege escalation is supported on this platform. */
bool poc_privilege_supported(void);

/* Elevate the current process to root via ucred manipulation.
 * Returns 0 on success, -1 on failure.
 * Pattern: memDBG privilege.c — kernel_set_ucred_* calls. */
int poc_privilege_jailbreak_self(void);

/* Elevate a specific target PID to root. Stores backup for restoration.
 * Returns 0 on success, -1 on failure. */
int poc_privilege_elevate_target(pid_t pid, poc_ucred_backup_t *backup);

/* Restore a target process's ucred from a backup. */
void poc_privilege_restore_target(pid_t pid, const poc_ucred_backup_t *backup);

/* Map physical memory into the process address space using pt_mmap/kernel
 * helpers. Root access does not by itself prove that A53-owned ranges are
 * visible through the main CPU's direct physical map.
 *
 * Returns a pointer to the mapped region, or NULL on failure.
 * flags: PROT_READ | PROT_WRITE (from <sys/mman.h>) */
void *poc_privilege_map_phys(uint64_t phys_addr, size_t size, int prot_flags);

/* Unmap a previously mapped physical region. */
int poc_privilege_unmap_phys(void *va, size_t size);

/* Read kernel memory at a given physical address (post-jailbreak).
 * Returns 0 on success, -1 on failure. */
int poc_privilege_phys_read(uint64_t phys_addr, void *buffer, size_t size);

/* Write kernel memory at a given physical address (post-jailbreak).
 * Returns 0 on success, -1 on failure. */
int poc_privilege_phys_write(uint64_t phys_addr, const void *buffer, size_t size);

/* Read-only diagnostic state for the DMAP probe. A base is returned only
 * when the probe was validated; it is never a claim that an A53 range is
 * accessible through that mapping. */
poc_dmap_state_t poc_privilege_dmap_state(void);
uint64_t poc_privilege_dmap_base(void);
poc_dmap_probe_result_t poc_privilege_dmap_probe_result(void);
int poc_privilege_dmap_probe_rc(void);

/* Scan physical memory for the A53 debug status structure by searching
 * for the three known magic values (mds_magic1/2/3). Returns the physical
 * address of the structure, or 0 if not found.
 *
 * Uses kernel_copyout on the kernel's DMAP direct map. The DMAP base is
 * probed dynamically so this works across different firmware versions. */
uint64_t poc_privilege_scan_debug_status(void);

/* Copy the full debug status structure from physical memory into a local
 * buffer. Returns 0 on success, -1 on failure.
 * ds_pa must be a valid physical address (from scan_debug_status). */
int poc_privilege_read_debug_status(uint64_t ds_pa, poc_mailbox_t *out);

/* --- Enhanced diagnostics (post-fix) --- */

/* Pre-check: does kernel_copyout work on known kernel addresses?
 * This runs BEFORE the DMAP probe to distinguish between:
 *   - kernel_copyout broken (authid/jailbreak issue)
 *   - DMAP base wrong (different firmware layout)
 *   - probe address not DRAM (wrong candidate PA)
 * Returns 1 if kernel_copyout works, 0 if it fails. */
int poc_privilege_kernel_copyout_available(void);

/* Get physical memory size from sysctl hw.physmem (0 on failure). */
uint64_t poc_privilege_get_physmem(void);

/* Get user-accessible memory from sysctl hw.usermem (0 on failure). */
uint64_t poc_privilege_get_usermem(void);

/* Return number of DRAM candidate addresses tried in probe. */
int poc_privilege_dmap_num_candidates_tried(void);

#ifdef __cplusplus
}
#endif

#endif /* PAL_PRIVILEGE_H */
