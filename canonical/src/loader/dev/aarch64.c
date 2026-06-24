#include <stddef.h>

#include "a53_abi.h"

static void dc_cvac(void *addr)
{
    __asm__ volatile("dc cvac, %0" : : "r"(addr) : "memory");
}

static void dc_ivac(void *addr)
{
    __asm__ volatile("dc ivac, %0" : : "r"(addr) : "memory");
}

static void dc_civac(void *addr)
{
    __asm__ volatile("dc civac, %0" : : "r"(addr) : "memory");
}

static void dsb_sy(void)
{
    __asm__ volatile("dsb sy" : : : "memory");
}

static void isb(void)
{
    __asm__ volatile("isb" : : : "memory");
}

void A53_SECTION(".text.dev.loader") aarch64_DC_CVAC_range_be(void *vbegin, void *vend)
{
    unsigned char *p;

    for (p = (unsigned char *)((a53_u64)vbegin & ~(a53_u64)63); p < (unsigned char *)vend;
         p += 64) {
        dc_cvac(p);
    }
    dsb_sy();
    isb();
}

void A53_SECTION(".text.dev.loader") aarch64_DC_CVAC_range_bs(void *vbegin, a53_u64 vsize)
{
    unsigned char *p;

    for (p = (unsigned char *)((a53_u64)vbegin & ~(a53_u64)63);
         p < (unsigned char *)vbegin + vsize; p += 64) {
        dc_cvac(p);
    }
    dsb_sy();
    isb();
}

void A53_SECTION(".text.dev.loader") aarch64_DC_IVAC_range_be(void *vbegin, void *vend)
{
    unsigned char *p;

    for (p = (unsigned char *)((a53_u64)vbegin & ~(a53_u64)63); p < (unsigned char *)vend;
         p += 64) {
        dc_ivac(p);
    }
    dsb_sy();
    isb();
}

void A53_SECTION(".text.dev.loader") aarch64_DC_CIVAC_range_be(void *vbegin, void *vend)
{
    unsigned char *p;

    for (p = (unsigned char *)((a53_u64)vbegin & ~(a53_u64)63); p < (unsigned char *)vend;
         p += 64) {
        dc_civac(p);
    }
    dsb_sy();
    isb();
}
