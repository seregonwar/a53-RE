#ifndef A53_LOADER_H
#define A53_LOADER_H

#include <stddef.h>

#include "a53_abi.h"

/* =========================================================================
 * Core debug / context types
 * ========================================================================= */

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

/* DWARF layout verification */
_Static_assert(sizeof(mp4_debug_status_t) == 568, "DWARF layout mismatch: mp4_debug_status_t");
_Static_assert(offsetof(mp4_debug_status_t, mds_gpr) == 16, "DWARF offset mismatch: mds_gpr");
_Static_assert(offsetof(mp4_debug_status_t, mds_ttyp_buffer_offset) == 520, "DWARF offset mismatch: ttyp offset");
_Static_assert(sizeof(sttyp_putchar_context_t) == 72, "DWARF layout mismatch: sttyp_putchar_context_t");
_Static_assert(offsetof(sttyp_putchar_context_t, spc_pericom) == 40, "DWARF offset mismatch: spc_pericom");
_Static_assert(sizeof(dev_context_t) == 56, "DWARF layout mismatch: dev_context_t");
_Static_assert(offsetof(dev_context_t, dc_putchar_low_hook) == 40, "DWARF offset mismatch: dc_putchar_low_hook");

/* =========================================================================
 * Boot / layout types
 * ========================================================================= */

typedef int (*printf_func_t)(char *format, ...);

typedef struct el3_jmp_buf el3_jmp_buf;

typedef struct layout_section {
    a53_u64 msi_begin;
    a53_u64 msi_end;
    a53_u64 msi_page_size;
    a53_u64 msi_sram;
    a53_u64 msi_pa;
} layout_section_t;

typedef struct layout {
    layout_section_t msl_loader_el3_text;
    layout_section_t msl_loader_el3_data;
    layout_section_t msl_loader_dev_text;
    layout_section_t msl_loader_dev_data;
    layout_section_t msl_loader_el2;
    layout_section_t msl_loader_el1;
    layout_section_t msl_controller_dev_text;
    layout_section_t msl_controller_dev_data;
} layout_t;

typedef struct el3_param {
    void            *core0_main;
    dev_context_t   *core0_dev_context;
    a53_u64         *log0;
    void            *core1_main;
    dev_context_t   *core1_dev_context;
    a53_u64         *log1;
} el3_param_t;

/* =========================================================================
 * dev_context functions
 * ========================================================================= */

dev_context_t *get_dev_context(void);
int dev_context_init(dev_context_t *dc, sttyp_putchar_context_t *spc,
    a53_u32 cpu, a53_u32 cp_param2,
    char *buf, a53_u64 len, mp4_debug_status_t *mds);
int dev_context_init_for_el3(dev_context_t *context);
int spc_begin(sttyp_putchar_context_t *context);
int spc_putchar(sttyp_putchar_context_t *context, int character);
int spc_end(sttyp_putchar_context_t *context);

/* =========================================================================
 * debug_status functions
 * ========================================================================= */

mp4_debug_status_t *mp4_debug_status_get(void);
a53_u64 mp4_debug_status_get_reg(a53_u32 regid);
int mp4_debug_status_get_frame(struct aarch64_frame *af);
void mp4_debug_status_show(void);
int mp4_debug_status_putchar(int c);
void mp4_debug_status_init(void);
void mp4_debug_status_exit(void);
void mp4_debug_status_c_set(void);

/* =========================================================================
 * putchar / printf system
 * ========================================================================= */

int putchar(int c);
sttyp_putchar_context_t *sttyp_putchar_context_get(void);
void set_sttyp_putchar_context(sttyp_putchar_context_t *spc);
int putchar_sttyp_end(void);
int putchar_el0_direct(int c);
int putchar_sttyp(int c);
int putchar_sttyp_begin(void);
int putchar_titania_uart_el0(int c);
int putchar_titania_uart_el3(int c);
int putchar_low(int c);
int putchar_cp(int c);
int putchar_pericom(int c);
void pericom_putchar(a53_u8 *base, int c);

int printf_low(char *format, ...);
int printf_cp(char *format, ...);
int printf_sttyp(char *format, ...);
int printf_titania_uart_el0(char *format, ...);
int write_EL3(char *msg, a53_u64 len);
int puts_EL3(char *msg);

int putchar_sttyp_hook(void *pfd, int ch);
int putchar_titania_uart_hook_el0(void *pfd, int ch);
int putchar_low_hook(void *pfd, int ch);
int putchar_cp_hook(void *pfd, int ch);

/* =========================================================================
 * Boot function prototypes
 * ========================================================================= */

void el3_print_common(void);
void *el3_jmpbuf_enter(el3_jmp_buf *n);
void el3_jmpbuf_exit(el3_jmp_buf *n);
int el3_boot(a53_u64 *log, printf_func_t printf_func, a53_u32 cp_param2);
void el3_serror_handler(a53_u64 x0, a53_u64 vector);

/* =========================================================================
 * SVC handler
 * ========================================================================= */

int svc_EL3(a53_u32 esr_el1, mp4_debug_status_t *status);

/* =========================================================================
 * Peripheral initialization
 * ========================================================================= */

int arm_timer_init(void);
int cp_param_check(a53_u32 bit);
int cp_param_init(void);
a53_u32 dvm_read_mailbox(a53_u32 no);
int dvm_init(void);
a53_u32 mp4_timer_get_cnt(a53_u32 id);
int mp4_timer_init(void);
int smnif_init(void);

/* =========================================================================
 * Utility
 * ========================================================================= */

void el3_assert(char *file, char *func, a53_u32 line, int c, char *cstr);
a53_u32 mp4_get_cpu(void);
char *mp4_basename(char *f);

/* =========================================================================
 * Global dev_context instances (defined in boot.c / el0_support.c)
 * ========================================================================= */

extern dev_context_t g_dev_context_el3_core0;
extern dev_context_t g_dev_context_el3_core1;
extern dev_context_t g_dev_context_mm;
extern dev_context_t g_dev_context_io;

#endif
