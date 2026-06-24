#ifndef A53_RUNTIME_H
#define A53_RUNTIME_H

#include <stdarg.h>

#include "a53_abi.h"

/* ---- printf formatter (shared by all printf variants) ---- */

int prnt(int (*pf)(void *, int), void *pfd, char *fmt0, va_list *argp);

/* =========================================================================
 * AArch64 system register helpers (print/decode)
 * ========================================================================= */

void aarch64_print_CurrentEL(void);
void aarch64_print_SPSR_EL1(void);
void aarch64_print_SPSR_EL2(void);
void aarch64_print_SPSR_EL3(void);
void aarch64_print_ESR_EL1(void);
void aarch64_print_ESR_EL2(void);
void aarch64_print_ESR_EL3(void);
void aarch64_print_ISS_instruction_abort(a53_u32 iss);
void aarch64_print_HCR_EL2(void);
void aarch64_print_SCR_EL3(void);
void aarch64_print_SCTLR_EL1(void);
void aarch64_print_SCTLR_EL2(void);
void aarch64_print_SCTLR_EL3(void);
a53_u64 aarch64_read_ELR(void);
a53_u64 aarch64_read_ESR(void);
a53_u64 aarch64_read_FAR(void);
a53_u64 aarch64_read_DAIF(void);
a53_u64 aarch64_address_translation_read(void *va);
a53_u64 aarch64_address_translation_write(void *va);

/* =========================================================================
 * Cache operations
 * ========================================================================= */

void aarch64_ccahe_op_init(void);
int aarch64_CISW_all(void);
void aarch64_DC_CVAC_range(void *base, a53_u64 vsize);
void aarch64_DC_CVAC_range_bs(void *base, a53_u64 length);

/* =========================================================================
 * PMU
 * ========================================================================= */

int dev_pmu_init(void);
int dev_pmu_setup_default(void);
void pmu_print_count(a53_u32 type, a53_u32 count);
int dev_pmu_report(void);

/* =========================================================================
 * Misc
 * ========================================================================= */

int check_consistency(void);

#endif
