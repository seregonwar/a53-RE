#ifndef A53_ABI_H
#define A53_ABI_H

/*
 * Baseline ABI definitions for hand-promoted source. All widths are verified
 * against the AArch64 DWARF ABI; do not use host-sized aliases in canonical
 * code. More specific structures belong in recovered/metadata/types.jsonl
 * until their declarations are reviewed and promoted.
 */

#include <stdint.h>

typedef uint8_t a53_u8;
typedef uint16_t a53_u16;
typedef uint32_t a53_u32;
typedef uint64_t a53_u64;
typedef int8_t a53_s8;
typedef int16_t a53_s16;
typedef int32_t a53_s32;
typedef int64_t a53_s64;
typedef a53_u64 a53_paddr_t;
typedef a53_u64 a53_vaddr_t;

#define A53_PACKED __attribute__((packed))
#define A53_ALIGNED(value) __attribute__((aligned(value)))
#if defined(__ELF__)
#define A53_SECTION(name) __attribute__((section(name)))
#else
#define A53_SECTION(name)
#endif
#define A53_NORETURN __attribute__((noreturn))
#define A53_UNUSED __attribute__((unused))

#if defined(__cplusplus)
static_assert(sizeof(a53_u64) == 8, "A53 ABI requires 64-bit u64");
#else
_Static_assert(sizeof(a53_u64) == 8, "A53 ABI requires 64-bit u64");
#endif

#endif
