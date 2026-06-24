#include "a53_abi.h"
#include "a53_context.h"

/* External globals */
extern mmu_page_table_t g_mmu_page_table_el3_level3;
extern mmu_page_table_t g_mmu_page_table_el3_level2;
extern mmu_page_table_t g_mmu_page_table_el3_level2_2;
extern mmu_page_table_mgr_t g_mmu_page_table_mgr_core0;
extern mmu_page_table_mgr_t g_mmu_page_table_mgr_core1;

/* Page table arrays in BSS */
extern pte_t _page_table_el3_level3[512];
extern pte_t _page_table_el3_level2[512];

/* Entry size / table size lookup tables */
static const a53_u64 g_entry_size_table[4] = {
    0x1000ULL, 0x200000ULL, 0x40000000ULL, 0x8000000000ULL
};
static const a53_u64 g_table_size_table[4] = {
    0x200000ULL, 0x40000000ULL, 0x8000000000ULL, 0xffffffffffffffffULL
};

/* ---- Helper: DC CVAC (clean data cache to point of coherency) ---- */
static void A53_SECTION(".text.el3.loader") DC_CVAC(a53_u64 va)
{
    __asm__ volatile("dc cvac, %0" : : "r"(va));
}

/* ---- mmu_va_to_pa ---- */
a53_u64 A53_SECTION(".text.el3.loader") mmu_va_to_pa(void *va)
{
    a53_u64 result;

    result = aarch64_address_translation_read(va);
    if ((result & 1) == 0) {
        result = result & 0xfffffffff000ULL;
    } else {
        printf_low("%d:%s:WARNING!!!: READ: 0x%016lx -> 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "mmu_va_to_pa", (a53_u64)va,
                   result);
        printf_low("%d:%s:FST = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "mmu_va_to_pa",
                   result & 0x7eULL);
        printf_low("%d:%s:PAR = 0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "mmu_va_to_pa",
                   result & 0xfffffffff000ULL);
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_va_to_pa", 0x57, 0, "0");
        result = 0xffffffffffffffffULL;
    }
    return result;
}

/* ---- mmu_page_table_init_table ---- */
int A53_SECTION(".text.el3.loader") mmu_page_table_init_table(mmu_page_table_t *mpt)
{
    pte_t *ppVar2;
    pte_t *ppVar1;

    ppVar2 = mpt->mpt_table;
    ppVar1 = ppVar2 + 512;
    for (; ppVar2 < ppVar1; ++ppVar2) {
        *ppVar2 = 0;
    }
    return 0;
}

/* ---- mmu_page_table_init ---- */
int A53_SECTION(".text.el3.loader")
mmu_page_table_init(mmu_page_table_t *mpt, a53_u8 el, a53_u8 level,
                     a53_u64 vaddr, pte_t *pte0)
{
    mpt->mpt_el = el;
    mpt->mpt_level = level;
    mpt->mpt_vbase = vaddr;
    mpt->mpt_table = pte0;
    mpt->mpt_table_pbase = mmu_va_to_pa(pte0);
    return 0;
}

/* ---- mmu_page_table_get_entry_shift ---- */
static a53_u64 A53_SECTION(".text.el3.loader")
mmu_page_table_get_entry_shift(mmu_page_table_t *mpt)
{
    if (mpt->mpt_level < 4) {
        return 39 - (a53_u64)mpt->mpt_level * 9;
    }
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_get_entry_shift", 0xf9, 0, "0");
    return 0xffffffffffffffffULL;
}

/* ---- mmu_page_table_get_table_vsize ---- */
static a53_u64 A53_SECTION(".text.el3.loader")
mmu_page_table_get_table_vsize(mmu_page_table_t *mpt)
{
    if (mpt->mpt_level < 4) {
        return g_table_size_table[mpt->mpt_level];
    }
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_get_table_vsize", 0xd4, 0, "0");
    return 0xffffffffffffffffULL;
}

/* ---- mmu_page_table_get_entry_size ---- */
a53_u64 A53_SECTION(".text.el3.loader")
mmu_page_table_get_entry_size(mmu_page_table_t *mpt)
{
    if (mpt->mpt_level < 4) {
        return g_entry_size_table[mpt->mpt_level];
    }
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_get_entry_size", 0xe7, 0, "0");
    return 0xffffffffffffffffULL;
}

/* ---- mmu_page_table_get_vbase ---- */
a53_u64 A53_SECTION(".text.el3.loader")
mmu_page_table_get_vbase(mmu_page_table_t *mpt, a53_u64 va)
{
    switch (mpt->mpt_level) {
    case 3: return va & 0xfffffffffffff000ULL;
    case 2: return va & 0xffffffffffe00000ULL;
    case 1: return va & 0xffffffffc0000000ULL;
    default:
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_get_vbase", 0x126, 0, "0");
        return 0xffffffffffffffffULL;
    }
}

/* ---- mmu_page_table_get_index ---- */
static a53_u64 A53_SECTION(".text.el3.loader")
mmu_page_table_get_index(mmu_page_table_t *mpt, a53_u64 va)
{
    a53_u64 index;

    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_get_index", 0x198,
               (int)(mpt->mpt_vbase <= va),
               "mpt->mpt_vbase <= base");
    index = (va - mpt->mpt_vbase) >> (a53_u32)mmu_page_table_get_entry_shift(mpt);
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_get_index", 0x19b,
               (int)((a53_u32)index < 512),
               "pte_index < VMSAv8_64_4K_MAX_PTE");
    return index;
}

/* ---- mmu_page_table_get_ppte ---- */
pte_t *A53_SECTION(".text.el3.loader")
mmu_page_table_get_ppte(mmu_page_table_t *mpt, a53_u64 va)
{
    a53_u64 index;

    index = mmu_page_table_get_index(mpt, va);
    return mpt->mpt_table + index;
}

/* ---- mmu_page_table_set_table ---- */
int A53_SECTION(".text.el3.loader")
mmu_page_table_set_table(mmu_page_table_t *mpt, mmu_page_table_t *link_mpt)
{
    pte_t *ppVar4;
    a53_u64 va;

    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_set_table", 0x1b8,
               (int)(mpt != (mmu_page_table_t *)0),
               "mpt != NULL");
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_set_table", 0x1b9,
               (int)(link_mpt != (mmu_page_table_t *)0),
               "link_mpt != NULL");
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_set_table", 0x1ba,
               (int)(mpt->mpt_el == link_mpt->mpt_el),
               "mpt->mpt_el == link_mpt->mpt_el");
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_set_table", 0x1bb,
               (int)(mpt->mpt_level + 1 == (a53_u32)link_mpt->mpt_level),
               "mpt->mpt_level + 1 == link_mpt->mpt_level");

    va = link_mpt->mpt_vbase;
    ppVar4 = mmu_page_table_get_ppte(mpt, va);
    if (ppVar4 == (pte_t *)0) {
        printf_low("%d:%s:mmu_page_table_get_ppte(va 0x%016lx) failed\n",
                   (a53_u64)mp4_get_cpu(), "mmu_page_table_set_table", va);
        return -1;
    }
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_set_table", 0x1c3,
               (int)((*ppVar4 & 3) == 0),
               "(pte & VMSAv8_64_TYPE_MASK) == VMSAv8_64_TYPE_INVALID");
    *ppVar4 = link_mpt->mpt_table_pbase | 3;
    DC_CVAC((a53_u64)ppVar4 & 0xffffffffffffffc0ULL);
    return 0;
}

/* ---- mmu_page_table_check_be ---- */
int A53_SECTION(".text.el3.loader")
mmu_page_table_check_be(mmu_page_table_t *mpt, a53_u64 begin, a53_u64 end)
{
    a53_u64 table_size;

    table_size = mmu_page_table_get_table_vsize(mpt);

    if (begin < end) {
        if (begin < mpt->mpt_vbase) {
            printf_low("%d:%s:req:  begin=0x%016lx, end=0x%016lx\n",
                       (a53_u64)mp4_get_cpu(), "mmu_page_table_check_be",
                       begin, end);
            printf_low("%d:%s:table:begin=0x%016lx, end=0x%016lx\n",
                       (a53_u64)mp4_get_cpu(), "mmu_page_table_check_be",
                       mpt->mpt_vbase, mpt->mpt_vbase + table_size);
        }
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_check_be", 0x1e6,
                   (int)(mpt->mpt_vbase <= begin),
                   "mpt->mpt_vbase <= begin");

        if (mpt->mpt_vbase + table_size <= begin) {
            printf_low("%d:%s:req:  begin=0x%016lx, end=0x%016lx\n",
                       (a53_u64)mp4_get_cpu(), "mmu_page_table_check_be",
                       begin, end);
            printf_low("%d:%s:table:begin=0x%016lx, end=0x%016lx\n",
                       (a53_u64)mp4_get_cpu(), "mmu_page_table_check_be",
                       mpt->mpt_vbase, mpt->mpt_vbase + table_size);
        }
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_check_be", 0x1ec,
                   (int)(begin < mpt->mpt_vbase + table_size),
                   "begin < mpt->mpt_vbase + table_size");

        if (mpt->mpt_vbase + table_size < end) {
            printf_low("%d:%s:req:  begin=0x%016lx, end=0x%016lx\n",
                       (a53_u64)mp4_get_cpu(), "mmu_page_table_check_be",
                       begin, end);
            printf_low("%d:%s:table:begin=0x%016lx, end=0x%016lx\n",
                       (a53_u64)mp4_get_cpu(), "mmu_page_table_check_be",
                       mpt->mpt_vbase, mpt->mpt_vbase + table_size);
        }
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_check_be", 0x1f2,
                   (int)(end <= mpt->mpt_vbase + table_size),
                   "end <= mpt->mpt_vbase + table_size");
    } else {
        printf_low("%d:%s:begin=0x%016lx, end=0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "mmu_page_table_check_be",
                   begin, end);
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_check_be", 0x1df, 0, "0");
    }
    return 0;
}

/* ---- mmu_page_table_sync_all ---- */
int A53_SECTION(".text.el3.loader") mmu_page_table_sync_all(mmu_page_table_t *mpt)
{
    switch (mpt->mpt_el) {
    case 3:
        __asm__ volatile("tlbi alle3" ::: "memory");
        break;
    case 2:
        __asm__ volatile("tlbi alle2" ::: "memory");
        break;
    case 1:
        __asm__ volatile("tlbi alle1" ::: "memory");
        break;
    default:
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_sync_all", 0xa7, 0, "0");
        break;
    }
    __asm__ volatile("dsb sy" ::: "memory");
    __asm__ volatile("isb" ::: "memory");
    return 0;
}

/* ---- mmu_page_table_sync_va ---- */
static void A53_SECTION(".text.el3.loader")
mmu_page_table_sync_va(mmu_page_table_t *mpt, a53_u64 va)
{
    switch (mpt->mpt_el) {
    case 3:
        __asm__ volatile("tlbi vae3is, %0" : : "r"(va) : "memory");
        break;
    case 2:
        __asm__ volatile("tlbi vae2is, %0" : : "r"(va) : "memory");
        break;
    case 1:
        __asm__ volatile("tlbi vae1is, %0" : : "r"(va) : "memory");
        break;
    default:
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_sync_va", 0xbd, 0, "0");
        break;
    }
}

/* ---- mmu_page_table_access_check_read ---- */
int A53_SECTION(".text.el3.loader")
mmu_page_table_access_check_read(mmu_page_table_t *mpt, a53_u64 va,
                                  a53_u64 *pa)
{
    a53_u64 result;

    switch (mpt->mpt_el) {
    case 3:
        __asm__ volatile("at s1e3r, %1; mrs %0, par_el1"
                         : "=r"(result) : "r"(va) : "memory");
        break;
    case 2:
        __asm__ volatile("at s1e2r, %1; mrs %0, par_el1"
                         : "=r"(result) : "r"(va) : "memory");
        break;
    case 1:
        __asm__ volatile("at s1e1r, %1; mrs %0, par_el1"
                         : "=r"(result) : "r"(va) : "memory");
        break;
    default:
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_access_check_read", 0x139, 0, "0");
        return 0;
    }
    if (pa != (a53_u64 *)0) {
        *pa = result;
    }
    return -((int)result & 1);
}

/* ---- mmu_page_table_access_check_write ---- */
int A53_SECTION(".text.el3.loader")
mmu_page_table_access_check_write(mmu_page_table_t *mpt, a53_u64 va,
                                   a53_u64 *pa)
{
    a53_u64 result;

    switch (mpt->mpt_el) {
    case 3:
        __asm__ volatile("at s1e3w, %1; mrs %0, par_el1"
                         : "=r"(result) : "r"(va) : "memory");
        break;
    case 2:
        __asm__ volatile("at s1e2w, %1; mrs %0, par_el1"
                         : "=r"(result) : "r"(va) : "memory");
        break;
    case 1:
        __asm__ volatile("at s1e1w, %1; mrs %0, par_el1"
                         : "=r"(result) : "r"(va) : "memory");
        break;
    default:
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_access_check_write", 0x155, 0, "0");
        return 0;
    }
    if (pa != (a53_u64 *)0) {
        *pa = result;
    }
    return -((int)result & 1);
}

/* ---- mmu_page_table_access_check_el0_read ---- */
int A53_SECTION(".text.el3.loader")
mmu_page_table_access_check_el0_read(mmu_page_table_t *mpt, a53_u64 va,
                                      a53_u64 *pa)
{
    a53_u64 result;

    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_access_check_el0_read", 0x166,
               (int)(mpt->mpt_el == 1),
               "mpt->mpt_el == MMU_EL1");
    __asm__ volatile("at s1e0r, %1; mrs %0, par_el1"
                     : "=r"(result) : "r"(va) : "memory");
    if (pa != (a53_u64 *)0) {
        *pa = result;
    }
    return -((int)result & 1);
}

/* ---- mmu_page_table_access_check_el0_write ---- */
int A53_SECTION(".text.el3.loader")
mmu_page_table_access_check_el0_write(mmu_page_table_t *mpt, a53_u64 va,
                                       a53_u64 *pa)
{
    a53_u64 result;

    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_access_check_el0_write", 0x176,
               (int)(mpt->mpt_el == 1),
               "mpt->mpt_el == MMU_EL1");
    __asm__ volatile("at s1e0w, %1; mrs %0, par_el1"
                     : "=r"(result) : "r"(va) : "memory");
    if (pa != (a53_u64 *)0) {
        *pa = result;
    }
    return -((int)result & 1);
}

/* ---- mmu_page_table_access_check_range_be ---- */
int A53_SECTION(".text.el3.loader")
mmu_page_table_access_check_range_be(mmu_page_table_t *mpt, a53_u64 vbegin,
                                      a53_u64 vend, mmu_access_check_t type)
{
    a53_u64 va;
    a53_u64 entry_size;
    int iVar5;

    va = mmu_page_table_get_vbase(mpt, vbegin);
    iVar5 = 0;
    for (;;) {
        if (vend <= va) {
            return iVar5;
        }
        switch (type) {
        case mmu_access_read_ok: {
            a53_u64 pa;
            int ret;

            ret = mmu_page_table_access_check_read(mpt, va, &pa);
            if (ret != 0) {
                printf_low("%d:%s:WARNING!!!: READ NG: 0x%016lx -> 0x%016lx\n",
                           (a53_u64)mp4_get_cpu(),
                           "mmu_page_table_access_check_range_be", va, pa);
                iVar5 = -1;
            }
            break;
        }
        case mmu_access_read_ng: {
            a53_u64 pa;

            if (mmu_page_table_access_check_read(mpt, va, &pa) == 0) {
                printf_low("%d:%s:WARNING!!!: READ OK: 0x%016lx -> 0x%016lx\n",
                           (a53_u64)mp4_get_cpu(),
                           "mmu_page_table_access_check_range_be", va, pa);
                iVar5 = -1;
            }
            break;
        }
        case mmu_access_write_ok: {
            a53_u64 pa;
            int ret;

            ret = mmu_page_table_access_check_write(mpt, va, &pa);
            if (ret != 0) {
                printf_low("%d:%s:WARNING!!!: WRITE NG: 0x%016lx -> 0x%016lx\n",
                           (a53_u64)mp4_get_cpu(),
                           "mmu_page_table_access_check_range_be", va, pa);
                iVar5 = -1;
            }
            break;
        }
        case mmu_access_write_ng: {
            a53_u64 pa;

            if (mmu_page_table_access_check_write(mpt, va, &pa) == 0) {
                printf_low("%d:%s:WARNING!!!: WRITE OK: 0x%016lx -> 0x%016lx\n",
                           (a53_u64)mp4_get_cpu(),
                           "mmu_page_table_access_check_range_be", va, pa);
                iVar5 = -1;
            }
            break;
        }
        case mmu_access_el0_read_ok: {
            a53_u64 pa;
            int ret;

            ret = mmu_page_table_access_check_el0_read(mpt, va, &pa);
            if (ret != 0) {
                printf_low("%d:%s:WARNING!!!: EL0 READ NG: 0x%016lx -> 0x%016lx\n",
                           (a53_u64)mp4_get_cpu(),
                           "mmu_page_table_access_check_range_be", va, pa);
                iVar5 = -1;
            }
            break;
        }
        case mmu_access_el0_read_ng: {
            a53_u64 pa;

            if (mmu_page_table_access_check_el0_read(mpt, va, &pa) == 0) {
                printf_low("%d:%s:WARNING!!!: EL0 READ OK: 0x%016lx -> 0x%016lx\n",
                           (a53_u64)mp4_get_cpu(),
                           "mmu_page_table_access_check_range_be", va, pa);
                iVar5 = -1;
            }
            break;
        }
        case mmu_access_el0_write_ok: {
            a53_u64 pa;
            int ret;

            ret = mmu_page_table_access_check_el0_write(mpt, va, &pa);
            if (ret != 0) {
                printf_low("%d:%s:WARNING!!!: EL0 WRITE NG: 0x%016lx -> 0x%016lx\n",
                           (a53_u64)mp4_get_cpu(),
                           "mmu_page_table_access_check_range_be", va, pa);
                iVar5 = -1;
            }
            break;
        }
        case mmu_access_el0_write_ng: {
            a53_u64 pa;

            if (mmu_page_table_access_check_el0_write(mpt, va, &pa) == 0) {
                printf_low("%d:%s:WARNING!!!: EL0 WRITE OK: 0x%016lx -> 0x%016lx\n",
                           (a53_u64)mp4_get_cpu(),
                           "mmu_page_table_access_check_range_be", va, pa);
                iVar5 = -1;
            }
            break;
        }
        default:
            break;
        }
        entry_size = mmu_page_table_get_entry_size(mpt);
        va = entry_size + va;
    }
}

/* ---- mmu_page_table_set_map ---- */
static void A53_SECTION(".text.el3.loader")
mmu_page_table_set_map(mmu_page_table_t *mpt, pte_t *ppte,
                        mmu_op_t op, a53_u64 pbase,
                        mmu_map_mode_t mode, mmu_mem_type_t mem)
{
    pte_t pVar11;

    switch ((int)mem) {
    case 0: pVar11 = 0x40000000000780ULL; break;
    case 1: pVar11 = 0x40000000000700ULL; break;
    case 2: pVar11 = 0x780ULL; break;
    case 3:
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_set_map", 0x223,
                   (int)(mpt->mpt_el == 1),
                   "mpt->mpt_el == MMU_EL1");
        pVar11 = 0x400000000007c0ULL;
        break;
    case 4:
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_set_map", 0x22a,
                   (int)(mpt->mpt_el == 1),
                   "mpt->mpt_el == MMU_EL1");
        pVar11 = 0x40000000000740ULL;
        break;
    case 5:
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_set_map", 0x230,
                   (int)(mpt->mpt_el == 1),
                   "mpt->mpt_el == MMU_EL1");
        pVar11 = 0x7c0ULL;
        break;
    default:
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_set_map", 0x236, 0, "0");
        pVar11 = 0x300ULL;
        break;
    }

    pVar11 = pbase | pVar11;

    if (mode == map_mode_ro) {
        pVar11 = (pVar11 & 0xffffffffffffffe3ULL) | 0x10ULL;
    } else if (mode == map_mode_rw) {
        pVar11 = pVar11 & 0xffffffffffffffe3ULL;
    } else {
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_set_map", 0x242, 0, "0");
    }

    if (mpt->mpt_level <= 2) {
        pVar11 |= 1;
    } else if (mpt->mpt_level == 3) {
        pVar11 |= 3;
    } else {
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_page_table_set_map", 0x24f, 0, "0");
    }

    *ppte = pVar11;
}

/* ---- mmu_page_table_op_range_be ---- */
int A53_SECTION(".text.el3.loader")
mmu_page_table_op_range_be(mmu_page_table_t *mpt, mmu_op_t op,
                             a53_u64 vbegin, a53_u64 pbegin, a53_u64 vend,
                             mmu_map_mode_t mode, mmu_mem_type_t mem)
{
    a53_u64 va;
    pte_t *ppVar5;
    pte_t *ppVar9;
    a53_u64 entry_size;
    a53_u64 pbase;

    mmu_page_table_check_be(mpt, vbegin, vend);
    va = mmu_page_table_get_vbase(mpt, vbegin);
    ppVar5 = mmu_page_table_get_ppte(mpt, va);
    pbase = pbegin;
    ppVar9 = ppVar5;

    for (;;) {
        mmu_page_table_set_map(mpt, ppVar9, op, pbase, mode, mem);

        entry_size = mmu_page_table_get_entry_size(mpt);
        va += entry_size;

        if (vend <= va) {
            pte_t *ppVarS;
            pte_t *ppVarE;

            ppVarS = (pte_t *)((a53_u64)ppVar5 & 0xffffffffffffffc0ULL);
            ppVarE = ppVar5;
            for (; ppVarS < ppVarE; ppVarS += 8) {
                DC_CVAC((a53_u64)ppVarS);
            }

            mmu_page_table_sync_all(mpt);

            /* Access check based on mem type */
            switch ((int)mem) {
            case 0:
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x2ed,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_read_ok) == 0),
                           "mmu_page_table_access_check_range_be(mpt, vbegin, vend, mmu_access_read_ok) == 0");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x2ee,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_write_ng) == 0),
                           "mmu_page_table_access_check_range_be(mpt, vbegin, vend, mmu_access_write_ng) == 0");
                break;
            case 1:
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x2f1,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_read_ok) == 0),
                           "mmu_page_table_access_check_range_be(mpt, vbegin, vend, mmu_access_read_ok) == 0");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x2f2,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_write_ok) == 0),
                           "mmu_page_table_access_check_range_be(mpt, vbegin, vend, mmu_access_write_ok) == 0");
                break;
            case 2:
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x2f5,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_read_ok) == 0),
                           "mmu_page_table_access_check_range_be(mpt, vbegin, vend, mmu_access_read_ok) == 0");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x2f6,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_write_ng) == 0),
                           "mmu_page_table_access_check_range_be(mpt, vbegin, vend, mmu_access_write_ng) == 0");
                break;
            case 3:
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x2f9,
                           (int)(mpt->mpt_el == 1), "mpt->mpt_el == MMU_EL1");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x2fa,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_read_ok) == 0),
                           "read_ok");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x2fb,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_write_ng) == 0),
                           "write_ng");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x2fc,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_el0_read_ok) == 0),
                           "el0_read_ok");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x2fd,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_el0_write_ng) == 0),
                           "el0_write_ng");
                break;
            case 4:
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x300,
                           (int)(mpt->mpt_el == 1), "mpt->mpt_el == MMU_EL1");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x301,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_read_ok) == 0),
                           "read_ok");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x302,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_write_ok) == 0),
                           "write_ok");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x303,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_el0_read_ok) == 0),
                           "el0_read_ok");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x304,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_el0_write_ok) == 0),
                           "el0_write_ok");
                break;
            case 5:
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x307,
                           (int)(mpt->mpt_el == 1), "mpt->mpt_el == MMU_EL1");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x308,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_read_ok) == 0),
                           "read_ok");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x309,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_write_ng) == 0),
                           "write_ng");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x30a,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_el0_read_ok) == 0),
                           "el0_read_ok");
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                           "mmu_page_table_op_range_be", 0x30b,
                           (int)(mmu_page_table_access_check_range_be(
                               mpt, vbegin, vend, mmu_access_el0_write_ng) == 0),
                           "el0_write_ng");
                break;
            default:
                return 0;
            }
            return 0;
        }

        ++ppVar9;
        entry_size = mmu_page_table_get_entry_size(mpt);
        vbegin += entry_size;
        pbase += entry_size;
    }
}

/* ---- mmu_page_table_map_range_be ---- */
int A53_SECTION(".text.el3.loader")
mmu_page_table_map_range_be(mmu_page_table_t *mpt, a53_u64 vbegin,
                              a53_u64 pbegin, a53_u64 vend,
                              mmu_map_mode_t mode, mmu_mem_type_t mem)
{
    mmu_page_table_op_range_be(mpt, mmu_op_map, vbegin, pbegin, vend, mode, mem);
    return 0;
}

/* ---- mmu_page_table_map_range_bs ---- */
int A53_SECTION(".text.el3.loader")
mmu_page_table_map_range_bs(mmu_page_table_t *mpt, a53_u64 vbegin,
                              a53_u64 pbegin, a53_u64 vsize,
                              mmu_map_mode_t mode, mmu_mem_type_t mem)
{
    mmu_page_table_op_range_be(mpt, mmu_op_map, vbegin, pbegin,
                                vbegin + vsize, mode, mem);
    return 0;
}

/* ---- mmu_page_table_show_map ---- */
void A53_SECTION(".text.el3.loader")
mmu_page_table_show_map(a53_u32 el, a53_u64 map_va, a53_u64 map_size,
                          a53_u64 map_pa0, a53_u64 map_pte, a53_u64 pte_vsize)
{
    a53_u64 uVar8;
    a53_u64 uVar5;

    printf_low("%d:%s:MAP: 0x%016lx+0x%016lx: 0x%016lx: 0x%016lx: ",
               (a53_u64)mp4_get_cpu(), "mmu_page_table_show_map",
               map_va, map_size, map_pa0, map_pte);

    if ((map_pte >> 52 & 1) == 0) {
        uVar8 = map_pte & 0xfffffffffffffffcULL;
        uVar5 = pte_vsize;
    } else {
        uVar8 = map_pte & 0xffeffffffffffffcULL;
        uVar5 = 0x10000ULL;
        if (pte_vsize != 0x1000ULL) {
            uVar5 = pte_vsize;
        }
    }

    if (uVar5 == 0x1000000000000ULL) {
        printf_low("256T: ");
    } else if (uVar5 == 0x10000ULL) {
        printf_low(" 64K: ");
    } else if (uVar5 == 0x200000ULL) {
        printf_low("  2M: ");
    } else if (uVar5 == 0x40000000ULL) {
        printf_low("  1G: ");
    } else if (uVar5 == 0x8000000000ULL) {
        printf_low("512G: ");
    } else if (uVar5 == 0x1000ULL) {
        printf_low("  4K: ");
    } else {
        printf_low("0x%016lx: ", uVar5);
    }
    printf_low("\n");
}

/* ---- mmu_page_table_get_map ---- */
int A53_SECTION(".text.el3.loader") mmu_page_table_get_map(mmu_page_table_t *mpt)
{
    a53_u64 pte_vsize;
    a53_u64 uVar9;
    int bVar5;
    a53_u64 map_va;
    a53_u64 map_pa0;
    a53_u64 map_size;
    a53_u64 map_pte;
    a53_u64 uVar11;
    a53_s64 lVar13;
    pte_t *ppVar2;
    a53_u8 uVar3;

    uVar3 = mpt->mpt_level;
    uVar11 = mpt->mpt_vbase;
    ppVar2 = mpt->mpt_table;

    if (uVar3 == 0) {
        uVar9 = 0xff8000000000ULL;
        pte_vsize = 0x8000000000ULL;
        bVar5 = 1;
    } else if (uVar3 == 2) {
        uVar9 = 0xffffffe00000ULL;
        pte_vsize = 0x200000ULL;
        bVar5 = 1;
    } else if (uVar3 == 1) {
        uVar9 = 0xffffc0000000ULL;
        pte_vsize = 0x40000000ULL;
        bVar5 = 1;
    } else {
        uVar9 = 0xfffffffff000ULL;
        pte_vsize = 0x1000ULL;
        bVar5 = 0;
    }

    lVar13 = 0;
    map_va = 0;
    map_pa0 = 0;
    map_size = 0;
    map_pte = 0;

    do {
        a53_u64 uVar10;
        a53_u32 uVar1;

        if (lVar13 == 0x1000) {
            if (map_pte != 0) {
                mmu_page_table_show_map(
                    mpt->mpt_el, map_va, map_size, map_pa0,
                    map_pte, pte_vsize);
            }
            return 0;
        }

        uVar10 = *(a53_u64 *)((a53_u64)ppVar2 + lVar13);
        uVar1 = (a53_u32)uVar10 & 3;

        if (uVar1 == 0) {
            if (map_pte != 0) {
                mmu_page_table_show_map(
                    mpt->mpt_el, map_va, map_size, map_pa0,
                    map_pte, pte_vsize);
                map_pte = 0;
                map_va = uVar11;
                map_size = 0;
            }
        } else if (uVar1 == 3) {
            if (bVar5) {
                if (map_pte != 0) {
                    mmu_page_table_show_map(
                        mpt->mpt_el, map_va, map_size, map_pa0,
                        map_pte, pte_vsize);
                    map_pte = 0;
                    map_va = uVar11;
                    map_size = 0;
                }
            }
        } else if (uVar1 == 1) {
            a53_u64 uVar12;
            a53_u64 uVar8;

            uVar12 = uVar10 & uVar9;
            uVar8 = uVar10 & ~uVar9;

            if (map_pte != 0) {
                if (map_pte != uVar8 || map_pa0 + map_size != uVar12) {
                    mmu_page_table_show_map(
                        mpt->mpt_el, map_va, map_size, map_pa0,
                        map_pte, pte_vsize);
                    map_pte = 0;
                    map_va = uVar11;
                    map_size = 0;
                }
            }
            map_pa0 = uVar12;
            map_pte = uVar8;
            if (map_size == 0) {
                map_va = uVar11;
            }
            map_size += pte_vsize;
        } else {
            printf_low("%d:%s:0x%016lx: OTHERS\n",
                       (a53_u64)mp4_get_cpu(),
                       "mmu_page_table_get_map", uVar11);
            if (map_pte != 0) {
                mmu_page_table_show_map(
                    mpt->mpt_el, map_va, map_size, map_pa0,
                    map_pte, pte_vsize);
                map_pte = 0;
                map_va = uVar11;
                map_size = 0;
            }
        }

        uVar11 += pte_vsize;
        lVar13 += 8;
    } while (1);
}

/* ---- mmu_page_table_mgr_alloc ---- */
mmu_page_table_t *A53_SECTION(".text.el3.loader")
mmu_page_table_mgr_alloc(a53_u8 el, a53_u8 level, a53_u64 va)
{
    mmu_page_table_mgr_t *pmgr;
    a53_u32 max_tables;

    if (mp4_get_cpu() != 0) {
        pmgr = &g_mmu_page_table_mgr_core1;
    } else {
        pmgr = &g_mmu_page_table_mgr_core0;
    }

    max_tables = pmgr->mptm_table_max;
    if (max_tables == 0) {
        max_tables = (a53_u32)((a53_u64)pmgr->mptm_ptr_end
                               - (a53_u64)pmgr->mptm_ptr_begin) >> 12;
        pmgr->mptm_table_max = max_tables;
    }

    if (pmgr->mptm_table_count < max_tables
        && pmgr->mptm_mpt_count < pmgr->mptm_mpt_max) {
        mmu_page_table_t *mpt;

        mpt = &pmgr->mptm_mpt[pmgr->mptm_mpt_count];
        ++pmgr->mptm_mpt_count;
        mmu_page_table_init(mpt, 0xff, 0xff, 0, pmgr->mptm_ptr_cur);
        pmgr->mptm_ptr_cur += 512;
        ++pmgr->mptm_table_count;

        if (mpt != (mmu_page_table_t *)0) {
            pte_t *ppVar7;
            pte_t *ppVar1;

            ppVar7 = mpt->mpt_table;
            mpt->mpt_el = el;
            mpt->mpt_level = level;
            mpt->mpt_vbase = va & 0xffffffffffe00000ULL;

            ppVar1 = ppVar7 + 512;
            for (; ppVar7 < ppVar1; ++ppVar7) {
                *ppVar7 = 0;
            }
            return mpt;
        }
    }

    printf_low("%d:%s:.mptm_table_count=0x%02x, .mptm_table_max=0x%02x\n",
               (a53_u64)mp4_get_cpu(), "mmu_page_table_mgr_alloc_core",
               (a53_u64)pmgr->mptm_table_count,
               (a53_u64)pmgr->mptm_table_max);
    printf_low("%d:%s:.mptm_mpt_count=0x%02x, .mptm_mpt_max=0x%02x\n",
               (a53_u64)mp4_get_cpu(), "mmu_page_table_mgr_alloc_core",
               (a53_u64)pmgr->mptm_mpt_count,
               (a53_u64)pmgr->mptm_mpt_max);
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_mgr_alloc_core", 0x603, 0, "0");
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_mgr_alloc", 0x61c, 0, "0");
    return (mmu_page_table_t *)0;
}

/* ---- mmu_init functions ---- */
void A53_SECTION(".text.el3.loader") mmu_init_phase1(void)
{
    a53_u64 tcr;

    __asm__("mrs %0, tcr_el3" : "=r"(tcr));
    if ((tcr & 0xc000ULL) != 0) {
        printf_low("%d:%s:TCR 0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "mmu_init_phase1",
                   (a53_u32)tcr);
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
                   "mmu_init_phase1", 0x59b, 0, "0");
    }

    mmu_page_table_init(&g_mmu_page_table_el3_level3,
                         3, 3, 0, _page_table_el3_level3);

    printf_low("%d:%s:UNMAP 0x00000000 - 0x00001000\n",
               (a53_u64)mp4_get_cpu(), "mmu_init_phase1");

    /* DC CIVAC range for unmapping */
    {
        pte_t *ppVar8;

        ppVar8 = mmu_page_table_get_ppte(&g_mmu_page_table_el3_level3, 0);
        *ppVar8 &= ~3ULL;
        mmu_page_table_sync_va(&g_mmu_page_table_el3_level3, 0);
    }

    __asm__ volatile("tlbi alle3" ::: "memory");
    __asm__ volatile("dsb sy" ::: "memory");
    __asm__ volatile("isb" ::: "memory");

    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_init_phase1", 0x5a7,
               (int)(mmu_page_table_access_check_read(
                   &g_mmu_page_table_el3_level3, 0, (a53_u64 *)0) < 0),
               "mmu_page_table_access_check_read(mpt, 0x0, NULL) < 0");

    mmu_page_table_init(&g_mmu_page_table_el3_level2,
                         3, 2, 0, _page_table_el3_level2);

    /* mmu_page_table_el3_level2_2 at 0x40000000 */
    {
        extern pte_t _DAT_00102000[512];

        mmu_page_table_init(&g_mmu_page_table_el3_level2_2,
                             3, 2, 0x40000000ULL, _DAT_00102000);
    }
}

void A53_SECTION(".text.el3.loader") mmu_init_phase2a(void)
{
    return;
}

void A53_SECTION(".text.el3.loader") mmu_init_phase2b(void)
{
    mmu_el3_level2_0_op_range_be(0x6000000ULL, mmu_op_map,
                                  0x88200000ULL, 0x6200000ULL,
                                  map_mode_rw_rw, mem_type_memory);
}

void A53_SECTION(".text.el3.loader") mmu_init_phase3(void)
{
    printf_low("%d:%s:()\n", (a53_u64)mp4_get_cpu(), "mmu_init_phase3");
    mmu_el3_level2_0_op_range_be(0x40200000ULL, mmu_op_map,
                                  0x40400000ULL, 0x40600000ULL,
                                  map_mode_rw_rw, mem_type_memory);
    mmu_el3_level2_0_unmap_range_be(0x40600000ULL, 0x44000000ULL);
    mmu_el3_level2_0_unmap_range_be(0x50000000ULL, 0x80000000ULL);
    printf_low("%d:%s:<-\n", (a53_u64)mp4_get_cpu(), "mmu_init_phase3");
}

void A53_SECTION(".text.el3.loader") mmu_init_phase4(void)
{
    a53_u64 sctlr;

    __asm__("mrs %0, sctlr_el3" : "=r"(sctlr));
    sctlr |= 0x80000ULL;
    __asm__("msr sctlr_el3, %0" : : "r"(sctlr));
}

/* ---- mmu_el3_level2_0_unmap_range_be ---- */
int A53_SECTION(".text.el3.loader")
mmu_el3_level2_0_unmap_range_be(a53_u64 vbegin, a53_u64 vend)
{
    mmu_page_table_t *mpt;
    a53_u64 va;
    pte_t *ppVar2;

    if (vbegin >> 30 != 0) {
        mpt = &g_mmu_page_table_el3_level2_2;
    } else {
        mpt = &g_mmu_page_table_el3_level2;
    }

    mmu_page_table_check_be(mpt, vbegin, vend);
    va = mmu_page_table_get_vbase(mpt, vbegin);
    ppVar2 = mmu_page_table_get_ppte(mpt, va);

    do {
        *ppVar2 &= ~3ULL;
        va += mmu_page_table_get_entry_size(mpt);
        ++ppVar2;
    } while (va < vend);

    mmu_page_table_sync_all(mpt);
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_unmap_range_be", 0x343,
               (int)(mmu_page_table_access_check_range_be(
                   mpt, vbegin, vend, mmu_access_read_ng) == 0),
               "mmu_page_table_access_check_range_be(mpt, vbegin, vend, mmu_access_read_ng) == 0");
    el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\mmu.c",
               "mmu_page_table_unmap_range_be", 0x344,
               (int)(mmu_page_table_access_check_range_be(
                   mpt, vbegin, vend, mmu_access_write_ng) == 0),
               "mmu_page_table_access_check_range_be(mpt, vbegin, vend, mmu_access_write_ng) == 0");
    return 0;
}

/* ---- mmu_el3_level2_0_op_range_be ---- */
int A53_SECTION(".text.el3.loader")
mmu_el3_level2_0_op_range_be(a53_u64 vbegin, mmu_op_t op,
                               a53_u64 pbegin, a53_u64 vend,
                               mmu_map_mode_t mode, mmu_mem_type_t mem)
{
    mmu_page_table_t *mpt;

    if (vbegin >> 30 != 0) {
        mpt = &g_mmu_page_table_el3_level2_2;
    } else {
        mpt = &g_mmu_page_table_el3_level2;
    }

    return mmu_page_table_op_range_be(mpt, mmu_op_map, vbegin, pbegin,
                                       vend & 0xffffffffULL, mode, mem);
}
