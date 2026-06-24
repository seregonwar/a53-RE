#ifndef A53_MMIO_H
#define A53_MMIO_H

#include "a53_abi.h"

/* ---- IOMMU map info struct (shared between msi.c, syshub.c, gic.c) ---- */
typedef struct {
    a53_u64 mimi_pa;
    a53_u64 mimi_pa_base;
    a53_u64 mimi_iommu_addr;
    a53_u64 mimi_physical_size;
    a53_u64 mimi_iommu_size;
    a53_u64 mimi_syshub_base;
} mp4_iommu_map_info_t;

/* ---- MMIO primitives ---- */
void writel(a53_u64 addr, a53_u32 val);
a53_u32 readl(a53_u64 addr);

/* ---- IOMMU map info helpers ---- */
void mp4_iommu_map_info_print(mp4_iommu_map_info_t *mimi, char *member);
void mp4_iommu_map_info_printf(mp4_iommu_map_info_t *mimi, char *name);

#endif
