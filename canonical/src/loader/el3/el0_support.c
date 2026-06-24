#include "a53_abi.h"
#include "a53_context.h"

extern el3_param_t g_param;
extern layout_t g_layout;
extern dev_context_t g_dev_context_mm;
extern dev_context_t g_dev_context_io;
extern mmu_el0_mm_t g_mmu_el0_mm;
extern mmu_el0_io_t g_mmu_el0_io;
extern a53_u8 core1_boot_config[];

extern void check_consistency(void);
extern void aarch64_DC_CIVAC_range_be(void *base, void *end);
extern void aarch64_write_DAIF(a53_u64 v);

extern a53_u64 __loader_el3_text_end;
extern a53_u64 __loader_el3_bss_end;
extern a53_u64 __loader_dev_text_end;
extern a53_u64 __loader_el2_end;
extern a53_u64 __loader_el1_end;
extern a53_u64 __controller_dev_text_end;
extern a53_u64 __controller_dev_data_end;
extern a53_u64 SRAM_END;
extern a53_u64 EL3_VBAR_ASM;

int A53_SECTION(".text.el3.loader")
el0_support(el3_param_t *param, a53_u32 cp_param2)
{
    a53_u64 uVar11;
    a53_s64 lVar13;
    void *p_Var14;
    dev_context_t *pdVar15;

    mp4_get_cpu();
    if (mp4_get_cpu() == 0) {
        check_consistency();
    }

    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(uVar11));
    uVar11 = (uVar11 & 0xfbf7bff8ULL) | 0x4004000ULL;
    __asm__ volatile("msr sctlr_el1, %0" : : "r"(uVar11));

    __asm__ volatile("mrs %0, sctlr_el2" : "=r"(uVar11));
    uVar11 = (uVar11 & 0xfffffff8ULL) | 0x4000ULL;
    __asm__ volatile("msr sctlr_el2, %0" : : "r"(uVar11));

    __asm__ volatile("msr scr_el3, %0" : : "r"(0x3d0eULL));

    __asm__ volatile("mrs %0, hcr_el2" : "=r"(uVar11));
    uVar11 = (uVar11 & 0x7ffffffeULL) | 0x8000038ULL;
    __asm__ volatile("msr hcr_el2, %0" : : "r"(uVar11));

    __asm__ volatile("msr vbar_el1, %0" : : "r"(0x125000ULL));
    __asm__ volatile("msr vbar_el2, %0" : : "r"(0x124000ULL));

    __asm__ volatile("mrs %0, mdcr_el2" : "=r"(uVar11));
    uVar11 = (uVar11 & 0xffffffffULL) | 6;
    __asm__ volatile("msr mdcr_el2, %0" : : "r"(uVar11));

    __asm__ volatile("mrs %0, mdcr_el3" : "=r"(uVar11));
    uVar11 = (uVar11 & 0xffffffffULL) | 0x20000ULL;
    __asm__ volatile("msr mdcr_el3, %0" : : "r"(uVar11));

    __asm__ volatile("mrs %0, pmuserenr_el0" : "=r"(uVar11));
    uVar11 = (uVar11 & 0xfffffff0ULL) | 0xf;
    __asm__ volatile("msr pmuserenr_el0, %0" : : "r"(uVar11));

    if (mp4_get_cpu() == 0) {
        lVar13 = -0x3dd000;
        __asm__ volatile("msr tpidr_el1, %0" : : "r"(0x125cb8ULL));
        __asm__ volatile("msr tpidr_el2, %0" : : "r"(0x124c68ULL));
        pdVar15 = &g_dev_context_mm;
    } else {
        param = (el3_param_t *)&param->core1_main;
        lVar13 = -0x1da000;
        __asm__ volatile("msr tpidr_el1, %0" : : "r"(0x125cf0ULL));
        pdVar15 = &g_dev_context_io;
        __asm__ volatile("msr tpidr_el2, %0" : : "r"(0x124ca0ULL));
    }

    lVar13 += (a53_s64)&core1_boot_config;
    p_Var14 = param->core0_main;

    printf_low("%d:%s:sp_el0 = 0x%016lx\n",
               (a53_u64)mp4_get_cpu(), "el0_support", lVar13);
    printf_low("%d:%s:entry  = 0x%016lx\n",
               (a53_u64)mp4_get_cpu(), "el0_support", (a53_u64)p_Var14);
    printf_low("%d:%s:dc     = 0x%016lx\n",
               (a53_u64)mp4_get_cpu(), "el0_support", (a53_u64)pdVar15);

    __asm__ volatile("msr sp_el0, %0" : : "r"(lVar13));
    __asm__ volatile("msr sp_el1, xzr");
    __asm__ volatile("msr sp_el2, xzr");

    __asm__ volatile("mrs %0, spsr_el3" : "=r"(uVar11));
    printf_low("%d:%s:%-16s = 0x%08x\n",
               (a53_u64)mp4_get_cpu(), "el0_support", "SPSR_EL3",
               (a53_u32)uVar11);

    uVar11 = (uVar11 & 0xffcffc20ULL);
    __asm__ volatile("msr spsr_el3, %0" : : "r"(uVar11));
    __asm__ volatile("msr elr_el3, %0" : : "r"(p_Var14));

    aarch64_CISW_all();
    __asm__ volatile("msr tcr_el1, %0" : : "r"(0x200c0351fULL));

    if (mp4_get_cpu() == 0) {
        /* MM core */
        mmu_el0_common_phase1(&g_mmu_el0_mm.mem_mec);
        g_mmu_el0_mm.mem_level3_03200000 =
            mmu_page_table_mgr_alloc(1, 3, 0x3200000ULL);
        g_mmu_el0_mm.mem_level3_03400000 =
            mmu_page_table_mgr_alloc(1, 3, 0x3400000ULL);
        g_mmu_el0_mm.mem_level3_03A00000 =
            mmu_page_table_mgr_alloc(1, 3, 0x3a00000ULL);
        g_mmu_el0_mm.mem_level3_04000000 =
            mmu_page_table_mgr_alloc(1, 3, 0x4000000ULL);
        g_mmu_el0_mm.mem_level3_04800000 =
            mmu_page_table_mgr_alloc(1, 3, 0x4800000ULL);
        g_mmu_el0_mm.mem_level3_04C00000 =
            mmu_page_table_mgr_alloc(1, 3, 0x4c00000ULL);
        g_mmu_el0_mm.mem_level3_04E00000 =
            mmu_page_table_mgr_alloc(1, 3, 0x4e00000ULL);
        g_mmu_el0_mm.mem_level3_05000000 =
            mmu_page_table_mgr_alloc(1, 3, 0x5000000ULL);

        mmu_page_table_set_table(g_mmu_el0_mm.mem_mec.mec_level2_00000000,
                                  g_mmu_el0_mm.mem_level3_03200000);
        mmu_page_table_set_table(g_mmu_el0_mm.mem_mec.mec_level2_00000000,
                                  g_mmu_el0_mm.mem_level3_03400000);
        mmu_page_table_set_table(g_mmu_el0_mm.mem_mec.mec_level2_00000000,
                                  g_mmu_el0_mm.mem_level3_03A00000);
        mmu_page_table_set_table(g_mmu_el0_mm.mem_mec.mec_level2_00000000,
                                  g_mmu_el0_mm.mem_level3_04000000);
        mmu_page_table_set_table(g_mmu_el0_mm.mem_mec.mec_level2_00000000,
                                  g_mmu_el0_mm.mem_level3_04800000);
        mmu_page_table_set_table(g_mmu_el0_mm.mem_mec.mec_level2_00000000,
                                  g_mmu_el0_mm.mem_level3_04C00000);
        mmu_page_table_set_table(g_mmu_el0_mm.mem_mec.mec_level2_00000000,
                                  g_mmu_el0_mm.mem_level3_04E00000);
        mmu_page_table_set_table(g_mmu_el0_mm.mem_mec.mec_level2_00000000,
                                  g_mmu_el0_mm.mem_level3_05000000);
    } else {
        /* IO core */
        mmu_el0_common_phase1(&g_mmu_el0_io.mei_mec);
        g_mmu_el0_io.mei_level2_40000000 =
            mmu_page_table_mgr_alloc(1, 2, 0x40000000ULL);
        g_mmu_el0_io.mei_level3_03600000 =
            mmu_page_table_mgr_alloc(1, 3, 0x3600000ULL);
        g_mmu_el0_io.mei_level3_03800000 =
            mmu_page_table_mgr_alloc(1, 3, 0x3800000ULL);
        g_mmu_el0_io.mei_level3_03C00000 =
            mmu_page_table_mgr_alloc(1, 3, 0x3c00000ULL);
        g_mmu_el0_io.mei_level3_04200000 =
            mmu_page_table_mgr_alloc(1, 3, 0x4200000ULL);
        g_mmu_el0_io.mei_level3_04400000 =
            mmu_page_table_mgr_alloc(1, 3, 0x4400000ULL);
        g_mmu_el0_io.mei_level3_04A00000 =
            mmu_page_table_mgr_alloc(1, 3, 0x4a00000ULL);
        g_mmu_el0_io.mei_level3_04E00000 =
            mmu_page_table_mgr_alloc(1, 3, 0x4e00000ULL);
        g_mmu_el0_io.mei_level3_05200000 =
            mmu_page_table_mgr_alloc(1, 3, 0x5200000ULL);

        mmu_page_table_set_table(g_mmu_el0_io.mei_mec.mec_level1_00000000,
                                  g_mmu_el0_io.mei_level2_40000000);
        mmu_page_table_set_table(g_mmu_el0_io.mei_mec.mec_level2_00000000,
                                  g_mmu_el0_io.mei_level3_03600000);
        mmu_page_table_set_table(g_mmu_el0_io.mei_mec.mec_level2_00000000,
                                  g_mmu_el0_io.mei_level3_03800000);
        mmu_page_table_set_table(g_mmu_el0_io.mei_mec.mec_level2_00000000,
                                  g_mmu_el0_io.mei_level3_03C00000);
        mmu_page_table_set_table(g_mmu_el0_io.mei_mec.mec_level2_00000000,
                                  g_mmu_el0_io.mei_level3_04200000);
        mmu_page_table_set_table(g_mmu_el0_io.mei_mec.mec_level2_00000000,
                                  g_mmu_el0_io.mei_level3_04400000);
        mmu_page_table_set_table(g_mmu_el0_io.mei_mec.mec_level2_00000000,
                                  g_mmu_el0_io.mei_level3_04A00000);
        mmu_page_table_set_table(g_mmu_el0_io.mei_mec.mec_level2_00000000,
                                  g_mmu_el0_io.mei_level3_04E00000);
        mmu_page_table_set_table(g_mmu_el0_io.mei_mec.mec_level2_00000000,
                                  g_mmu_el0_io.mei_level3_05200000);
    }

    /* TLB + DSB + ISB */
    __asm__ volatile("tlbi alle1" ::: "memory");
    __asm__ volatile("dsb sy" ::: "memory");
    __asm__ volatile("isb" ::: "memory");

    {
        a53_u64 mair;

        if (mp4_get_cpu() == 0) {
            __asm__("msr ttbr0_el1, %0" : :
                    "r"(g_mmu_el0_mm.mem_mec.mec_level1_00000000->mpt_table_pbase));
        } else {
            __asm__("msr ttbr0_el1, %0" : :
                    "r"(g_mmu_el0_io.mei_mec.mec_level1_00000000->mpt_table_pbase));
        }
        __asm__("mrs %0, mair_el3" : "=r"(mair));
        __asm__("msr mair_el1, %0" : : "r"(mair));
        __asm__("msr mair_el2, %0" : : "r"(mair));
    }

    /* SCTLR tweaks */
    {
        a53_u64 sctlr;

        __asm__("mrs %0, sctlr_el1" : "=r"(sctlr));
        sctlr = (sctlr & 0xfff72ff0ULL) | 0xd005ULL;
        __asm__("msr sctlr_el1, %0" : : "r"(sctlr));
    }
    {
        a53_u64 sctlr;

        __asm__("mrs %0, sctlr_el2" : "=r"(sctlr));
        sctlr = (sctlr & 0xfff7eff9ULL) | 0x1006ULL;
        __asm__("msr sctlr_el2, %0" : : "r"(sctlr));
    }

    aarch64_DC_CIVAC_range_be((void *)EL3_VBAR_ASM, (void *)SRAM_END);

    if (mp4_get_cpu() == 0) {
        /* MM core phase 2 */
        main_mp4_param_t *pm;

        pm = msi_get_main_param();
        mmu_el0_common_phase2(&g_mmu_el0_mm.mem_mec);

        mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_mec.mec_level2_00000000,
                                     0x28000000ULL, 0x18000000ULL, 0x800000ULL,
                                     map_mode_rw_rw, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_mec.mec_level2_00000000,
                                     0x10000000ULL, 0x4000000ULL, 0x10000000ULL,
                                     map_mode_rw_rw, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_level3_03200000,
                                     0x3200000ULL, 0x1000000ULL, 0x10000ULL,
                                     map_mode_rw_rw, mem_type_so);
        mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_level3_03A00000,
                                     0x3a00000ULL, 0xec000000ULL, 0x100000ULL,
                                     map_mode_rw_rw, mem_type_so);
        mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_level3_03A00000,
                                     0x3b01000ULL, 0xbd110000ULL, 0x1000ULL,
                                     map_mode_rw_rw, mem_type_so);

        {
            a53_u64 iova;

            iova = (a53_u64)((a53_s32)(pm->mm4p_mm_param.mimi_iommu_addr
                                        + 0x48000000ULL)
                              - (a53_s32)pm->mm4p_mm_param.mimi_syshub_base);
            printf_low("%d:%s:EL0:MM:MAP: 0x%016lx -> 0x%016lx\n",
                       (a53_u64)mp4_get_cpu(), "mmu_el0_mm_phase2",
                       0x4001000ULL, iova);
            mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_level3_04000000,
                                         0x4001000ULL, iova, 0x50000ULL,
                                         map_mode_rw_rw, mem_type_so);
        }
        mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_level3_04000000,
                                     0x4040000ULL, 0x40000ULL, 0x40000ULL,
                                     map_mode_rw_rw, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_level3_04000000,
                                     0x4080000ULL, 0x88500000ULL, 0x40000ULL,
                                     map_mode_rw_rw, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_level3_04800000,
                                     0x4820000ULL, 0x20000ULL, 0x3000ULL,
                                     map_mode_rw_rw, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_level3_04C00000,
                                     0x4c00000ULL, 0, 0x6000ULL,
                                     map_mode_rx_rx, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_level3_04C00000,
                                     0x4c06000ULL, 0x6000ULL, 0x6000ULL,
                                     map_mode_rw_rw, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_level3_05000000,
                                     0x501c000ULL, 0x1c000ULL, 0x2000ULL,
                                     map_mode_rx_rx, mem_type_memory);
    } else {
        /* IO core phase 2 */
        mmu_el0_common_phase2(&g_mmu_el0_io.mei_mec);

        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_mec.mec_level2_00000000,
                                     0x30000000ULL, 0x1fc00000ULL, 0x200000ULL,
                                     map_mode_rw_rw, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_level2_40000000,
                                     0x40000000ULL, 0x15000000ULL, 0x2a00000ULL,
                                     map_mode_rw_rw, mem_type_so);
        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_mec.mec_level3_02200000,
                                     0x22e0000ULL, 0x32e0000ULL, 0x20000ULL,
                                     map_mode_rw_rw, mem_type_so);
        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_level3_03800000,
                                     0x3800000ULL, 0x34200000ULL, 0x10000ULL,
                                     map_mode_rw_rw, mem_type_so);
        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_level3_03C00000,
                                     0x3c00000ULL, 0xec100000ULL, 0x100000ULL,
                                     map_mode_rw_rw, mem_type_so);
        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_level3_03C00000,
                                     0x3d01000ULL, 0xbd110000ULL, 0x1000ULL,
                                     map_mode_rw_rw, mem_type_so);
        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_level3_04200000,
                                     0x4227000ULL, 0x27000ULL, 0x19000ULL,
                                     map_mode_rw_rw, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_level3_04400000,
                                     0x4426000ULL, 0x26000ULL, 0x1000ULL,
                                     map_mode_rw_rw, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_level3_04A00000,
                                     0x4a23000ULL, 0x23000ULL, 0x3000ULL,
                                     map_mode_rw_rw, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_level3_04E00000,
                                     0x4e0c000ULL, 0xc000ULL, 0x8000ULL,
                                     map_mode_rx_rx, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_level3_04E00000,
                                     0x4e77000ULL, 0x14000ULL, 0x8000ULL,
                                     map_mode_rw_rw, mem_type_memory);
        mmu_page_table_map_range_bs(g_mmu_el0_io.mei_level3_05200000,
                                     0x521e000ULL, 0x1e000ULL, 0x2000ULL,
                                     map_mode_rx_rx, mem_type_memory);
    }

    /* Phase 3: Boot config */
    {
        a53_u8 *boot_cfg_out = (a53_u8 *)0;

        if (mp4_get_cpu() == 0) {
            main_mp4_param_t *pm;
            a53_u64 iova;

            boot_config_mm_phase1();
            boot_config_mm_phase2(&boot_cfg_out);

            printf_low("%d:%s:(mem %p)\n",
                       (a53_u64)mp4_get_cpu(), "mmu_el0_mm_phase3",
                       &g_mmu_el0_mm);

            pm = msi_get_main_param();
            iova = (a53_u64)((a53_s32)(pm->mm4p_sdma0_mmio.mimi_iommu_addr
                                        + 0x1100000ULL)
                              - (a53_s32)pm->mm4p_sdma0_mmio.mimi_syshub_base);
            printf_low("%d:%s:EL0:MM:MAP: 0x%016lx -> 0x%016lx\n",
                       (a53_u64)mp4_get_cpu(), "mmu_el0_mm_phase3",
                       0x3400000ULL, iova);
            mmu_page_table_map_range_bs(g_mmu_el0_mm.mem_level3_03400000,
                                         0x3400000ULL, iova, 0x10000ULL,
                                         map_mode_rw_rw, mem_type_so);
        } else {
            main_mp4_param_t *pm;
            a53_u64 iova;

            boot_config_io_phase1();
            boot_config_io_phase2(&boot_cfg_out);

            printf_low("%d:%s:(mei %p)\n",
                       (a53_u64)mp4_get_cpu(), "mmu_el0_io_phase3",
                       &g_mmu_el0_io);

            pm = msi_get_main_param();
            iova = (a53_u64)((a53_s32)(pm->mm4p_sdma1_mmio.mimi_iommu_addr
                                        + 0x1100000ULL)
                              - (a53_s32)pm->mm4p_sdma1_mmio.mimi_syshub_base);
            printf_low("%d:%s:EL0:IO:MAP: 0x%016lx -> 0x%016lx\n",
                       (a53_u64)mp4_get_cpu(), "mmu_el0_io_phase3",
                       0x3600000ULL, iova);
            mmu_page_table_map_range_bs(g_mmu_el0_io.mei_level3_03600000,
                                         0x3600000ULL, iova, 0x10000ULL,
                                         map_mode_rw_rw, mem_type_so);
        }
    }

    /* Print EL3 state */
    printf_low("%d:%s:===== ===== EL3 ===== =====\n",
               (a53_u64)mp4_get_cpu(), "el0_support");
    aarch64_print_SCR_EL3();
    aarch64_print_SCTLR_EL3();
    aarch64_print_SPSR_EL3();
    aarch64_print_ESR_EL3();

    {
        a53_u64 tpidr;

        __asm__("mrs %0, tpidr_el3" : "=r"(tpidr));
        printf_low("%d:%s:TPIDR_EL3   = %p\n",
                   (a53_u64)mp4_get_cpu(), "el0_support", (void *)tpidr);
    }

    /* Print more state... */
    aarch64_print_HCR_EL2();
    aarch64_print_SCTLR_EL2();
    aarch64_print_SPSR_EL2();
    aarch64_print_ESR_EL2();
    aarch64_print_SCTLR_EL1();
    aarch64_print_SPSR_EL1();
    aarch64_print_ESR_EL1();

    /* MSI test if requested */
    if (cp_param_check(0x8000) != 0) {
        msi_test();
    }

    /* DAIF */
    {
        a53_u64 daif;

        daif = aarch64_read_DAIF();
        printf_low("%d:%s:DAIF = 0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "el0_support", (a53_u32)daif);
        aarch64_write_DAIF(daif | 0x800ULL);
    }

    /* PMU setup */
    __asm__("msr pmevtyper0_el0, %0" : : "r"(0x8000001ULL));
    __asm__("msr pmevtyper1_el0, %0" : : "r"(0x8000002ULL));
    __asm__("msr pmevtyper2_el0, %0" : : "r"(0x8000003ULL));
    __asm__("msr pmevtyper3_el0, %0" : : "r"(0x8000005ULL));
    __asm__("msr pmevtyper4_el0, %0" : : "r"(0x14ULL));
    __asm__("msr pmevtyper5_el0, %0" : : "r"(0x6000004ULL));
    __asm__("msr pmcntenset_el0, %0" : : "r"(0x8000003fULL));
    __asm__("msr pmcr_el0, %0" : : "r"(7ULL));

    /* MMU map print */
    if (mp4_get_cpu() == 0) {
        mmu_el0_common_get_map_low(&g_mmu_el0_mm.mem_mec);
        mmu_page_table_get_map(g_mmu_el0_mm.mem_level3_03200000);
        mmu_page_table_get_map(g_mmu_el0_mm.mem_level3_03400000);
        mmu_page_table_get_map(g_mmu_el0_mm.mem_level3_03A00000);
        mmu_page_table_get_map(g_mmu_el0_mm.mem_level3_04000000);
        mmu_page_table_get_map(g_mmu_el0_mm.mem_level3_04800000);
        mmu_page_table_get_map(g_mmu_el0_mm.mem_level3_04C00000);
        mmu_page_table_get_map(g_mmu_el0_mm.mem_level3_04E00000);
        mmu_page_table_get_map(g_mmu_el0_mm.mem_level3_05000000);
        mmu_el0_common_get_map_high(&g_mmu_el0_mm.mem_mec);
    } else {
        mmu_el0_common_get_map_low(&g_mmu_el0_io.mei_mec);
        mmu_page_table_get_map(g_mmu_el0_io.mei_level3_03600000);
        mmu_page_table_get_map(g_mmu_el0_io.mei_level3_03800000);
        mmu_page_table_get_map(g_mmu_el0_io.mei_level3_03C00000);
        mmu_page_table_get_map(g_mmu_el0_io.mei_level3_04200000);
        mmu_page_table_get_map(g_mmu_el0_io.mei_level3_04400000);
        mmu_page_table_get_map(g_mmu_el0_io.mei_level3_04A00000);
        mmu_page_table_get_map(g_mmu_el0_io.mei_level3_04E00000);
        mmu_page_table_get_map(g_mmu_el0_io.mei_level3_05200000);
        mmu_el0_common_get_map_high(&g_mmu_el0_io.mei_mec);
        mmu_page_table_get_map(g_mmu_el0_io.mei_level2_40000000);
    }

    printf_low("%d:%s:*************************************************************\n",
               (a53_u64)mp4_get_cpu(), "el0_support");
    printf_low("%d:%s:call eret: 0x%016lx: spsr_el3 0x%08x\n",
               (a53_u64)mp4_get_cpu(), "el0_support",
               (a53_u64)p_Var14, (a53_u32)uVar11);
    printf_low("%d:%s:*************************************************************\n",
               (a53_u64)mp4_get_cpu(), "el0_support");

    /* Final ERET setup */
    __asm__ volatile("msr spsr_el3, %0" : : "r"(uVar11));
    __asm__ volatile("msr elr_el3, %0" : : "r"(p_Var14));
    __asm__ volatile("msr tpidrro_el0, %0" : : "r"(pdVar15));

    aarch64_CISW_all();

    /* IC IALLU + DSB + ISB (twice) */
    __asm__ volatile("ic iallu" ::: "memory");
    __asm__ volatile("dsb sy" ::: "memory");
    __asm__ volatile("isb" ::: "memory");
    __asm__ volatile("ic iallu" ::: "memory");
    __asm__ volatile("dsb sy" ::: "memory");
    __asm__ volatile("isb" ::: "memory");

    __asm__ volatile("eret");

    return (int)cp_param2;
}

a53_u8 *A53_SECTION(".text.el3.loader") el0_va_to_el3_va(a53_u8 *el0_va0)
{
    a53_u64 par;
    a53_u64 pa;
    a53_u8 *puVar6;

    __asm__ volatile("at s1e1r, %1; mrs %0, par_el1"
                     : "=r"(par) : "r"(el0_va0) : "memory");

    if ((par & 1) == 0) {
        pa = (par & 0xfffffffff000ULL) | ((a53_u64)el0_va0 & 0xfffULL);
        puVar6 = (a53_u8 *)pa;

        if (puVar6 == (a53_u8 *)0) {
            printf_low("%d:%s:S1E1R: PAR_EL1: 0x%016lx <- %p\n",
                       (a53_u64)mp4_get_cpu(), "el0_va_to_el3_va",
                       par, el0_va0);
            puVar6 = (a53_u8 *)0x4200000ULL;
        } else if ((par & 0xfffffc000000ULL) != 0x88000000ULL
                   && puVar6 > (a53_u8 *)0x7fffULL) {
            printf_low("%d:%s:pa: 0x%016lx (not SRAM)\n",
                       (a53_u64)mp4_get_cpu(), "el0_va_to_el3_va",
                       (a53_u64)puVar6);
            el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\el0_support.c",
                       "el0_va_to_el3_va", 0x573, 0, "0");
        }

        {
            a53_u64 par3;

            __asm__ volatile("at s1e3r, %1; mrs %0, par_el1"
                             : "=r"(par3) : "r"(puVar6) : "memory");
            if ((par & 0xfffffffff000ULL) != (par3 & 0xfffffffff000ULL)) {
                printf_low("%d:%s:pa0 0x%016lx != 0x%016lx pa3 (0x%016lx)\n",
                           (a53_u64)mp4_get_cpu(), "el0_va_to_el3_va",
                           par, par3, (a53_u64)puVar6);
                el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\el0_support.c",
                           "el0_va_to_el3_va", 0x57c, 0, "0");
            }
        }
    } else {
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\el0_support.c",
                   "el0_va_to_el3_va", 0x55f, 0, "0");
        puVar6 = (a53_u8 *)0;
    }
    return puVar6;
}

int A53_SECTION(".text.el3.loader")
mmu_el0_common_phase1(mmu_el0_common_t *mec)
{
    mec->mec_level1_00000000 = mmu_page_table_mgr_alloc(1, 1, 0);
    mec->mec_level2_00000000 = mmu_page_table_mgr_alloc(1, 2, 0);
    mec->mec_level3_00000000 = mmu_page_table_mgr_alloc(1, 3, 0);
    mec->mec_level3_02000000 = mmu_page_table_mgr_alloc(1, 3, 0x2000000ULL);
    mec->mec_level3_02200000 = mmu_page_table_mgr_alloc(1, 3, 0x2200000ULL);
    mec->mec_level3_03000000 = mmu_page_table_mgr_alloc(1, 3, 0x3000000ULL);

    mmu_page_table_set_table(mec->mec_level1_00000000,
                              mec->mec_level2_00000000);
    mmu_page_table_set_table(mec->mec_level2_00000000,
                              mec->mec_level3_00000000);
    mmu_page_table_set_table(mec->mec_level2_00000000,
                              mec->mec_level3_02000000);
    mmu_page_table_set_table(mec->mec_level2_00000000,
                              mec->mec_level3_02200000);
    return mmu_page_table_set_table(mec->mec_level2_00000000,
                                     mec->mec_level3_03000000);
}

int A53_SECTION(".text.el3.loader")
mmu_el0_common_phase2(mmu_el0_common_t *mec)
{
    a53_u64 pbegin;

    mmu_page_table_map_range_be(mec->mec_level3_00000000,
                                 0x1000ULL, 0x1000ULL, 0x10000ULL,
                                 map_mode_rx_rx, mem_type_memory);
    mmu_page_table_map_range_be(mec->mec_level3_00000000,
                                 0x125000ULL, 0x88025000ULL, 0x125d28ULL,
                                 map_mode_rx_rx, mem_type_memory);
    mmu_page_table_map_range_be(mec->mec_level3_00000000,
                                 0x124000ULL, 0x88024000ULL, 0x124cd8ULL,
                                 map_mode_rx_rx, mem_type_memory);

    mmu_page_table_map_range_bs(mec->mec_level2_00000000,
                                 0x6000000ULL, 0x88200000ULL, 0x200000ULL,
                                 map_mode_rx_rx, mem_type_memory);
    mmu_page_table_map_range_bs(mec->mec_level2_00000000,
                                 0x6200000ULL, 0x88400000ULL, 0x200000ULL,
                                 map_mode_rw_rw, mem_type_memory);

    pbegin = mmu_va_to_pa((void *)mp4_get_cpu);
    mmu_page_table_map_range_be(mec->mec_level3_00000000,
                                 0x117000ULL, pbegin, 0x1232d0ULL,
                                 map_mode_rx_rx, mem_type_memory);

    mmu_page_table_map_range_bs(mec->mec_level3_02000000,
                                 0x2010000ULL, 0x3010000ULL, 0x10000ULL,
                                 map_mode_rw_rw, mem_type_so);
    mmu_page_table_map_range_bs(mec->mec_level3_02000000,
                                 0x2060000ULL, 0x3060000ULL, 0x10000ULL,
                                 map_mode_rw_rw, mem_type_so);
    mmu_page_table_map_range_bs(mec->mec_level3_02000000,
                                 0x2070000ULL, 0x3070000ULL, 0x10000ULL,
                                 map_mode_rw_rw, mem_type_so);
    mmu_page_table_map_range_bs(mec->mec_level3_02000000,
                                 0x20c0000ULL, 0x30c0000ULL, 0x30000ULL,
                                 map_mode_rw_rw, mem_type_so);
    mmu_page_table_map_range_bs(mec->mec_level3_02200000,
                                 0x2200000ULL, 0x3200000ULL, 0x1000ULL,
                                 map_mode_rw_rw, mem_type_so);
    mmu_page_table_map_range_bs(mec->mec_level3_02200000,
                                 0x2290000ULL, 0x3290000ULL, 0x10000ULL,
                                 map_mode_rw_rw, mem_type_so);
    mmu_page_table_map_range_bs(mec->mec_level3_02200000,
                                 0x22c0000ULL, 0x32c0000ULL, 0x10000ULL,
                                 map_mode_rw_rw, mem_type_so);
    return mmu_page_table_map_range_bs(mec->mec_level3_03000000,
                                        0x3000000ULL, 0xf6e00000ULL,
                                        0x200000ULL,
                                        map_mode_rw_rw, mem_type_so);
}

int A53_SECTION(".text.el3.loader")
mmu_el0_common_get_map_low(mmu_el0_common_t *mec)
{
    printf_low("%d:%s:(mec %p)\n",
               (a53_u64)mp4_get_cpu(), "mmu_el0_common_get_map_low", mec);
    mmu_page_table_get_map(mec->mec_level3_00000000);
    mmu_page_table_get_map(mec->mec_level3_02000000);
    mmu_page_table_get_map(mec->mec_level3_02200000);
    return mmu_page_table_get_map(mec->mec_level3_03000000);
}

int A53_SECTION(".text.el3.loader")
mmu_el0_common_get_map_high(mmu_el0_common_t *mec)
{
    printf_low("%d:%s:(mec %p)\n",
               (a53_u64)mp4_get_cpu(), "mmu_el0_common_get_map_high", mec);
    mmu_page_table_get_map(mec->mec_level2_00000000);
    return mmu_page_table_get_map(mec->mec_level1_00000000);
}
