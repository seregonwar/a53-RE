#include "a53_abi.h"
#include "a53_context.h"
#include "../include/poc_bridge.h"

/* Forward declarations from existing modules */
extern main_mp4_param_t *msi_get_main_param(void);
extern void syshub_tlb_get(a53_u32 tlb, a53_u32 *tlb0, a53_u32 *tlb1,
                            a53_u32 *tlb2, a53_u32 *tlb3,
                            a53_u32 *sub, a53_u32 *attr1);
extern a53_u64 aarch64_address_translation_read(void *va);
extern a53_u64 aarch64_read_DAIF(void);
extern void writel(a53_u64 addr, a53_u32 val);
extern mmu_page_table_t g_mmu_page_table_el3_level3;
extern pte_t *mmu_page_table_get_ppte(mmu_page_table_t *, a53_u64);

/* ---- Safe memory access validation ---- */
static int A53_SECTION(".text.el3.loader")
poc_is_safe_physical_range(a53_u64 pa, a53_u64 size)
{
    a53_u64 end = pa + size;

    /* Allow: SRAM (0x88000000-0x88FFFFFF) */
    if (pa >= 0x88000000ULL && end <= 0x89000000ULL) return 1;
    /* Allow: Debug status region (0xEC000000-0xEC1FFFFF) */
    if (pa >= 0xEC000000ULL && end <= 0xEC200000ULL) return 1;
    /* Allow: MSI / IOMMU / Syshub / GIC MMIO (0x30000000-0x33000000) */
    if (pa >= 0x30000000ULL && end <= 0x33000000ULL) return 1;
    /* Allow: DRAM (0x40000000-0x9FFFFFFF for development) */
    if (pa >= 0x40000000ULL && end <= 0xA0000000ULL) return 1;

    printf_low("%d:%s:BLOCKED: pa=0x%016lx size=0x%lx not in safe range\n",
               (a53_u64)mp4_get_cpu(), "poc_is_safe_physical_range",
               pa, size);
    return 0;
}

/* ---- System register read via inline asm ---- */
static a53_u64 A53_SECTION(".text.el3.loader")
poc_read_sysreg(a53_u32 reg)
{
    a53_u64 v = 0;
    switch (reg) {
    case POC_REG_SCTLR_EL1:   __asm__("mrs %0, sctlr_el1"   : "=r"(v)); break;
    case POC_REG_SCTLR_EL2:   __asm__("mrs %0, sctlr_el2"   : "=r"(v)); break;
    case POC_REG_SCTLR_EL3:   __asm__("mrs %0, sctlr_el3"   : "=r"(v)); break;
    case POC_REG_SCR_EL3:     __asm__("mrs %0, scr_el3"     : "=r"(v)); break;
    case POC_REG_HCR_EL2:     __asm__("mrs %0, hcr_el2"     : "=r"(v)); break;
    case POC_REG_TCR_EL1:     __asm__("mrs %0, tcr_el1"     : "=r"(v)); break;
    case POC_REG_TTBR0_EL1:   __asm__("mrs %0, ttbr0_el1"   : "=r"(v)); break;
    case POC_REG_TTBR0_EL3:   __asm__("mrs %0, ttbr0_el3"   : "=r"(v)); break;
    case POC_REG_VBAR_EL1:    __asm__("mrs %0, vbar_el1"    : "=r"(v)); break;
    case POC_REG_VBAR_EL2:    __asm__("mrs %0, vbar_el2"    : "=r"(v)); break;
    case POC_REG_VBAR_EL3:    __asm__("mrs %0, vbar_el3"    : "=r"(v)); break;
    case POC_REG_MAIR_EL1:    __asm__("mrs %0, mair_el1"    : "=r"(v)); break;
    case POC_REG_MAIR_EL3:    __asm__("mrs %0, mair_el3"    : "=r"(v)); break;
    case POC_REG_SPSR_EL3:    __asm__("mrs %0, spsr_el3"    : "=r"(v)); break;
    case POC_REG_ELR_EL3:     __asm__("mrs %0, elr_el3"     : "=r"(v)); break;
    case POC_REG_DAIF:        v = aarch64_read_DAIF();                 break;
    case POC_REG_PMCR_EL0:    __asm__("mrs %0, pmcr_el0"    : "=r"(v)); break;
    case POC_REG_MDCR_EL2:    __asm__("mrs %0, mdcr_el2"    : "=r"(v)); break;
    case POC_REG_MDCR_EL3:    __asm__("mrs %0, mdcr_el3"    : "=r"(v)); break;
    case POC_REG_CPACR_EL1:   __asm__("mrs %0, cpacr_el1"   : "=r"(v)); break;
    case POC_REG_CPUECTLR_EL1:__asm__("mrs %0, s3_1_c15_c2_1" : "=r"(v)); break;
    default:
        printf_low("%d:%s:unknown sysreg 0x%04x\n",
                   (a53_u64)mp4_get_cpu(), "poc_read_sysreg", reg);
        return 0xFFFFFFFFFFFFFFFFULL;
    }
    return v;
}

/* ---- System register write via inline asm (safe subset only) ---- */
static void A53_SECTION(".text.el3.loader")
poc_write_sysreg(a53_u32 reg, a53_u64 val)
{
    switch (reg) {
    case POC_REG_SCTLR_EL1:   __asm__("msr sctlr_el1, %0"   : : "r"(val)); break;
    case POC_REG_SCTLR_EL2:   __asm__("msr sctlr_el2, %0"   : : "r"(val)); break;
    case POC_REG_SCTLR_EL3:   __asm__("msr sctlr_el3, %0"   : : "r"(val)); break;
    case POC_REG_SCR_EL3:     __asm__("msr scr_el3, %0"     : : "r"(val)); break;
    case POC_REG_HCR_EL2:     __asm__("msr hcr_el2, %0"     : : "r"(val)); break;
    case POC_REG_TCR_EL1:     __asm__("msr tcr_el1, %0"     : : "r"(val)); break;
    case POC_REG_MAIR_EL1:    __asm__("msr mair_el1, %0"    : : "r"(val)); break;
    case POC_REG_MAIR_EL3:    __asm__("msr mair_el3, %0"    : : "r"(val)); break;
    case POC_REG_PMCR_EL0:    __asm__("msr pmcr_el0, %0"    : : "r"(val)); break;
    case POC_REG_MDCR_EL2:    __asm__("msr mdcr_el2, %0"    : : "r"(val)); break;
    case POC_REG_MDCR_EL3:    __asm__("msr mdcr_el3, %0"    : : "r"(val)); break;
    case POC_REG_CPACR_EL1:   __asm__("msr cpacr_el1, %0"   : : "r"(val)); break;
    /* Blocked: VBAR, TTBR, SPSR, ELR writes */
    case POC_REG_VBAR_EL1:
    case POC_REG_VBAR_EL2:
    case POC_REG_VBAR_EL3:
    case POC_REG_TTBR0_EL1:
    case POC_REG_TTBR0_EL3:
    case POC_REG_SPSR_EL3:
    case POC_REG_ELR_EL3:
        printf_low("%d:%s:BLOCKED write to safety-critical reg 0x%04x\n",
                   (a53_u64)mp4_get_cpu(), "poc_write_sysreg", reg);
        break;
    default:
        printf_low("%d:%s:unknown sysreg 0x%04x\n",
                   (a53_u64)mp4_get_cpu(), "poc_write_sysreg", reg);
        break;
    }
}

/* ---- Extended SVC dispatcher ---- */
int A53_SECTION(".text.el3.loader")
poc_bridge_dispatch(mp4_debug_status_t *status)
{
    a53_u32 cmd;
    int result;

    cmd = (a53_u32)status->mds_gpr[0];
    result = 0;
    status->mds_gpr[0] = 0;

    switch (cmd) {

    case POC_CMD_PEEK_MEM: {
        a53_u64 pa  = status->mds_gpr[1];
        a53_u64 val = 0xFFFFFFFFFFFFFFFFULL;

        if (poc_is_safe_physical_range(pa, 8)) {
            if ((pa & 7) == 0) {
                val = *(volatile a53_u64 *)pa;
            } else if ((pa & 3) == 0) {
                val = (a53_u64)*(volatile a53_u32 *)pa;
            } else {
                val = (a53_u64)*(volatile a53_u8 *)pa;
            }
        }
        status->mds_gpr[0] = val;
        break;
    }

    case POC_CMD_POKE_MEM: {
        a53_u64 pa  = status->mds_gpr[1];
        a53_u64 val = status->mds_gpr[2];

        if (poc_is_safe_physical_range(pa, 8)) {
            if ((pa & 7) == 0) {
                *(volatile a53_u64 *)pa = val;
                __asm__ volatile("dc cvac, %0" : : "r"(pa & ~0x3fULL));
            } else if ((pa & 3) == 0) {
                *(volatile a53_u32 *)pa = (a53_u32)val;
                __asm__ volatile("dc cvac, %0" : : "r"(pa & ~0x3fULL));
            } else {
                *(volatile a53_u8 *)pa = (a53_u8)val;
                __asm__ volatile("dc cvac, %0" : : "r"(pa & ~0x3fULL));
            }
            status->mds_gpr[0] = 0;
        } else {
            status->mds_gpr[0] = 1;
        }
        break;
    }

    case POC_CMD_READ_SYSREG: {
        a53_u32 reg = (a53_u32)status->mds_gpr[1];
        status->mds_gpr[0] = poc_read_sysreg(reg);
        break;
    }

    case POC_CMD_WRITE_SYSREG: {
        a53_u32 reg = (a53_u32)status->mds_gpr[1];
        a53_u64 val = status->mds_gpr[2];
        poc_write_sysreg(reg, val);
        status->mds_gpr[0] = 0;
        break;
    }

    case POC_CMD_GET_TLB: {
        a53_u32 tlb = (a53_u32)status->mds_gpr[1];
        a53_u32 tlb0, tlb1, tlb2, tlb3, sub, attr1;

        syshub_tlb_get(tlb, &tlb0, &tlb1, &tlb2, &tlb3, &sub, &attr1);
        status->mds_gpr[1] = (a53_u64)tlb0;
        status->mds_gpr[2] = (a53_u64)tlb1;
        status->mds_gpr[3] = (a53_u64)tlb2;
        status->mds_gpr[4] = (a53_u64)tlb3;
        status->mds_gpr[5] = (a53_u64)sub;
        status->mds_gpr[6] = (a53_u64)attr1;
        status->mds_gpr[0] = 0;
        break;
    }

    case POC_CMD_SET_TLB: {
        a53_u32 tlb  = (a53_u32)status->mds_gpr[1];
        a53_u64 pa   = status->mds_gpr[2];
        a53_u32 seg  = (a53_u32)status->mds_gpr[3];
        a53_u32 attr = (a53_u32)status->mds_gpr[4];

        if (poc_is_safe_physical_range(pa, (a53_u64)seg ? seg : 0x1000)) {
            a53_u32 idx, tlb0, tlb1;
            volatile a53_u32 *p;
            a53_u32 uVar1;

            switch (seg) {
            case 0x20000:    tlb0 = 0;   break;
            case 0x4000000:  tlb0 = 0x12; break;
            case 0x80000:    tlb0 = 4;   break;
            case 0x100000:   tlb0 = 6;   break;
            case 0x200000:   tlb0 = 8;   break;
            case 0x400000:   tlb0 = 10;  break;
            case 0x800000:   tlb0 = 0xc; break;
            case 0x1000000:  tlb0 = 0xe; break;
            case 0x2000000:  tlb0 = 0x10; break;
            case 0x40000:    tlb0 = 2;   break;
            default:         tlb0 = 0x12; break;
            }

            uVar1 = (a53_u32)pa & 0xfc000000U;
            tlb1 = (a53_u32)(pa - uVar1);
            if (tlb1 != 0) {
                tlb0 |= (tlb1 >> 12) & 0xfffe0;
            }

            idx = tlb - 1;
            p = (volatile a53_u32 *)(a53_u64)(idx * 0x10 + 0x3230000);
            p[0] = (a53_u32)(pa >> 26);
            p[1] = tlb0;
            p[2] = attr;
            p[3] = attr;
            *(volatile a53_u32 *)(a53_u64)(idx * 4 + 0x32303e0) = 0xffffffff;
            if (tlb != 0x3e) {
                *(volatile a53_u32 *)(a53_u64)(idx * 4 + 0x32304d8) = 0xc1800003;
            }
            __asm__ volatile("dsb sy" ::: "memory");
            status->mds_gpr[0] = 0;
        } else {
            status->mds_gpr[0] = 1;
        }
        break;
    }

    case POC_CMD_VA_TO_PA: {
        void *va = (void *)status->mds_gpr[1];
        a53_u64 pa;

        pa = aarch64_address_translation_read(va);
        if ((pa & 1) == 0) {
            status->mds_gpr[0] = pa & 0xfffffffff000ULL;
        } else {
            status->mds_gpr[0] = 0xFFFFFFFFFFFFFFFFULL;
        }
        break;
    }

    case POC_CMD_EL0_VA_TO_PA: {
        a53_u64 el0_va = status->mds_gpr[1];
        a53_u64 par;

        __asm__ volatile("at s1e0r, %1; mrs %0, par_el1"
                         : "=r"(par) : "r"(el0_va) : "memory");
        if ((par & 1) == 0) {
            status->mds_gpr[0] = (par & 0xfffffffff000ULL) |
                                  (el0_va & 0xfffULL);
        } else {
            status->mds_gpr[0] = 0xFFFFFFFFFFFFFFFFULL;
        }
        break;
    }

    case POC_CMD_GET_MAIN_PARAM: {
        main_mp4_param_t *pm;

        pm = msi_get_main_param();
        status->mds_gpr[0] = (a53_u64)pm;
        status->mds_gpr[1] = (a53_u64)(pm ? pm->mm4p_self_size : 0);
        break;
    }

    case POC_CMD_GET_DEBUG_STAT: {
        dev_context_t *dc;
        mp4_debug_status_t *ds;

        dc = get_dev_context();
        ds = dc->dc_debug_status;
        status->mds_gpr[0] = (a53_u64)ds;
        status->mds_gpr[1] = (a53_u64)(ds ? ds->mds_self_size : 0);
        break;
    }

    case POC_CMD_GET_PAGE_TABLE: {
        a53_u64 vbar;

        __asm__("mrs %0, vbar_el3" : "=r"(vbar));
        status->mds_gpr[0] = vbar;
        __asm__("mrs %0, ttbr0_el3" : "=r"(vbar));
        status->mds_gpr[1] = vbar;
        __asm__("mrs %0, ttbr0_el1" : "=r"(vbar));
        status->mds_gpr[2] = vbar;
        break;
    }

    case POC_CMD_MAP_PAGE: {
        a53_u64 va   = status->mds_gpr[1];
        a53_u64 pa   = status->mds_gpr[2];
        a53_u32 mode = (a53_u32)status->mds_gpr[3];

        if (poc_is_safe_physical_range(pa, 0x1000)) {
            pte_t *ppte;
            pte_t pte_val;
            a53_u64 pbase = pa & 0xfffffffff000ULL;

            ppte = mmu_page_table_get_ppte(
                &g_mmu_page_table_el3_level3, va);
            if (ppte) {
                pte_val = pbase | 0x40000000000705ULL;
                if (mode == 0) {
                    pte_val = (pte_val & ~0x1cULL) | 0x10ULL;
                } else {
                    pte_val = pte_val & ~0x1cULL;
                }
                *ppte = pte_val;
                __asm__ volatile("dc cvac, %0" : :
                    "r"((a53_u64)ppte & ~0x3fULL));
                __asm__ volatile("tlbi vae3is, %0" : : "r"(va) : "memory");
                __asm__ volatile("dsb sy" ::: "memory");
                __asm__ volatile("isb" ::: "memory");
                status->mds_gpr[0] = pbase;
            } else {
                status->mds_gpr[0] = 0xFFFFFFFFFFFFFFFFULL;
            }
        } else {
            status->mds_gpr[0] = 0xFFFFFFFFFFFFFFFFULL;
        }
        break;
    }

    case POC_CMD_TLBI_SYNC: {
        __asm__ volatile("tlbi alle3" ::: "memory");
        __asm__ volatile("tlbi alle1" ::: "memory");
        __asm__ volatile("dsb sy" ::: "memory");
        __asm__ volatile("isb" ::: "memory");
        status->mds_gpr[0] = 0;
        break;
    }

    case POC_CMD_CACHE_CLEAN: {
        a53_u64 va  = status->mds_gpr[1];
        a53_u64 len = status->mds_gpr[2];
        a53_u64 end = va + len;
        a53_u64 p;

        for (p = va & ~0x3fULL; p < end; p += 64) {
            __asm__ volatile("dc civac, %0" : : "r"(p) : "memory");
        }
        __asm__ volatile("dsb sy" ::: "memory");
        __asm__ volatile("isb" ::: "memory");
        status->mds_gpr[0] = 0;
        break;
    }

    case POC_CMD_READ_MSI_PARAM: {
        a53_u32 offset = (a53_u32)status->mds_gpr[1];
        main_mp4_param_t *pm;

        pm = msi_get_main_param();
        if (pm && offset < sizeof(main_mp4_param_t)) {
            status->mds_gpr[0] = *(a53_u64 *)((a53_u64)pm + (a53_u64)offset);
        } else {
            status->mds_gpr[0] = 0xFFFFFFFFFFFFFFFFULL;
        }
        break;
    }

    case POC_CMD_NONE:
    default:
        result = -1;
        status->mds_gpr[0] = 0xFFFFFFFFFFFFFFFFULL;
        break;
    }

    return result;
}
