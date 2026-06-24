#ifndef A53_GIC_H
#define A53_GIC_H

#include "a53_abi.h"

/* ---- GIC status struct ---- */
typedef struct {
    a53_u32 gs_count;
    a53_u32 gsd_igroupr[4];
    a53_u32 gsd_isenabler[8];
    a53_u32 gsd_ispendr[8];
    a53_u32 gsd_isactiver[8];
} gic_status;

/* ---- GIC functions ---- */
a53_u32 gic_read_GICC_IAR(void);
void     gic_write_GICC_EOIR(a53_u32 v);
a53_u32 gic_read_GICC_RPR(void);
a53_u32 gic_read_GICC_HPPIR(void);
int      gic_check(void);
int      gic_init_by_1st_core(void);
int      gic_init_by_2nd_core(void);
void     gic_sgi1(void);
void     gic_core1(void);

a53_u32 gic_enable_irq(a53_u32 irq, a53_u32 cpu);
int      gic_print_reg(char *name, a53_u32 v);
int      gic_print_gicd_reg(char *name, a53_u32 offset);
int      gic_print_gicc_reg(char *name, a53_u32 offset);
a53_u32 gic_status_check(gic_status *gs);

/* ---- GICv2m ---- */
int gic_v2m_init(void);

#endif
