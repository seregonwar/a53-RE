#include "a53_abi.h"
#include "a53_context.h"

/*
 * Register bit-name tables.
 * Each entry: bit value (0 = field-type entry), bitmask, string name.
 * Terminated by bit=0, mask=0, name=NULL.
 * el3_reg_bit_name32 is defined in a53_context.h.
 */
typedef struct {
    a53_u64 bit;
    a53_u64 mask;
    const char *name;
} el3_reg_bit_name64;

/* clang-format off */
static const el3_reg_bit_name32 g_reg_SPSR_EL1[] = {
    { 0x80000000, 0, "N" },
    { 0x40000000, 0, "Z" },
    { 0x20000000, 0, "C" },
    { 0x10000000, 0, "V" },
    { 0x00100000, 0, "SS" },
    { 0x00000200, 0, "IL" },
    { 0x00000100, 0, "DIT" },
    { 0x00000080, 0, "UAO" },
    { 0x00000040, 0, "PAN" },
    { 0x00000000, 0x001f0000, "SSBS" },
    { 0x00000004, 0x001f0004, "BTYPE" },
    { 0x00000005, 0x001f0005, "BTYPE" },
    { 0x00000000, 0x0000001f, "M" },
    { 0, 0, NULL },
};

static const el3_reg_bit_name32 g_reg_SPSR_EL2[] = {
    { 0x80000000, 0, "N" },
    { 0x40000000, 0, "Z" },
    { 0x20000000, 0, "C" },
    { 0x10000000, 0, "V" },
    { 0x00100000, 0, "SS" },
    { 0x00000200, 0, "IL" },
    { 0x00000100, 0, "DIT" },
    { 0x00000080, 0, "UAO" },
    { 0x00000040, 0, "PAN" },
    { 0x00000000, 0x001f0000, "SSBS" },
    { 0x00000004, 0x001f0004, "BTYPE" },
    { 0x00000005, 0x001f0005, "BTYPE" },
    { 0x00000008, 0x001f0008, "BTYPE" },
    { 0x00000009, 0x001f0009, "BTYPE" },
    { 0x00000000, 0x0000001f, "M" },
    { 0, 0, NULL },
};

static const el3_reg_bit_name32 g_reg_SPSR_EL3[] = {
    { 0x80000000, 0, "N" },
    { 0x40000000, 0, "Z" },
    { 0x20000000, 0, "C" },
    { 0x10000000, 0, "V" },
    { 0x00100000, 0, "SS" },
    { 0x00000200, 0, "IL" },
    { 0x00000100, 0, "DIT" },
    { 0x00000080, 0, "UAO" },
    { 0x00000040, 0, "PAN" },
    { 0x0000000d, 0x001f000d, "BTYPE" },
    { 0x0000000c, 0x001f000c, "BTYPE" },
    { 0x00000009, 0x001f0009, "BTYPE" },
    { 0x00000008, 0x001f0008, "BTYPE" },
    { 0x00000005, 0x001f0005, "BTYPE" },
    { 0x00000004, 0x001f0004, "BTYPE" },
    { 0x00000000, 0x0000001f, "M" },
    { 0, 0, NULL },
};

static const el3_reg_bit_name32 g_reg_ESR_ELx[] = {
    { 0x00000000, 0xfc000000, "EC" },
    { 0x38000000, 0xfc000000, "EC" },
    { 0x54000000, 0xfc000000, "EC" },
    { 0x5c000000, 0xfc000000, "EC" },
    { 0x80000000, 0xfc000000, "EC" },
    { 0x84000000, 0xfc000000, "EC" },
    { 0x90000000, 0xfc000000, "EC" },
    { 0x94000000, 0xfc000000, "EC" },
    { 0xf0000000, 0xfc000000, "EC" },
    { 0x00000002, 0x00000002, "IL" },
    { 0, 0, NULL },
};

static const el3_reg_bit_name64 g_reg_HCR_EL2[] = {
    { 0x0000000000000080ULL, 0, "RW" },
    { 0x0000000000000060ULL, 0, "???60" },
    { 0x0000000800000000ULL, 0, "TGE" },
    { 0x0000002000000000ULL, 0, "E2H" },
    { 0x0000001000000000ULL, 0, "DC" },
    { 0x0000000800000000ULL, 0, "???B" },
    { 0x0000000200000000ULL, 0, "API" },
    { 0, 0, NULL },
};

static const el3_reg_bit_name32 g_reg_SCR_EL3[] = {
    { 0x00000800, 0, "HCE" },
    { 0x00000400, 0, "SIF" },
    { 0x00000200, 0, "HSUE" },
    { 0x00000100, 0, "NS" },
    { 0x00000080, 0, "EA" },
    { 0x00000020, 0, "FIQ" },
    { 0x00000010, 0, "IRQ" },
    { 0x00000008, 0, "RW" },
    { 0x00000004, 0, "SMD" },
    { 0x00000002, 0, "SCE" },
    { 0x00000001, 0, "NS" },
    { 0, 0, NULL },
};

static const el3_reg_bit_name32 g_reg_SCTLR_EL1[] = {
    { 0x00008000, 0, "???" },
    { 0x00004000, 0, "???" },
    { 0x00001000, 0, "I" },
    { 0x00000400, 0, "SA0" },
    { 0x00000100, 0, "CP15BEN" },
    { 0x00100000, 0, "nTWE" },
    { 0x00080000, 0, "nTWI" },
    { 0x00000020, 0, "CP15DIS" },
    { 0x00000010, 0, "???" },
    { 0x00000008, 0, "A" },
    { 0x00000004, 0, "C" },
    { 0x00000002, 0, "WXN" },
    { 0x00000001, 0, "M" },
    { 0, 0, NULL },
};

static const el3_reg_bit_name32 g_reg_SCTLR_EL2[] = {
    { 0x00200000, 0, "???" },
    { 0x00100000, 0, "???" },
    { 0x00008000, 0, "???" },
    { 0x00004000, 0, "???" },
    { 0x00000800, 0, "???" },
    { 0x00000400, 0, "SA0" },
    { 0x00000100, 0, "???" },
    { 0x00100000, 0, "nTWE" },
    { 0x00080000, 0, "nTWI" },
    { 0x00000020, 0, "???" },
    { 0x00000010, 0, "IDEN" },
    { 0x00000008, 0, "A" },
    { 0x00000004, 0, "C" },
    { 0x00000002, 0, "WXN" },
    { 0x00000001, 0, "M" },
    { 0, 0, NULL },
};
/* clang-format on */

/*
 * Bit-name printer for 32-bit registers.
 */
void A53_SECTION(".text.el3.loader")
el3_reg_bit_name32_print(const el3_reg_bit_name32 *p, a53_u32 v)
{
    const el3_reg_bit_name32 *e;
    int first;

    first = 1;
    for (e = p; e->name != NULL; ++e) {
        a53_u32 mask;
        a53_u32 expect;

        mask = e->mask ? e->mask : e->bit;
        expect = e->bit;
        if ((v & mask) == expect) {
            if (!first) {
                printf_low(",");
            }
            first = 0;
            printf_low("%s", e->name);
            v &= ~mask;
        }
    }
    if (v != 0) {
        printf_low("[0x%08x]", v);
    }
}

/*
 * Bit-name printer for 64-bit registers.
 */
static void A53_SECTION(".text.el3.loader")
el3_reg_bit_name64_print(const el3_reg_bit_name64 *p, a53_u64 v)
{
    const el3_reg_bit_name64 *e;
    int first;

    first = 1;
    for (e = p; e->name != NULL; ++e) {
        a53_u64 mask;
        a53_u64 expect;

        mask = e->mask ? e->mask : e->bit;
        expect = e->bit;
        if ((v & mask) == expect) {
            if (!first) {
                printf_low(",");
            }
            first = 0;
            printf_low("%s", e->name);
            v &= ~mask;
        }
    }
    if (v != 0) {
        printf_low("[0x%016lx]", v);
    }
}

/* ---- Globals for cache geometry ---- */
a53_u32 g_L1D_NumSets;
a53_u32 g_L1D_Associativity;
a53_u32 g_L1I_NumSets;
a53_u32 g_L1I_Associativity;
a53_u32 g_L2D_NumSets;
a53_u32 g_L2D_Associativity;

/* ---- Helper: current exception level name ---- */
static const char *current_el_name(a53_u64 el_reg)
{
    a53_u32 idx;

    idx = (el_reg >> 2) & 3;
    /* Ghidra mapping: 0->2, 1->3, 2->0, 3->1 */
    idx ^= 2;
    switch (idx) {
    case 0: return "EL2";
    case 1: return "EL3";
    case 2: return "EL0";
    case 3: return "EL1";
    default: return "?";
    }
}

/* ------------------------------------------------------------------ */
/*  System register print functions                                    */
/* ------------------------------------------------------------------ */

void A53_SECTION(".text.el3.loader") aarch64_print_CurrentEL(void)
{
    a53_u64 el;
    a53_u32 cpu;

    __asm__("mrs %0, currentel" : "=r"(el));
    cpu = mp4_get_cpu();
    printf_low("%d:CurrentEl:      0x%08x: EL=%s\n", (a53_u64)cpu,
               (a53_u32)el, current_el_name(el));
}

void A53_SECTION(".text.el3.loader") aarch64_print_SPSR_EL1(void)
{
    a53_u64 tmp;
    a53_u32 v;
    a53_u32 cpu;

    __asm__("mrs %0, spsr_el1" : "=r"(tmp));
    v = (a53_u32)tmp;
    cpu = mp4_get_cpu();
    printf_low("%d:%-16s = 0x%08x:", (a53_u64)cpu, "SPSR_EL1", (a53_u64)v);
    el3_reg_bit_name32_print(g_reg_SPSR_EL1, v);
    printf_low("\n");
}

void A53_SECTION(".text.el3.loader") aarch64_print_SPSR_EL2(void)
{
    a53_u64 tmp;
    a53_u32 v;
    a53_u32 cpu;

    __asm__("mrs %0, spsr_el2" : "=r"(tmp));
    v = (a53_u32)tmp;
    cpu = mp4_get_cpu();
    printf_low("%d:%-16s = 0x%08x:", (a53_u64)cpu, "SPSR_EL2", (a53_u64)v);
    el3_reg_bit_name32_print(g_reg_SPSR_EL2, v);
    printf_low("\n");
}

void A53_SECTION(".text.el3.loader") aarch64_print_SPSR_EL3(void)
{
    a53_u64 v;
    a53_u32 cpu;

    __asm__("mrs %0, spsr_el3" : "=r"(v));
    cpu = mp4_get_cpu();
    printf_low("%d:%-16s = 0x%08x:", (a53_u64)cpu, "SPSR_EL3", (a53_u32)v);
    el3_reg_bit_name32_print(g_reg_SPSR_EL3, (a53_u32)v);
    printf_low("\n");
}

void A53_SECTION(".text.el3.loader") aarch64_print_ESR_EL1(void)
{
    a53_u64 tmp;
    a53_u32 v;
    a53_u32 cpu;

    __asm__("mrs %0, esr_el1" : "=r"(tmp));
    v = (a53_u32)tmp;
    cpu = mp4_get_cpu();
    printf_low("%d:%-16s = 0x%08x:", (a53_u64)cpu, "ESR_EL1", (a53_u64)v);
    el3_reg_bit_name32_print(g_reg_ESR_ELx, v);
    printf_low("\n");
}

void A53_SECTION(".text.el3.loader") aarch64_print_ESR_EL2(void)
{
    a53_u64 tmp;
    a53_u32 v;
    a53_u32 cpu;

    __asm__("mrs %0, esr_el2" : "=r"(tmp));
    v = (a53_u32)tmp;
    cpu = mp4_get_cpu();
    printf_low("%d:%-16s = 0x%08x:", (a53_u64)cpu, "ESR_EL2", (a53_u64)v);
    el3_reg_bit_name32_print(g_reg_ESR_ELx, v);
    printf_low("\n");
}

void A53_SECTION(".text.el3.loader") aarch64_print_ESR_EL3(void)
{
    a53_u64 tmp;
    a53_u32 v;
    a53_u32 cpu;

    __asm__("mrs %0, esr_el3" : "=r"(tmp));
    v = (a53_u32)tmp;
    cpu = mp4_get_cpu();
    printf_low("%d:%-16s = 0x%08x:", (a53_u64)cpu, "ESR_EL3", (a53_u64)v);
    el3_reg_bit_name32_print(g_reg_ESR_ELx, v);
    printf_low("\n");
}

void A53_SECTION(".text.el3.loader")
aarch64_print_ISS_instruction_abort(a53_u32 iss)
{
    a53_u32 cpu;

    cpu = mp4_get_cpu();
    printf_low("%d:%s:SET =0x%08x\n", (a53_u64)cpu,
               "aarch64_print_ISS_instruction_abort", (a53_u64)(iss & 0x1800));
    printf_low("%d:%s:IFSC=0x%08x\n", (a53_u64)cpu,
               "aarch64_print_ISS_instruction_abort", (a53_u64)(iss & 0x1f));
    if (iss == 0x10) {
        printf_low("%d:%s:AARCH64_ESR_ELx_ISS_IFSC_SYNCHRONOUS_EXTERNAL_ABORT"
                   "_NOT_ON_TRANS\n",
                   (a53_u64)cpu, "aarch64_print_ISS_instruction_abort");
        return;
    }
    printf_low("%d:%s:IFSC=0x%08x\n", (a53_u64)cpu,
               "aarch64_print_ISS_instruction_abort", (a53_u64)(iss & 0x1f));
}

void A53_SECTION(".text.el3.loader") aarch64_print_HCR_EL2(void)
{
    a53_u64 v;
    a53_u32 cpu;

    __asm__("mrs %0, hcr_el2" : "=r"(v));
    cpu = mp4_get_cpu();
    printf_low("%d:%-16s = 0x%016lx:", (a53_u64)cpu, "HCR_EL2", v);
    el3_reg_bit_name64_print(g_reg_HCR_EL2, v);
    printf_low("\n");
}

void A53_SECTION(".text.el3.loader") aarch64_print_SCR_EL3(void)
{
    a53_u64 v;
    a53_u32 cpu;

    __asm__("mrs %0, scr_el3" : "=r"(v));
    cpu = mp4_get_cpu();
    printf_low("%d:%-16s = 0x%08x:", (a53_u64)cpu, "SCR_EL3", (a53_u32)v);
    el3_reg_bit_name32_print(g_reg_SCR_EL3, (a53_u32)v);
    printf_low("\n");
}

void A53_SECTION(".text.el3.loader") aarch64_print_SCTLR_EL1(void)
{
    a53_u64 v;
    a53_u32 cpu;

    __asm__("mrs %0, sctlr_el1" : "=r"(v));
    cpu = mp4_get_cpu();
    printf_low("%d:%-16s = 0x%08x:", (a53_u64)cpu, "SCTLR_EL1", (a53_u32)v);
    el3_reg_bit_name32_print(g_reg_SCTLR_EL1, (a53_u32)v);
    printf_low("\n");
}

void A53_SECTION(".text.el3.loader") aarch64_print_SCTLR_EL2(void)
{
    a53_u64 v;
    a53_u32 cpu;

    __asm__("mrs %0, sctlr_el2" : "=r"(v));
    cpu = mp4_get_cpu();
    printf_low("%d:%-16s = 0x%08x:", (a53_u64)cpu, "SCTLR_EL2", (a53_u32)v);
    el3_reg_bit_name32_print(g_reg_SCTLR_EL2, (a53_u32)v);
    printf_low("\n");
}

void A53_SECTION(".text.el3.loader") aarch64_print_SCTLR_EL3(void)
{
    a53_u64 v;
    a53_u32 cpu;

    __asm__("mrs %0, sctlr_el3" : "=r"(v));
    cpu = mp4_get_cpu();
    printf_low("%d:%-16s = 0x%08x:", (a53_u64)cpu, "SCTLR_EL3", (a53_u32)v);
    el3_reg_bit_name32_print(g_reg_SCTLR_EL2, (a53_u32)v);
    printf_low("\n");
}

a53_u64 A53_SECTION(".text.el3.loader") aarch64_read_ELR(void)
{
    a53_u64 v;
    __asm__("mrs %0, elr_el3" : "=r"(v));
    return v;
}

a53_u64 A53_SECTION(".text.el3.loader") aarch64_read_ESR(void)
{
    a53_u64 v;
    __asm__("mrs %0, esr_el3" : "=r"(v));
    return v;
}

a53_u64 A53_SECTION(".text.el3.loader") aarch64_read_FAR(void)
{
    a53_u64 v;
    __asm__("mrs %0, far_el3" : "=r"(v));
    return v;
}

a53_u64 A53_SECTION(".text.el3.loader")
aarch64_address_translation_read(void *va)
{
    a53_u64 result;

    __asm__("at s1e3r, %1; mrs %0, par_el1"
            : "=r"(result)
            : "r"((a53_u64)va)
            : "memory");
    return result;
}

a53_u64 A53_SECTION(".text.el3.loader")
aarch64_address_translation_write(void *va)
{
    a53_u64 result;

    __asm__("at s1e3w, %1; mrs %0, par_el1"
            : "=r"(result)
            : "r"((a53_u64)va)
            : "memory");
    return result;
}

void A53_SECTION(".text.el3.loader") aarch64_ccahe_op_init(void)
{
    a53_u64 clidr;
    a53_u64 ccsidr;
    a53_u32 cpu;

    __asm__("mrs %0, clidr_el1" : "=r"(clidr));
    cpu = mp4_get_cpu();
    printf_low("%d:%s:CLIDR_EL1=0x%016lx\n", (a53_u64)cpu,
               "aarch64_ccahe_op_init", clidr);

    __asm__("msr csselr_el1, xzr");
    __asm__("isb");
    __asm__("mrs %0, ccsidr_el1" : "=r"(ccsidr));
    g_L1D_NumSets = (a53_u32)((ccsidr >> 13) & 0x7fff);
    g_L1D_Associativity = (a53_u32)((ccsidr >> 3) & 0x3ff);

    __asm__("msr csselr_el1, %0" : : "r"(1UL));
    __asm__("isb");
    __asm__("mrs %0, ccsidr_el1" : "=r"(ccsidr));
    g_L1I_NumSets = (a53_u32)((ccsidr >> 13) & 0x7fff);
    g_L1I_Associativity = (a53_u32)((ccsidr >> 3) & 0x3ff);

    __asm__("msr csselr_el1, %0" : : "r"(2UL));
    __asm__("isb");
    __asm__("mrs %0, ccsidr_el1" : "=r"(ccsidr));
    g_L2D_NumSets = (a53_u32)((ccsidr >> 13) & 0x7fff);
    g_L2D_Associativity = (a53_u32)((ccsidr >> 3) & 0x3ff);

    cpu = mp4_get_cpu();
    printf_low("%d:%s:g_L1D_NumSets       = 0x%08x\n", (a53_u64)cpu,
               "aarch64_ccahe_op_init", (a53_u64)g_L1D_NumSets);
    printf_low("%d:%s:g_L1D_Associativity = 0x%08x\n", (a53_u64)cpu,
               "aarch64_ccahe_op_init", (a53_u64)g_L1D_Associativity);
    printf_low("%d:%s:g_L1I_NumSets       = 0x%08x\n", (a53_u64)cpu,
               "aarch64_ccahe_op_init", (a53_u64)g_L1I_NumSets);
    printf_low("%d:%s:g_L1I_Associativity = 0x%08x\n", (a53_u64)cpu,
               "aarch64_ccahe_op_init", (a53_u64)g_L1I_Associativity);
    printf_low("%d:%s:g_L2D_NumSets       = 0x%08x\n", (a53_u64)cpu,
               "aarch64_ccahe_op_init", (a53_u64)g_L2D_NumSets);
    printf_low("%d:%s:g_L2D_Associativity = 0x%08x\n", (a53_u64)cpu,
               "aarch64_ccahe_op_init", (a53_u64)g_L2D_Associativity);
}

/*
 * The cloned function recovers cache-ops-by-set/way with two variants.
 * Original names: aarch64_CISW_all, aarch64_cache_op_set_way.
 */
static void A53_SECTION(".text.el3.loader")
DC_CISW(a53_u64 val)
{
    __asm__ volatile("dc cisw, %0" : : "r"(val));
}

int A53_SECTION(".text.el3.loader")
aarch64_cache_op_set_way(cache_op_sw_type_t op, a53_u32 level)
{
    a53_u32 nsets;
    a53_u32 assoc;
    a53_u64 set;
    int way;

    if (op == cache_op_csw) {
        nsets = g_L1D_NumSets;
        assoc = g_L1D_Associativity;
    } else {
        nsets = g_L2D_NumSets;
        assoc = g_L2D_Associativity;
    }

    for (set = 0; set != (a53_u64)(nsets + 1); ++set) {
        for (way = 0; way != assoc + 1; ++way) {
            a53_u64 op_type;
            a53_u64 val;

            op_type = (op == cache_op_csw) ? 0ULL : 2ULL;
            val = op_type
                | ((set & 0x3ffffff) << 6)
                | ((a53_u64)way << ((op == cache_op_csw) ? 30 : 28));
            DC_CISW(val);
        }
    }

    /* DSB + ISB */
    __asm__ volatile("dsb sy");
    __asm__ volatile("isb");
    return op;
}

int A53_SECTION(".text.el3.loader") aarch64_CISW_all(void)
{
    aarch64_cache_op_set_way(cache_op_csw, 0);
    aarch64_cache_op_set_way(cache_op_isw, 0);
    return 0;
}
