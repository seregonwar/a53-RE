#include "a53_abi.h"
#include "a53_context.h"

mp4_debug_status_t *A53_SECTION(".text.el3.loader") mp4_debug_status_get(void)
{
    dev_context_t *dc;

    dc = get_dev_context();
    return dc->dc_debug_status;
}

a53_u64 A53_SECTION(".text.el3.loader") mp4_debug_status_get_reg(a53_u32 regid)
{
    dev_context_t *dc;
    mp4_debug_status_t *ds;

    dc = get_dev_context();
    ds = dc->dc_debug_status;
    if (regid < 0x1f) {
        return ds->mds_gpr[regid];
    }
    switch (regid) {
    case 0xc01c: return ds->mds_far;
    case 0x21:   return ds->mds_elr_mode;
    case 0x1001: return ds->mds_current_el;
    case 0x1002: return ds->mds_daif;
    case 0x100a: return ds->mds_nzcv;
    case 0xc010: return ds->mds_spsr;
    case 0xc01b: return ds->mds_esr;
    case 0x1f:   return ds->mds_sp;
    default:     return 0xffffffffffffffffULL;
    }
}

int A53_SECTION(".text.el3.loader") mp4_debug_status_get_frame(aarch64_frame_t *af)
{
    dev_context_t *dc;
    mp4_debug_status_t *ds;

    dc = get_dev_context();
    ds = dc->dc_debug_status;
    af->af_pc = ds->mds_gpr[0x1d];
    af->af_sp = ds->mds_sp;
    af->af_fp = ds->mds_gpr[0x1d];
    return 0;
}

void A53_SECTION(".text.el3.loader") mp4_debug_status_show(void)
{
    dev_context_t *dc;
    mp4_debug_status_t *ds;
    a53_u64 i;

    dc = get_dev_context();
    ds = dc->dc_debug_status;
    printf_low("mds_vector  = 0x%016lx\n", ds->mds_vector);
    for (i = 0; i < 0x1f; ++i) {
        if ((i & 3) == 0) {
            printf_low("mds_gpr[%2d] =", (a53_u32)i);
        }
        printf_low(" 0x%016lx", ds->mds_gpr[i]);
        if ((i & 3) == 3 || i == 0x1e) {
            printf_low("\n");
        }
    }
    printf_low("mds_sp      = 0x%016lx\n", ds->mds_sp);
}

int A53_SECTION(".text.el3.loader") mp4_debug_status_putchar(int c)
{
    dev_context_t *dc;
    mp4_debug_status_t *ds;
    a53_u8 *buf;

    dc = get_dev_context();
    ds = dc->dc_debug_status;
    buf = (a53_u8 *)((a53_u64)ds + ds->mds_ttyp_buffer_offset);
    buf[ds->mds_ttyp_buffer_last] = (a53_u8)c;
    ++ds->mds_ttyp_buffer_last;
    if (ds->mds_ttyp_buffer_last == ds->mds_ttyp_buffer_size) {
        ds->mds_ttyp_buffer_last = 0;
        ++ds->mds_ttyp_buffer_count;
    }
    return c;
}

void A53_SECTION(".text.el3.loader") mp4_debug_status_init(void)
{
    dev_context_t *dc;
    mp4_debug_status_t *ds;
    a53_u64 i;
    a53_u8 cpuid;

    dc = get_dev_context();
    cpuid = (a53_u8)mp4_get_cpu();
    ds = (mp4_debug_status_t *)(cpuid ? 0xec100000ULL : 0xec000000ULL);
    dc->dc_debug_status = ds;

    ds->mds_magic1 = 0xcbb3d18a1aa5daefULL;
    ds->mds_vector = 0;
    for (i = 0; i < 31; ++i) {
        ds->mds_gpr[i] = 0;
    }
    ds->mds_spsr = 0;
    ds->mds_esr = 0;
    ds->mds_far = 0;
    ds->mds_tpidrro_el0 = 0;
    ds->mds_1st_vector = 0;
    ds->mds_1st_el = 0;
    ds->mds_1st_spsr = 0;
    ds->mds_1st_esr = 0;
    ds->mds_1st_elr = 0;
    ds->mds_self_size = 0x238;
    ds->mds_daif = (a53_u64)cpuid;
    ds->mds_218 = 0;
    ds->mds_magic2 = 0x1aa5daef675a1801ULL;
    ds->mds_ttyp_buffer_offset = 0x1000;
    ds->mds_ttyp_buffer_last = 0;
    ds->mds_mbox_t2c = 0x100000000ULL;
    ds->mds_mbox_t2c_count = 0;
    ds->mds_ttyp_buffer_size = 0xf0000;
    ds->mds_ttyp_buffer_count = 0;
    ds->mds_phase = 0;
    ds->mds_version = 0x400000002ULL;
    ds->mds_magic3 = 0x8501dda72c7b400eULL;

    putchar_low('M');
    putchar_low('P');
    putchar_low('4');
    putchar_low('!');
    putchar_low('\n');
    mp4_debug_status_putchar('M');
    mp4_debug_status_putchar('P');
    mp4_debug_status_putchar('4');
    mp4_debug_status_putchar('\r');
    mp4_debug_status_putchar('\n');

    printf_low("Hello MP4/A53!\n");
    printf_cp("Hello MP4/A53!\n");
    printf_low("VBAR_EL3 0x%016lx\n", (a53_u64)dc->dc_debug_status->mds_vector);
}

void A53_SECTION(".text.el3.loader") mp4_debug_status_exit(void)
{
    dev_context_t *dc;

    dc = get_dev_context();
    dc->dc_debug_status->mds_mbox_t2c = 0xe00000000ULL;
}

void A53_SECTION(".text.el3.loader") mp4_debug_status_c_set(void)
{
    return;
}
