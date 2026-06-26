/* SPDX-License-Identifier: GPL-3.0-or-later
 * Ghostpad VDA Probe - read-only PS4/PS5 diagnostics.
 *
 * This payload intentionally does not write target memory and does not install
 * hooks. It collects the minimum information needed to make future libScePad/VDA
 * patching firmware-specific instead of pattern-guessing live code.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#ifndef __ORBIS__
#include <sys/utsname.h>
#endif
#include <unistd.h>

#ifdef __PROSPERO__
#include <ps5/kernel.h>
#include <ps5/klog.h>
#include <ps5/mdbg.h>
#include <ps5/nid.h>
#define GP_PLATFORM "PROSPERO/PS5"
#endif

#ifdef __ORBIS__
#include <ps4/kernel.h>
#include <ps4/klog.h>
#include <ps4/mdbg.h>
#define GP_PLATFORM "ORBIS/PS4"
#endif

#if !defined(__PROSPERO__) && !defined(__ORBIS__)
#error "Build with PS4_PAYLOAD_SDK or PS5_PAYLOAD_SDK"
#endif

#ifndef VDA_PROBE_PORT
#define VDA_PROBE_PORT 6975
#endif

#ifndef VDA_PROBE_SERVE_SECONDS
#define VDA_PROBE_SERVE_SECONDS 180
#endif

#define REPORT_CAP      (192u * 1024u)
#define READ_SMALL      256u
#define READ_WINDOW     4096u
#define HID_WINDOW      1024u
#define MAX_PIDS        8u
#define MAX_SYMBOL_NAME 64u

#define REPORT_DIR_DATA  "/data/ghostpad"
#define REPORT_PATH_DATA "/data/ghostpad/vda_probe_report.txt"
#define REPORT_PATH_USB  "/mnt/usb0/vda_probe_report.txt"

#define VDA_CAVE_MIN_RUN       10u
#define VDA_CAVE_TOP           12u
#define VDA_SCAN_PATCH_MAX     1536u
#define VDA_SCAN_CAVE_MAX      4096u
#define ENTROPY32_MAX_Q8       1280u

static char   g_report[REPORT_CAP];
static size_t g_report_len = 0;

typedef struct notify_request {
    char useless1[45];
    char message[3075];
} notify_request_t;

__attribute__((weak))
int sceKernelSendNotificationRequest(int, notify_request_t *, size_t, int);

static void
notify_user(const char *msg)
{
    notify_request_t req;
    if (!msg) return;
    if (sceKernelSendNotificationRequest != NULL) {
        memset(&req, 0, sizeof(req));
        strncpy(req.message, msg, sizeof(req.message) - 1);
        if (sceKernelSendNotificationRequest(0, &req, sizeof(req), 0) == 0) {
            return;
        }
    }
    klog_printf("%s\n", msg);
}

static int
ensure_dir(const char *path)
{
    struct stat st;
    if (!path) return -1;
    if (mkdir(path, 0755) == 0) return 0;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) ? 0 : -1;
}

static int
write_file_full(const char *path, const void *data, size_t len)
{
    const uint8_t *p = (const uint8_t *)data;
    int fd;
    if (!path || !data || len == 0) return -1;
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) return -1;
    while (len > 0) {
        ssize_t n = write(fd, p, len);
        if (n <= 0) {
            close(fd);
            return -2;
        }
        p += (size_t)n;
        len -= (size_t)n;
    }
    close(fd);
    return 0;
}

static void
report_append_raw(const char *s, size_t n)
{
    if (!s || n == 0) return;
    if (g_report_len + n >= sizeof(g_report)) {
        n = sizeof(g_report) - g_report_len - 1;
    }
    if (n > 0) {
        memcpy(g_report + g_report_len, s, n);
        g_report_len += n;
        g_report[g_report_len] = '\0';
    }
}

static void
reportf(const char *fmt, ...)
{
    char line[1024];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    if (n <= 0) return;
    if ((size_t)n >= sizeof(line)) {
        n = (int)sizeof(line) - 1;
        line[n] = '\0';
    }
    report_append_raw(line, (size_t)n);

    /* Mirror to klog in chunks; klog lines longer than a few hundred bytes are
     * easy to lose/truncate on some setups. */
    const size_t max_chunk = 360;
    for (size_t off = 0; off < (size_t)n; off += max_chunk) {
        char chunk[384];
        size_t take = (size_t)n - off;
        if (take > max_chunk) take = max_chunk;
        memcpy(chunk, line + off, take);
        chunk[take] = '\0';
        klog_printf("%s", chunk);
    }
}

static uint64_t
fnv1a64(const uint8_t *buf, size_t len)
{
    uint64_t h = 1469598103934665603ull;
    for (size_t i = 0; i < len; i++) {
        h ^= (uint64_t)buf[i];
        h *= 1099511628211ull;
    }
    return h;
}

static void
hex_bytes(char *out, size_t out_len, const uint8_t *buf, size_t len)
{
    static const char hexdig[] = "0123456789abcdef";
    size_t pos = 0;
    if (!out || out_len == 0) return;
    out[0] = '\0';
    for (size_t i = 0; i < len && pos + 4 < out_len; i++) {
        if (i != 0) out[pos++] = ' ';
        out[pos++] = hexdig[(buf[i] >> 4) & 0xf];
        out[pos++] = hexdig[buf[i] & 0xf];
    }
    out[pos] = '\0';
}

static int
all_bytes_equal(const uint8_t *data, size_t len)
{
    if (!data || len == 0) return 1;
    for (size_t i = 1; i < len; i++) {
        if (data[i] != data[0]) return 0;
    }
    return 1;
}

/* Fixed-point entropy for 32-byte windows, adapted from the EAP scanner.  It is
 * used only as a quality signal for candidate code caves/padding regions; no
 * bytes are treated as secrets and no memory is written. */
static uint32_t
entropy32_q8(const uint8_t *data)
{
    static const uint16_t log2_tbl[33] = {
        0,
        0, 256, 406, 512, 595, 662, 720, 768,
        811, 851, 887, 918, 947, 974, 999, 1024,
        1047, 1068, 1088, 1107, 1126, 1143, 1159, 1174,
        1189, 1203, 1216, 1229, 1241, 1253, 1264, 1280
    };
    uint32_t freq[256];
    uint32_t result_q8 = 0;

    if (!data) return 0;
    memset(freq, 0, sizeof(freq));
    for (size_t i = 0; i < 32; i++) freq[data[i]]++;
    for (size_t i = 0; i < 256; i++) {
        uint32_t f = freq[i];
        if (f > 0 && f <= 32) {
            result_q8 += (f * (ENTROPY32_MAX_Q8 - log2_tbl[f])) / 32u;
        }
    }
    return result_q8;
}

static uint32_t
q8_frac_to_percent(uint32_t q8)
{
    return ((q8 % 256u) * 100u + 128u) / 256u;
}

typedef struct VdaCaveCandidate {
    size_t   offset;
    size_t   length;
    uint8_t  byte;
    uint32_t entropy_before_q8;
    uint32_t entropy_after_q8;
    int32_t  score;
    int      valid;
} VdaCaveCandidate;

static int
cave_candidate_better(const VdaCaveCandidate *a, const VdaCaveCandidate *b)
{
    if (!a || !a->valid) return 0;
    if (!b || !b->valid) return 1;
    if (a->score != b->score) return a->score > b->score;
    if (a->length != b->length) return a->length > b->length;
    return a->offset < b->offset;
}

static void
cave_top_insert(VdaCaveCandidate top[VDA_CAVE_TOP],
                const VdaCaveCandidate *candidate)
{
    if (!top || !candidate || !candidate->valid) return;
    for (size_t i = 0; i < VDA_CAVE_TOP; i++) {
        if (cave_candidate_better(candidate, &top[i])) {
            for (size_t j = VDA_CAVE_TOP - 1; j > i; j--) top[j] = top[j - 1];
            top[i] = *candidate;
            return;
        }
    }
}

static void
score_cave_run(const uint8_t *buf, size_t len, size_t off, size_t run_len,
               uint8_t byte, VdaCaveCandidate *out)
{
    uint32_t before = 0;
    uint32_t after = 0;
    int32_t score;

    if (!out) return;
    memset(out, 0, sizeof(*out));
    if (!buf || run_len < VDA_CAVE_MIN_RUN) return;

    if (off >= 32) before = entropy32_q8(buf + off - 32);
    if (off + run_len + 32 <= len) after = entropy32_q8(buf + off + run_len);

    score = (int32_t)run_len * 20;
    if (byte == 0xCC) score += 250;       /* explicit padding/debug fill */
    else if (byte == 0x90) score += 140;  /* NOP padding */
    else if (byte == 0x00) score -= 120;  /* often data/reloc, less ideal */

    /* Prefer isolated low-entropy runs surrounded by normal-looking code. */
    if (before > 512 && before < 1180) score += 80;
    if (after > 512 && after < 1180) score += 80;
    if (all_bytes_equal(buf + off, run_len)) score += 40;

    out->offset = off;
    out->length = run_len;
    out->byte = byte;
    out->entropy_before_q8 = before;
    out->entropy_after_q8 = after;
    out->score = score;
    out->valid = 1;
}

static void
report_best_caves(const uint8_t *buf, size_t len)
{
    VdaCaveCandidate top[VDA_CAVE_TOP];
    memset(top, 0, sizeof(top));

    for (size_t i = 0; i < len && i < VDA_SCAN_CAVE_MAX;) {
        uint8_t b = buf[i];
        size_t j = i + 1;
        if (b != 0xCC && b != 0x90 && b != 0x00) {
            i++;
            continue;
        }
        while (j < len && j < VDA_SCAN_CAVE_MAX && buf[j] == b) j++;
        if (j - i >= VDA_CAVE_MIN_RUN) {
            VdaCaveCandidate c;
            score_cave_run(buf, len, i, j - i, b, &c);
            cave_top_insert(top, &c);
        }
        i = j;
    }

    reportf("  VDA_CAVE_RANKED top=%u min_run=%u\n",
            (unsigned)VDA_CAVE_TOP, (unsigned)VDA_CAVE_MIN_RUN);
    for (size_t i = 0; i < VDA_CAVE_TOP; i++) {
        if (!top[i].valid) continue;
        reportf("  VDA_CAVE_CANDIDATE #%zu off=0x%zx len=%zu byte=0x%02x score=%ld before_entropy=%u.%02u after_entropy=%u.%02u\n",
                i + 1,
                top[i].offset,
                top[i].length,
                top[i].byte,
                (long)top[i].score,
                (unsigned)(top[i].entropy_before_q8 / 256u),
                (unsigned)q8_frac_to_percent(top[i].entropy_before_q8),
                (unsigned)(top[i].entropy_after_q8 / 256u),
                (unsigned)q8_frac_to_percent(top[i].entropy_after_q8));
    }
}


/* find_pids — locate processes by thread name via sysctl.
 * These kinfo_proc offsets match the Ghostpad runtime path currently used for
 * PS4/PS5 payloads. If this returns no pids, the report explicitly says so. */
static size_t
find_pids_by_tdname(const char *name, pid_t *pids, size_t max_pids)
{
    int mib[4] = {1, 14, 8, 0};
    pid_t mypid = getpid();
    size_t buf_size = 0;
    uint8_t *buf = NULL;
    size_t count = 0;

    if (!name || !pids || max_pids == 0) return 0;
    if (sysctl(mib, 4, NULL, &buf_size, NULL, 0) != 0 || buf_size == 0) return 0;
    buf = (uint8_t *)malloc(buf_size);
    if (!buf) return 0;
    if (sysctl(mib, 4, buf, &buf_size, NULL, 0) != 0) {
        free(buf);
        return 0;
    }

    for (uint8_t *ptr = buf; ptr < (buf + buf_size);) {
        int ki_structsize = *(int *)ptr;
        if (ki_structsize <= 0) break;
        pid_t ki_pid = *(pid_t *)&ptr[72];
        char *ki_tdname = (char *)&ptr[447];
        int seen = 0;

        ptr += ki_structsize;
        if (strcmp(name, ki_tdname) != 0 || ki_pid == mypid) continue;
        for (size_t i = 0; i < count; i++) {
            if (pids[i] == ki_pid) { seen = 1; break; }
        }
        if (!seen && count < max_pids) pids[count++] = ki_pid;
    }

    for (size_t i = 1; i < count; i++) {
        pid_t pid = pids[i];
        size_t j = i;
        while (j > 0 && pids[j - 1] > pid) {
            pids[j] = pids[j - 1];
            j--;
        }
        pids[j] = pid;
    }

    free(buf);
    return count;
}

static int
get_lib_handle(pid_t pid, const char *name, uint32_t *handle)
{
    int ret;
    char sprx[MAX_SYMBOL_NAME];
    if (!handle) return -1;
    *handle = 0;
    ret = kernel_dynlib_handle(pid, name, handle);
    if ((ret != 0 || *handle == 0) && strlen(name) + 6 < sizeof(sprx)) {
        snprintf(sprx, sizeof(sprx), "%s.sprx", name);
        ret = kernel_dynlib_handle(pid, sprx, handle);
    }
    return (*handle != 0) ? 0 : -1;
}

static intptr_t
resolve_sym(pid_t pid, uint32_t lib_handle, const char *sym)
{
    intptr_t addr = 0;
    if (!lib_handle || !sym) return 0;
    addr = kernel_dynlib_dlsym(pid, lib_handle, sym);
    if (addr) return addr;
#ifdef __PROSPERO__
    char nid[12];
    nid_encode(sym, nid);
    addr = kernel_dynlib_resolve(pid, lib_handle, nid);
#endif
    return addr;
}

static int
read_remote(pid_t pid, intptr_t addr, void *buf, size_t len)
{
    uint8_t privcaps[16] = {
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
    };
    uint8_t saved_caps[16];
    pid_t mypid = getpid();
    uint64_t saved_authid = 0;
    int ret;

    if (!addr || !buf || len == 0) return -1;

    saved_authid = kernel_get_ucred_authid(mypid);
    if (saved_authid && kernel_get_ucred_caps(mypid, saved_caps) == 0) {
        kernel_set_ucred_authid(mypid, 0x4800000000010003l);
        kernel_set_ucred_caps(mypid, privcaps);
    } else {
        saved_authid = 0;
    }

    ret = mdbg_copyout(pid, addr, buf, len);

    if (saved_authid) {
        kernel_set_ucred_authid(mypid, saved_authid);
        kernel_set_ucred_caps(mypid, saved_caps);
    }

    return ret;
}

typedef struct ProbeSymbol {
    const char *name;
    int         is_vda;
} ProbeSymbol;

typedef struct ProbeLibrary {
    const char        *name;
    const ProbeSymbol *symbols;
    size_t             symbol_count;
} ProbeLibrary;

static const ProbeSymbol g_pad_symbols[] = {
    {"scePadVirtualDeviceAddDevice", 1},
    {"scePadVirtualDeviceDeleteDevice", 0},
    {"scePadVirtualDeviceInsertData", 0},
    {"scePadGetHandle", 0},
    {"scePadOpen", 0},
    {"scePadOpenExt", 0},
    {"scePadOpenExt2", 0},
    {"scePadSetProcessPrivilege", 0},
    {"scePadSetLoginUserNumber", 0},
    {"scePadSetUserNumber", 0},
    {"scePadSetProcessFocus", 0},
};

static const ProbeSymbol g_mbus_symbols[] = {
    /* Known exports used by current Ghostpad runtime. */
    {"sceMbusBindDeviceWithUserId", 0},
    {"sceMbusDisconnectDevice", 0},

    /* Candidate discovery/enumeration APIs.  These are intentionally
     * resolve-only in the probe: do not call unknown signatures yet.
     * If any of them resolve, we can replace the klog DeviceId path with
     * a direct MBus enumeration path in the runtime. */
    {"sceMbusGetUnassignedDeviceInfo", 0},
    {"sceMbusGetAssignedDeviceInfo", 0},
    {"sceMbusGetDeviceInfo", 0},
    {"sceMbusGetDeviceInfoByDeviceId", 0},
    {"sceMbusGetDeviceInfoByUserId", 0},
    {"sceMbusGetDeviceList", 0},
    {"sceMbusGetDeviceIdList", 0},
    {"sceMbusGetDeviceId", 0},
    {"sceMbusGetDeviceUserId", 0},
    {"sceMbusGetDeviceType", 0},
    {"sceMbusGetDeviceSubType", 0},
    {"sceMbusGetConnectedDeviceStatus", 0},
    {"sceMbusGetConnectionInfo", 0},
    {"sceMbusGetEvent", 0},
    {"sceMbusGetEventInfo", 0},
    {"sceMbusReceiveEvent", 0},
    {"sceMbusRegisterEventCallback", 0},
    {"sceMbusUnregisterEventCallback", 0},
    {"sceMbusOpen", 0},
    {"sceMbusClose", 0},
};

static const ProbeSymbol g_bt_symbols[] = {
    /* Resolve-only sweep. These names intentionally cover multiple possible
     * Sony naming layers because PSDevWiki documents Bluetooth/DS4 behavior,
     * but not public exports for the pairing stack.  Do not call any of these
     * until a later probe confirms prototype/side effects. */
    {"sceBtInit", 0},
    {"sceBtTerm", 0},
    {"sceBtStartInquiry", 0},
    {"sceBtCancelInquiry", 0},
    {"sceBtGetInquiryResult", 0},
    {"sceBtRegisterEventCallback", 0},
    {"sceBtUnregisterEventCallback", 0},
    {"sceBtGetLocalBdAddress", 0},
    {"sceBtGetLocalDeviceAddress", 0},
    {"sceBtGetDeviceInfo", 0},
    {"sceBtGetDeviceList", 0},
    {"sceBtGetPairedDeviceList", 0},
    {"sceBtGetConnectedDeviceList", 0},
    {"sceBtPairing", 0},
    {"sceBtPairDevice", 0},
    {"sceBtUnpairDevice", 0},
    {"sceBtConnectDevice", 0},
    {"sceBtDisconnectDevice", 0},
    {"sceBtSetPairingMode", 0},
    {"sceBtSetDiscoverable", 0},
    {"sceBtSetConnectable", 0},
    {"sceBtSetVisibility", 0},
    {"sceBluetoothInit", 0},
    {"sceBluetoothTerm", 0},
    {"sceBluetoothStartInquiry", 0},
    {"sceBluetoothCancelInquiry", 0},
    {"sceBluetoothGetDeviceInfo", 0},
    {"sceBluetoothGetDeviceList", 0},
    {"sceBluetoothPairDevice", 0},
    {"sceBluetoothConnectDevice", 0},
    {"sceBluetoothDisconnectDevice", 0},
    {"sceHidControlInit", 0},
    {"sceHidControlTerm", 0},
    {"sceHidControlGetDeviceInfo", 0},
    {"sceHidControlGetDeviceList", 0},
    {"sceHidControlConnect", 0},
    {"sceHidControlDisconnect", 0},
    {"sceHidControlRegisterCallback", 0},
    {"sceHidControlUnregisterCallback", 0},
};

static const ProbeSymbol g_kernel_symbols[] = {
    {"pthread_create", 0},
    {"usleep", 0},
    {"mmap", 0},
};

static const ProbeLibrary g_libraries[] = {
    {"libScePad",    g_pad_symbols,    sizeof(g_pad_symbols) / sizeof(g_pad_symbols[0])},
    {"libSceMbus",   g_mbus_symbols,   sizeof(g_mbus_symbols) / sizeof(g_mbus_symbols[0])},

    /* Bluetooth/HID stack candidates.  PSDevWiki documents DS4-BT behavior and
     * the internal libSceHidControl sysmodule, but names/exports vary by FW;
     * keep this resolve-only and report missing libraries explicitly. */
    {"libSceHidControl", g_bt_symbols, sizeof(g_bt_symbols) / sizeof(g_bt_symbols[0])},
    {"libSceBluetooth",  g_bt_symbols, sizeof(g_bt_symbols) / sizeof(g_bt_symbols[0])},
    {"libSceBt",         g_bt_symbols, sizeof(g_bt_symbols) / sizeof(g_bt_symbols[0])},
    {"libSceBtHid",      g_bt_symbols, sizeof(g_bt_symbols) / sizeof(g_bt_symbols[0])},
    {"libSceHid",        g_bt_symbols, sizeof(g_bt_symbols) / sizeof(g_bt_symbols[0])},
    {"libSceMove",       g_bt_symbols, sizeof(g_bt_symbols) / sizeof(g_bt_symbols[0])},
    {"libScePadTracker", g_bt_symbols, sizeof(g_bt_symbols) / sizeof(g_bt_symbols[0])},
    {"libSceUsbd",       g_bt_symbols, sizeof(g_bt_symbols) / sizeof(g_bt_symbols[0])},
    {"libSceRemoteplay", g_bt_symbols, sizeof(g_bt_symbols) / sizeof(g_bt_symbols[0])},

    {"libkernel",   g_kernel_symbols, sizeof(g_kernel_symbols) / sizeof(g_kernel_symbols[0])},
};

static void
scan_vda_candidates(const uint8_t *buf, size_t len)
{
    int found = 0;
    reportf("  VDA_SCAN len=%zu patch_scan_max=%u cave_scan_max=%u\n",
            len, (unsigned)VDA_SCAN_PATCH_MAX, (unsigned)VDA_SCAN_CAVE_MAX);

    /* Keep the old narrow pattern, but report it as a candidate only.  The
     * final patcher must still verify firmware, hash and prologue before any
     * write. */
    for (size_t i = 0; i + 20 < len && i < VDA_SCAN_PATCH_MAX; i++) {
        if (buf[i + 0] == 0xE8 &&
            buf[i + 5] == 0x48 && buf[i + 6] == 0x8B && buf[i + 7] == 0x0B &&
            buf[i + 8] == 0x48 && buf[i + 9] == 0x3B) {
            for (size_t j = i + 10; j < i + 24 && j < len; j++) {
                if (buf[j] == 0x75 || buf[j] == 0x0F || buf[j] == 0x74) {
                    int32_t rel = (int32_t)((uint32_t)buf[i + 1] |
                                             ((uint32_t)buf[i + 2] << 8) |
                                             ((uint32_t)buf[i + 3] << 16) |
                                             ((uint32_t)buf[i + 4] << 24));
                    reportf("  VDA_PATCHSITE_CANDIDATE type=call_canary off=0x%zx rel32=0x%08x after_call=0x%zx branch_off=0x%zx branch_op=0x%02x\n",
                            i, (uint32_t)rel, i + 5, j, buf[j]);
                    found = 1;
                    break;
                }
            }
        }
    }

    /* A second generic signal: calls followed by a conditional branch within a
     * short range.  This helps compare firmwares even when the exact old PS5
     * byte pattern does not exist on PS4. */
    for (size_t i = 0; i + 32 < len && i < VDA_SCAN_PATCH_MAX; i++) {
        if (buf[i] != 0xE8) continue;
        for (size_t j = i + 5; j < i + 32 && j < len; j++) {
            int branch = (buf[j] == 0x74 || buf[j] == 0x75 ||
                          buf[j] == 0x7C || buf[j] == 0x7D ||
                          buf[j] == 0x7E || buf[j] == 0x7F ||
                          (buf[j] == 0x0F && j + 1 < len &&
                           buf[j + 1] >= 0x80 && buf[j + 1] <= 0x8F));
            if (branch) {
                int32_t rel = (int32_t)((uint32_t)buf[i + 1] |
                                         ((uint32_t)buf[i + 2] << 8) |
                                         ((uint32_t)buf[i + 3] << 16) |
                                         ((uint32_t)buf[i + 4] << 24));
                reportf("  VDA_PATCHSITE_CANDIDATE type=generic_call_branch off=0x%zx rel32=0x%08x branch_off=0x%zx branch_op=0x%02x\n",
                        i, (uint32_t)rel, j, buf[j]);
                found = 1;
                break;
            }
        }
    }

    report_best_caves(buf, len);

    if (!found) reportf("  VDA_SCAN no patchsite candidates found\n");
}

static void
scan_hidcontrol_get_device_info(const uint8_t *buf, size_t len)
{
    if (!buf || len == 0) return;
    reportf("  HIDCONTROL_SCAN len=%zu note=read_only_no_calls\n", len);

    for (size_t off = 0; off < len; off += 16) {
        char hex[3 * 16 + 1];
        size_t take = len - off;
        if (take > 16) take = 16;
        hex_bytes(hex, sizeof(hex), buf + off, take);
        reportf("  HIDCONTROL_HEX +0x%03zx %s\n", off, hex);
    }

    for (size_t i = 0; i + 8 < len; i++) {
        if (buf[i] == 0xE8) {
            int32_t rel = (int32_t)((uint32_t)buf[i + 1] |
                                     ((uint32_t)buf[i + 2] << 8) |
                                     ((uint32_t)buf[i + 3] << 16) |
                                     ((uint32_t)buf[i + 4] << 24));
            reportf("  HIDCONTROL_CALL_CANDIDATE off=0x%zx rel32=0x%08x target_off=0x%zx\n",
                    i, (uint32_t)rel, (size_t)(i + 5 + rel));
        }
        if ((buf[i] == 0x74 || buf[i] == 0x75 || buf[i] == 0x7c || buf[i] == 0x7d ||
             buf[i] == 0x7e || buf[i] == 0x7f) && i + 1 < len) {
            int8_t rel8 = (int8_t)buf[i + 1];
            reportf("  HIDCONTROL_BRANCH8_CANDIDATE off=0x%zx op=0x%02x rel8=%d target_off=0x%zx\n",
                    i, buf[i], (int)rel8, (size_t)(i + 2 + rel8));
        }
    }
}

static void
probe_symbol(pid_t pid, const char *proc_name, const char *lib_name,
             const ProbeSymbol *sym, uint32_t lib_handle)
{
    intptr_t addr = resolve_sym(pid, lib_handle, sym->name);
    uint8_t small[READ_SMALL];
    uint8_t window[READ_WINDOW];
    uint8_t hid_window[HID_WINDOW];
    char hex[3 * 32 + 1];
    int got_small = -1;
    int got_window = -1;

    reportf(" SYMBOL process=%s pid=%d lib=%s name=%s addr=0x%lx\n",
            proc_name, pid, lib_name, sym->name, (unsigned long)addr);

    if (!addr) return;

    memset(small, 0, sizeof(small));
    got_small = read_remote(pid, addr, small, sizeof(small));
    if (got_small == 0) {
        hex_bytes(hex, sizeof(hex), small, 32);
        reportf("  PROLOGUE32 %s\n", hex);
        reportf("  HASH256 fnv1a64=0x%016llx\n",
                (unsigned long long)fnv1a64(small, sizeof(small)));
    } else {
        reportf("  READ256 failed errno=%d\n", errno);
    }

    if (sym->is_vda) {
        memset(window, 0, sizeof(window));
        got_window = read_remote(pid, addr, window, sizeof(window));
        if (got_window == 0) {
            reportf("  HASH4K fnv1a64=0x%016llx\n",
                    (unsigned long long)fnv1a64(window, sizeof(window)));
            for (size_t off = 0; off < 256; off += 16) {
                hex_bytes(hex, sizeof(hex), window + off, 16);
                reportf("  VDA_HEX +0x%03zx %s\n", off, hex);
            }
            scan_vda_candidates(window, sizeof(window));
        } else {
            reportf("  READ4K failed errno=%d\n", errno);
        }
    }

    if (strcmp(lib_name, "libSceHidControl") == 0 &&
        strcmp(sym->name, "sceHidControlGetDeviceInfo") == 0) {
        memset(hid_window, 0, sizeof(hid_window));
        int got_hid = read_remote(pid, addr, hid_window, sizeof(hid_window));
        if (got_hid == 0) {
            reportf("  HASH1K fnv1a64=0x%016llx\n",
                    (unsigned long long)fnv1a64(hid_window, sizeof(hid_window)));
            scan_hidcontrol_get_device_info(hid_window, sizeof(hid_window));
        } else {
            reportf("  HIDCONTROL_READ1K failed errno=%d\n", errno);
        }
    }
}

static void
probe_library(pid_t pid, const char *proc_name, const ProbeLibrary *lib)
{
    uint32_t handle = 0;
    intptr_t init_addr = 0;
    intptr_t fini_addr = 0;

    if (get_lib_handle(pid, lib->name, &handle) != 0) {
        reportf(" LIB process=%s pid=%d name=%s handle=0x0 status=missing\n",
                proc_name, pid, lib->name);
        return;
    }

    init_addr = kernel_dynlib_init_addr(pid, handle);
    fini_addr = kernel_dynlib_fini_addr(pid, handle);
    reportf(" LIB process=%s pid=%d name=%s handle=0x%x init=0x%lx fini=0x%lx\n",
            proc_name, pid, lib->name, handle,
            (unsigned long)init_addr, (unsigned long)fini_addr);

    for (size_t i = 0; i < lib->symbol_count; i++) {
        probe_symbol(pid, proc_name, lib->name, &lib->symbols[i], handle);
    }
}

static void
probe_process(const char *proc_name)
{
    pid_t pids[MAX_PIDS];
    size_t n = find_pids_by_tdname(proc_name, pids, MAX_PIDS);
    reportf("\nPROCESS name=%s count=%zu\n", proc_name, n);
    if (n == 0) return;

    for (size_t i = 0; i < n; i++) {
        reportf(" PROCESS_INSTANCE name=%s pid=%d index=%zu\n", proc_name, pids[i], i);
        for (size_t j = 0; j < sizeof(g_libraries) / sizeof(g_libraries[0]); j++) {
            probe_library(pids[i], proc_name, &g_libraries[j]);
        }
    }
}

static void
save_report_files(void)
{
    int data_ok = -1;
    int usb_ok = -1;

    if (g_report_len == 0) return;
    if (ensure_dir(REPORT_DIR_DATA) == 0) {
        data_ok = write_file_full(REPORT_PATH_DATA, g_report, g_report_len);
    }
    usb_ok = write_file_full(REPORT_PATH_USB, g_report, g_report_len);

    reportf("\nREPORT_SAVED data_path=%s data_status=%d usb_path=%s usb_status=%d\n",
            REPORT_PATH_DATA, data_ok, REPORT_PATH_USB, usb_ok);
}

static void
serve_report(void)
{
    int s = -1;
    int opt = 1;
    struct sockaddr_in addr;
    struct timeval tv;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        reportf("\nREPORT_SERVER socket failed errno=%d\n", errno);
        return;
    }

    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(VDA_PROBE_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        reportf("\nREPORT_SERVER bind port=%d failed errno=%d\n", VDA_PROBE_PORT, errno);
        close(s);
        return;
    }
    if (listen(s, 4) != 0) {
        reportf("\nREPORT_SERVER listen failed errno=%d\n", errno);
        close(s);
        return;
    }

    tv.tv_sec = 10;
    tv.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    reportf("\nREPORT_READY tcp_port=%d bytes=%zu serve_seconds=%d\n",
            VDA_PROBE_PORT, g_report_len, VDA_PROBE_SERVE_SECONDS);
    reportf("REPORT_FETCH nc <console-ip> %d > vda-probe-report.txt\n", VDA_PROBE_PORT);

    for (int waited = 0; waited < VDA_PROBE_SERVE_SECONDS; waited += 10) {
        int c = accept(s, NULL, NULL);
        if (c < 0) continue;
        size_t sent = 0;
        while (sent < g_report_len) {
            ssize_t n = send(c, g_report + sent, g_report_len - sent, 0);
            if (n <= 0) break;
            sent += (size_t)n;
        }
        close(c);
        reportf("REPORT_SENT bytes=%zu\n", sent);
    }

    close(s);
}

int
main(void)
{
    notify_user("Ghostpad VDA Probe: starting read-only scan");
    reportf("Ghostpad VDA Probe v2-eap-adapted\n");
    reportf("PLATFORM %s\n", GP_PLATFORM);
#ifndef __ORBIS__
    {
        struct utsname uts;
        memset(&uts, 0, sizeof(uts));
        uname(&uts);
        reportf("UNAME sysname=%s release=%s version=%s machine=%s\n",
                uts.sysname, uts.release, uts.version, uts.machine);
    }
#else
    reportf("UNAME kernel_version=Orbis\n");
#endif
    reportf("SELF pid=%d\n", getpid());
    reportf("SAFETY read_only=1 writes_target_memory=0 installs_hooks=0\n");
    reportf("MBUS_SYMBOL_SWEEP resolve_only=1 candidate_count=%u\n",
            (unsigned)(sizeof(g_mbus_symbols) / sizeof(g_mbus_symbols[0])));
    reportf("BT_SYMBOL_SWEEP resolve_only=1 candidate_count=%u note=no_calls_no_patches\n",
            (unsigned)(sizeof(g_bt_symbols) / sizeof(g_bt_symbols[0])));

    probe_process("SceShellCore");
    probe_process("SceShellUI");

    /* Extra process names are best-effort.  Most retail builds appear to route
     * controller events through ShellCore/ShellUI and MBus, but probing these
     * thread names helps confirm whether a separate BT/HID service is visible. */
    probe_process("SceSysCore");
    probe_process("SceHidControl");
    probe_process("SceBt");
    probe_process("SceBluetooth");
    probe_process("SceBtStack");
    probe_process("SceBluetoothService");

    save_report_files();
    notify_user("Ghostpad VDA Probe: report ready on TCP 6975 and /data/ghostpad");
    serve_report();
    return 0;
}
