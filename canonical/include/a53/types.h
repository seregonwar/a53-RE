#ifndef A53_TYPES_H
#define A53_TYPES_H

#include "a53_abi.h"

/* ---- AArch64 frame descriptor ---- */
typedef struct aarch64_frame {
    a53_u64 af_pc;
    a53_u64 af_fp;
    a53_u64 af_sp;
} aarch64_frame_t;

/* ---- Register bit-name table (used by el3_reg_bit_name32_print) ---- */
typedef struct {
    a53_u32 bit;
    a53_u32 mask;
    const char *name;
} el3_reg_bit_name32;

/* ---- Cache operation type (set/way) ---- */
typedef enum {
    cache_op_isw = 0,
    cache_op_csw = 1,
} cache_op_sw_type_t;

/* ---- Cache geometry globals (defined in aarch64.c) ---- */
extern a53_u32 g_L1D_NumSets;
extern a53_u32 g_L1D_Associativity;
extern a53_u32 g_L1I_NumSets;
extern a53_u32 g_L1I_Associativity;
extern a53_u32 g_L2D_NumSets;
extern a53_u32 g_L2D_Associativity;

void el3_reg_bit_name32_print(const el3_reg_bit_name32 *p, a53_u32 v);
int aarch64_cache_op_set_way(cache_op_sw_type_t op, a53_u32 level);

#endif
