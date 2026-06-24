#include "a53_abi.h"
#include "a53_context.h"

extern a53_u8 g_cbar_el1;

void A53_SECTION(".text.el3.loader") writel(a53_u64 addr, a53_u32 val)
{
    *(volatile a53_u32 *)addr = val;
}

a53_u32 A53_SECTION(".text.el3.loader") readl(a53_u64 addr)
{
    return *(volatile a53_u32 *)addr;
}

static void A53_SECTION(".text.el3.loader")
syshub_tlb_init_iommu(int tlb, a53_u64 phy_addr, a53_u32 seg_size)
{
    a53_u32 v;
    a53_u32 tlb1;
    a53_u32 uVar1;

    if (seg_size == 0x20000) {
        v = 0;
    } else if (seg_size == 0x4000000) {
        v = 0x12;
    } else if (seg_size == 0x80000) {
        v = 4;
    } else if (seg_size == 0x100000) {
        v = 6;
    } else if (seg_size == 0x200000) {
        v = 8;
    } else if (seg_size == 0x400000) {
        v = 10;
    } else if (seg_size == 0x800000) {
        v = 0xc;
    } else if (seg_size == 0x1000000) {
        v = 0xe;
    } else if (seg_size == 0x2000000) {
        v = 0x10;
    } else if (seg_size == 0x40000) {
        v = 2;
    } else {
        printf_low("Unsupport seg_size 0x%08x\n", (a53_u64)seg_size);
        v = 0x12;
    }

    uVar1 = (a53_u32)phy_addr & 0xfc000000U;
    tlb1 = (a53_u32)(phy_addr - uVar1);
    if (tlb1 != 0) {
        v |= tlb1 >> 12 & 0xfffe0;
        printf_low("tlb=%d, 0x%08x, offset=0x%08x, tlb1=0x%08x\n",
                   (a53_u64)(a53_u32)tlb, (a53_u64)uVar1,
                   (a53_u64)tlb1, (a53_u64)v);
    }

    {
        a53_u32 idx;
        volatile a53_u32 *p;

        idx = (a53_u32)(tlb - 1);
        p = (volatile a53_u32 *)(a53_u64)(idx * 0x10 + 0x3230000);
        p[0] = (a53_u32)(phy_addr >> 26);
        p[1] = v;
        p[2] = 0x40;
        p[3] = 0x40;
        *(volatile a53_u32 *)(a53_u64)(idx * 4 + 0x32303e0) = 0xffffffff;
        if (tlb != 0x3e) {
            *(volatile a53_u32 *)(a53_u64)(idx * 4 + 0x32304d8) = 0xc1800003;
        }
    }
}

int A53_SECTION(".text.el3.loader") syshub_init(void)
{
    volatile a53_u32 *p;

    p = (volatile a53_u32 *)0x03230040UL;
    p[0] = 0x14;   p[1] = 0x12;  p[2] = 0x40;  p[3] = 0x40;
    *(volatile a53_u32 *)0x032303f0UL = 0xffffffff;
    *(volatile a53_u32 *)0x032304e8UL = 0xc1800003;

    p = (volatile a53_u32 *)0x03230060UL;
    p[0] = 0x14;   p[1] = 0x12;  p[2] = 0x40;  p[3] = 0x40;
    *(volatile a53_u32 *)0x032303f8UL = 0xffffffff;
    *(volatile a53_u32 *)0x032304f0UL = 0xc1800003;

    syshub_tlb_init_iommu(8, 0x53a00000UL, 0x200000);

    p = (volatile a53_u32 *)0x032302b0UL;
    p[0] = 0x18;   p[1] = 0x12;  p[2] = 0x40;  p[3] = 0x40;
    *(volatile a53_u32 *)0x0323048cUL = 0xffffffff;
    *(volatile a53_u32 *)0x03230584UL = 0xc1800003;

    p = (volatile a53_u32 *)0x032302c0UL;
    p[0] = 0x14;   p[1] = 0x12;  p[2] = 0x40;  p[3] = 0x40;
    *(volatile a53_u32 *)0x03230490UL = 0xffffffff;
    *(volatile a53_u32 *)0x03230588UL = 0xc1800003;

    p = (volatile a53_u32 *)0x032302d0UL;
    p[0] = 0x14;   p[1] = 0x12;  p[2] = 0;     p[3] = 0;
    *(volatile a53_u32 *)0x03230494UL = 0xffffffff;
    *(volatile a53_u32 *)0x0323058cUL = 0xc1800003;
    return 0;
}

void A53_SECTION(".text.el3.loader")
mp4_iommu_map_info_printf(mp4_iommu_map_info_t *mimi, char *name)
{
    a53_u32 cpu;

    cpu = mp4_get_cpu();
    printf_low("%d:%s: %s.mimi_pa            = 0x%016lx\n",
               (a53_u64)cpu, "mp4_iommu_map_info_printf",
               name, mimi->mimi_pa);
    printf_low("%d:%s: %s.mimi_pa_base       = 0x%016lx\n",
               (a53_u64)cpu, "mp4_iommu_map_info_printf",
               name, mimi->mimi_pa_base);
    printf_low("%d:%s: %s.mimi_iommu_addr    = 0x%016lx\n",
               (a53_u64)cpu, "mp4_iommu_map_info_printf",
               name, mimi->mimi_iommu_addr);
    printf_low("%d:%s: %s.mimi_physical_size = 0x%016lx\n",
               (a53_u64)cpu, "mp4_iommu_map_info_printf",
               name, mimi->mimi_physical_size);
    printf_low("%d:%s: %s.mimi_iommu_size    = 0x%016lx\n",
               (a53_u64)cpu, "mp4_iommu_map_info_printf",
               name, mimi->mimi_iommu_size);
    printf_low("%d:%s: %s.mimi_syshub_base   = 0x%016lx\n",
               (a53_u64)cpu, "mp4_iommu_map_info_printf",
               name, mimi->mimi_syshub_base);
}

int A53_SECTION(".text.el3.loader") syshub_init_after_main_param(void)
{
    main_mp4_param_t *pm;
    a53_u32 cpu;

    cpu = mp4_get_cpu();
    printf_low("%d:%s:()\n", (a53_u64)cpu, "syshub_init_after_main_param");
    pm = msi_get_main_param();
    cpu = mp4_get_cpu();
    printf_low("%d:%s: mm4p_mapper_page_table_ioma     = 0x%016lx\n",
               (a53_u64)cpu, "syshub_init_after_main_param",
               pm->mm4p_mapper_page_table_ioma);
    cpu = mp4_get_cpu();
    printf_low("%d:%s: mm4p_iommu_mmio.mimi_iommu_addr = 0x%016lx\n",
               (a53_u64)cpu, "syshub_init_after_main_param",
               pm->mm4p_iommu_mmio.mimi_iommu_addr);

    syshub_tlb_init_iommu(1, pm->mm4p_mapper_page_table_ioma, 0x4000000);
    syshub_tlb_init_iommu(2, pm->mm4p_mapper_page_table_ioma + 0x4000000, 0x4000000);
    syshub_tlb_init_iommu(3, pm->mm4p_mapper_page_table_ioma + 0x8000000, 0x4000000);
    syshub_tlb_init_iommu(4, pm->mm4p_mapper_page_table_ioma + 0xc000000, 0x4000000);
    syshub_tlb_init_iommu(5, pm->mm4p_scf_buf_iommu_addr, 0x4000000);
    syshub_tlb_init_iommu(6, pm->mm4p_mapper_private_ioma, 0x2000000);

    {
        volatile a53_u32 *p;

        p = (volatile a53_u32 *)0x03230090UL;
        p[0] = 0x15;  p[1] = 0x12;  p[2] = 0x40;  p[3] = 0x40;
        *(volatile a53_u32 *)0x03230404UL = 0xffffffff;
        *(volatile a53_u32 *)0x032304fcUL = 0xc1800003;

        p = (volatile a53_u32 *)0x032300e0UL;
        p[0] = 0x3f;  p[1] = 0x12;  p[2] = 4;     p[3] = 4;
        *(volatile a53_u32 *)0x03230418UL = 0xffffffff;
        *(volatile a53_u32 *)0x03230510UL = 0xc1800003;
    }

    mp4_iommu_map_info_printf(&pm->mm4p_mm_param, "mm_param");
    mp4_iommu_map_info_printf(&pm->mm4p_io_param, "io_param");
    syshub_tlb_init_iommu(0x12, pm->mm4p_mm_param.mimi_syshub_base, 0x4000000);
    syshub_tlb_init_iommu(0x13, pm->mm4p_io_param.mimi_syshub_base, 0x4000000);
    return 0;
}

int A53_SECTION(".text.el3.loader") syshub_init_sdma(void)
{
    main_mp4_param_t *pm;
    a53_u32 cpu;

    cpu = mp4_get_cpu();
    printf_low("%d:%s:()\n", (a53_u64)cpu, "syshub_init_sdma");
    pm = msi_get_main_param();
    mp4_iommu_map_info_printf(&pm->mm4p_sdma0_mmio, "sdma0_mmio");
    mp4_iommu_map_info_printf(&pm->mm4p_sdma1_mmio, "sdma0_mmio");
    mp4_iommu_map_info_printf(&pm->mm4p_sdma0_rb, "sdma1_rb");
    mp4_iommu_map_info_printf(&pm->mm4p_sdma1_rb, "sdma1_rb");
    syshub_tlb_init_iommu(0xb, pm->mm4p_sdma0_mmio.mimi_syshub_base, 0x4000000);
    syshub_tlb_init_iommu(0xc, pm->mm4p_sdma1_mmio.mimi_syshub_base, 0x4000000);
    syshub_tlb_init_iommu(0x10, pm->mm4p_sdma0_rb.mimi_syshub_base, 0x4000000);
    syshub_tlb_init_iommu(0x11, pm->mm4p_sdma1_rb.mimi_syshub_base, 0x4000000);
    return 0;
}

int A53_SECTION(".text.el3.loader") syshub_init_for_io(a53_u64 base)
{
    syshub_tlb_init_iommu(0x15, base, 0x4000000);
    return 0;
}

void A53_SECTION(".text.el3.loader")
syshub_tlb_get(a53_u32 tlb, a53_u32 *tlb0, a53_u32 *tlb1,
               a53_u32 *tlb2, a53_u32 *tlb3,
               a53_u32 *sub, a53_u32 *attr1)
{
    a53_u32 idx;
    volatile a53_u32 *p;

    idx = tlb - 1;
    p = (volatile a53_u32 *)(a53_u64)(idx * 0x10 + 0x3230000);
    *tlb0 = p[0];
    *tlb1 = p[1];
    *tlb2 = p[2];
    *tlb3 = p[3];
    *sub = *(volatile a53_u32 *)(a53_u64)(idx * 4 + 0x32303e0);
    *attr1 = *(volatile a53_u32 *)(a53_u64)(idx * 4 + 0x32304d8);
}
