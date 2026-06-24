#ifndef A53_CONTEXT_H
#define A53_CONTEXT_H

#include <stddef.h>

#include "a53_abi.h"

typedef struct mp4_debug_status {
    a53_u64 mds_magic1;
    a53_u64 mds_vector;
    a53_u64 mds_gpr[31];
    a53_u64 mds_sp;
    a53_u64 mds_esr_el3;
    a53_u64 mds_elr_el3;
    a53_u64 mds_far_el3;
    a53_u64 mds_elr_mode;
    a53_u64 mds_current_el;
    a53_u64 mds_daif;
    a53_u64 mds_nzcv;
    a53_u64 mds_spsr;
    a53_u64 mds_esr;
    a53_u64 mds_far;
    a53_u64 mds_tpidrro_el0;
    a53_u64 mds_esr_el32;
    a53_u64 mds_158;
    a53_u64 mds_160;
    a53_u64 mds_168;
    a53_u64 mds_170;
    a53_u64 mds_178;
    a53_u64 mds_1st_vector;
    a53_u64 mds_1st_el;
    a53_u64 mds_1st_spsr;
    a53_u64 mds_1st_esr;
    a53_u64 mds_1st_elr;
    a53_u64 mds_1st_x0;
    a53_u64 mds_1st_x1;
    a53_u64 mds_218;
    a53_u64 mds_magic2;
    a53_u64 mds_self_size;
    a53_u64 mds_id;
    a53_u64 mds_version;
    a53_u64 mds_mbox_t2c_count;
    a53_u64 mds_mbox_t2c;
    a53_u64 mds_ttyp_buffer_offset;
    a53_u64 mds_ttyp_buffer_size;
    a53_u64 mds_ttyp_buffer_last;
    a53_u64 mds_ttyp_buffer_count;
    a53_u64 mds_phase;
    a53_u64 mds_magic3;
} mp4_debug_status_t;

typedef struct sttyp_putchar_context sttyp_putchar_context_t;
typedef int (*sttyp_putchar_begin_t)(sttyp_putchar_context_t *context);
typedef int (*sttyp_putchar_char_t)(sttyp_putchar_context_t *context, int character);
typedef int (*sttyp_putchar_end_t)(sttyp_putchar_context_t *context);

struct sttyp_putchar_context {
    a53_u32 spc_cpu;
    char *spc_buf;
    a53_u64 spc_size;
    a53_u64 spc_count;
    mp4_debug_status_t *spc_mds_el0;
    a53_u8 *spc_pericom;
    sttyp_putchar_begin_t spc_begin;
    sttyp_putchar_char_t spc_putchar;
    sttyp_putchar_end_t spc_end;
};

typedef struct dev_context {
    mp4_debug_status_t *dc_debug_status;
    struct dev_context *dc_dev_context_el0;
    struct dev_context *dc_dev_context_el1;
    struct dev_context *dc_dev_context_el2;
    a53_u32 dc_exception_nest;
    int (*dc_putchar_low_hook)(int character);
    sttyp_putchar_context_t *dc_sttyp_putchar_context;
} dev_context_t;

_Static_assert(sizeof(mp4_debug_status_t) == 568, "DWARF layout mismatch: mp4_debug_status_t");
_Static_assert(offsetof(mp4_debug_status_t, mds_gpr) == 16, "DWARF offset mismatch: mds_gpr");
_Static_assert(offsetof(mp4_debug_status_t, mds_ttyp_buffer_offset) == 520, "DWARF offset mismatch: ttyp offset");
_Static_assert(sizeof(sttyp_putchar_context_t) == 72, "DWARF layout mismatch: sttyp_putchar_context_t");
_Static_assert(offsetof(sttyp_putchar_context_t, spc_pericom) == 40, "DWARF offset mismatch: spc_pericom");
_Static_assert(sizeof(dev_context_t) == 56, "DWARF layout mismatch: dev_context_t");
_Static_assert(offsetof(dev_context_t, dc_putchar_low_hook) == 40, "DWARF offset mismatch: dc_putchar_low_hook");

dev_context_t *get_dev_context(void);
int spc_begin(sttyp_putchar_context_t *context);
int spc_putchar(sttyp_putchar_context_t *context, int character);
int spc_end(sttyp_putchar_context_t *context);

#endif
