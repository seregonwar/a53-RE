#ifndef A53_SYSHUB_H
#define A53_SYSHUB_H

#include "a53_abi.h"
#include "mmio.h"  /* mp4_iommu_map_info_t */

/* ---- Syshub IOMMU TLB management ---- */
int  syshub_init(void);
int  syshub_init_after_main_param(void);
int  syshub_init_sdma(void);
int  syshub_init_for_io(a53_u64 base);
void syshub_tlb_get(a53_u32 tlb, a53_u32 *tlb0, a53_u32 *tlb1,
                    a53_u32 *tlb2, a53_u32 *tlb3, a53_u32 *sub, a53_u32 *attr1);

#endif
