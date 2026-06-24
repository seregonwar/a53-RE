#include "a53_abi.h"
#include "a53_context.h"

extern a53_u8 core0_boot_config[128];
extern a53_u8 core1_boot_config[128];
extern a53_u8 *el0_va_to_el3_va(a53_u8 *el0_va0);

IoController_BootConfiguration *A53_SECTION(".text.el3.loader")
IoController_BootConfiguration_get(void)
{
    return (IoController_BootConfiguration *)
        el0_va_to_el3_va(core0_boot_config);
}

MmController_BootConfiguration *A53_SECTION(".text.el3.loader")
MmController_BootConfiguration_get(void)
{
    return (MmController_BootConfiguration *)
        el0_va_to_el3_va(core1_boot_config);
}

int A53_SECTION(".text.el3.loader")
boot_config_io_io(IoController_BootConfiguration *dst,
                   IoController_BootConfiguration *src)
{
    printf_low("%d:%s:(dst %p, src %p)\n",
               (a53_u64)mp4_get_cpu(), "boot_config_io_io", dst, src);
    printf_low("%d:%s:->memcpy(%p, %p)\n",
               (a53_u64)mp4_get_cpu(), "boot_config_io_io", dst, src);
    memcpy(dst, src, 0x38);
    printf_low("%d:%s:->memcpy(%p, %p)\n",
               (a53_u64)mp4_get_cpu(), "boot_config_io_io",
               (a53_u8 *)&dst->field_0 + 0x38,
               (a53_u8 *)&src->field_0 + 0x38);
    memcpy((a53_u8 *)&dst->field_0 + 0x38,
           (a53_u8 *)&src->field_0 + 0x38, 8);
    printf_low("%d:%s:->memcpy(%p, %p)\n",
               (a53_u64)mp4_get_cpu(), "boot_config_io_io",
               (a53_u8 *)&dst->field_0 + 0x50,
               (a53_u8 *)&src->field_0 + 0x50);
    memcpy((a53_u8 *)&dst->field_0 + 0x50,
           (a53_u8 *)&src->field_0 + 0x50, 0x20);
    return 0;
}

a53_u64 A53_SECTION(".text.el3.loader") bits_27_12(a53_u64 v)
{
    if ((v & 0xfffffffff0000fffULL) != 0) {
        printf_low("%d:%s:Warning: 0x%16lx\n",
                   (a53_u64)mp4_get_cpu(), "bits_27_12", v);
    }
    return (v >> 12) & 0xffffULL;
}

a53_u64 A53_SECTION(".text.el3.loader") bits_18_12(a53_u64 v)
{
    if ((v & 0xfffffffffff80fffULL) != 0) {
        printf_low("%d:%s:Warning: 0x%16lx\n",
                   (a53_u64)mp4_get_cpu(), "bits_18_12", v);
    }
    return (v >> 12) & 0x7fULL;
}

int A53_SECTION(".text.el3.loader")
boot_config_dram_entry(IoController_BootConfiguration *dst,
                        a53_u64 addr0, a53_u64 newbase)
{
    IoController_BootConfiguration *src;

    printf_low("%d:%s:(dst %p, addr0 %p, newbase 0x%016lx)\n",
               (a53_u64)mp4_get_cpu(), "boot_config_dram_entry",
               dst, (void *)addr0, newbase);
    src = (IoController_BootConfiguration *)((addr0 & 0x3ffffffULL) + newbase);
    printf_low("%d:%s:offset 0x%016lx\n",
               (a53_u64)mp4_get_cpu(), "boot_config_dram_entry",
               addr0 & 0x3ffffffULL);
    printf_low("%d:%s:src    0x%016lx\n",
               (a53_u64)mp4_get_cpu(), "boot_config_dram_entry",
               (a53_u64)src);
    boot_config_io_io(dst, src);
    return 0;
}

int A53_SECTION(".text.el3.loader") boot_config_mm_phase1(void)
{
    MmController_BootConfiguration *pMVar2;

    printf_low("%d:%s:()\n", (a53_u64)mp4_get_cpu(), "boot_config_mm_phase1");
    pMVar2 = MmController_BootConfiguration_get();
    printf_low("%d:%s:dst = %p\n",
               (a53_u64)mp4_get_cpu(), "boot_config_mm_phase1", pMVar2);
    pMVar2->field_0.asU64[8] = 0x4820501c44264040ULL;
    pMVar2->field_0.asU64[9] = 0x389a00c118c04c00ULL;
    return 0;
}

int A53_SECTION(".text.el3.loader") boot_config_io_phase1(void)
{
    IoController_BootConfiguration *pIVar2;

    pIVar2 = IoController_BootConfiguration_get();
    printf_low("%d:%s:dst = %p\n",
               (a53_u64)mp4_get_cpu(), "boot_config_io_phase1", pIVar2);
    pIVar2->field_0.asU64[8] = 0x4a23521e44264227ULL;
    pIVar2->field_0.asU64[9] = 0x3c99390118994e0cULL;
    return 0;
}

int A53_SECTION(".text.el3.loader") boot_config_mm_phase2(a53_u8 **out)
{
    main_mp4_param_t *pm;
    MmController_BootConfiguration *dest;
    a53_u32 uVar6;
    a53_u32 uVar7;
    a53_u16 uVar4;
    a53_u64 uVar9;

    pm = msi_get_main_param();
    dest = MmController_BootConfiguration_get();
    printf_low("%d:%s:Waiting SYNC MM driver by DVM_MAILBOX %d - %d\n",
               (a53_u64)mp4_get_cpu(), "boot_config_mm_phase2", 0xe, 0xf);
    do {
        uVar6 = dvm_read_mailbox(0xe);
        uVar7 = dvm_read_mailbox(0xf);
    } while ((uVar7 | uVar6) == 0);

    printf_low("%d:%s:dvm_mailbox_l 0x%016lx\n",
               (a53_u64)mp4_get_cpu(), "boot_config_mm_phase2",
               (a53_u64)uVar6);
    printf_low("%d:%s:dvm_mailbox_h 0x%016lx\n",
               (a53_u64)mp4_get_cpu(), "boot_config_mm_phase2",
               (a53_u64)uVar7);

    memcpy(dest,
           (void *)(a53_u64)((a53_s32)(pm->mm4p_mm_param.mimi_iommu_addr + 0x48000000ULL)
                              - (a53_s32)pm->mm4p_mm_param.mimi_syshub_base),
           0x80);

    dest->field_0.asU64[7] =
        (dest->field_0.asU64[7] & 0xfff00000ffffffffULL) | 0x1800000000000ULL;
    dest->field_0.asU64[0xe] =
        (dest->field_0.asU64[0xe] & 0xffffffffffffe000ULL) | 10;

    uVar4 = msi_get_vector_for_mm();
    uVar9 = (dest->field_0.asU64[4] & 0xf0000000000ULL)
        | (dest->field_0.asU64[4] & 0xffffffffULL)
        | ((a53_u64)(uVar4 & 0xffU) << 32)
        | 0x87f0000000000000ULL;
    dest->field_0.asU64[4] = uVar9;
    dest->field_0.asU64[5] =
        dest->field_0.asU64[5] & 0xffffffffff7fffffULL;
    dest->field_0.asU64[4] = uVar9;

    dest->field_0.asU64[5] =
        (a53_u64)((a53_u32)((pm->mm4p_sdma0_mmio.mimi_iommu_addr + 0x28000000ULL)
                             - pm->mm4p_sdma0_mmio.mimi_syshub_base) >> 12)
        | (dest->field_0.asU64[5] & 0xffffffffff700000ULL);

    syshub_init_sdma();

    *(volatile a53_u32 *)0x03290038ULL = 0xfdec6005U;
    *(volatile a53_u32 *)0x032900a0ULL = 0xfde6000cU;
    *(volatile a53_u32 *)0x032900a8ULL = 0xfde60010U;
    *(volatile a53_u64 *)0x032902a0ULL = pm->mm4p_iommu_command_buffer_pa;
    *(volatile a53_u32 *)0x032902a8ULL =
        (a53_u32)(pm->mm4p_iommu_command_buffer_size >> 4);
    *(volatile a53_u64 *)0x032902b8ULL = pm->mm4p_sdma0_rb.mimi_pa;
    *(volatile a53_u32 *)0x032902c0ULL =
        (a53_u32)pm->mm4p_sdma0_rb.mimi_physical_size;

    *out = (a53_u8 *)dest;
    return 0;
}

int A53_SECTION(".text.el3.loader") boot_config_io_phase2(a53_u8 **out)
{
    IoController_BootConfiguration *dst;
    main_mp4_param_t *pm;

    dst = IoController_BootConfiguration_get();
    pm = msi_get_main_param();
    *(volatile a53_u64 *)0x032902b8ULL = pm->mm4p_sdma0_rb.mimi_pa;
    *(volatile a53_u32 *)0x032902c0ULL =
        (a53_u32)pm->mm4p_sdma0_rb.mimi_physical_size;

    if (cp_param_check(0x10000) == 0) {
        a53_u32 uVar4;
        a53_u32 uVar5;
        a53_u64 base;
        a53_u64 lVar9;
        a53_u16 *puVar1;
        a53_u32 *puVar10;
        a53_u32 uVar7;
        a53_u32 uVar8;

        printf_low("%d:%s:waiting DVM_MAILBOX_IO\n",
                   (a53_u64)mp4_get_cpu(), "boot_config_io_phase2");
        do {
            uVar4 = dvm_read_mailbox(0x2e);
            uVar5 = dvm_read_mailbox(0x2f);
        } while ((uVar5 | uVar4) == 0);

        printf_low("%d:%s:dvm_mailbox_l    0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "boot_config_io_phase2",
                   (a53_u64)uVar4);
        printf_low("%d:%s:dvm_mailbox_h    0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "boot_config_io_phase2",
                   (a53_u64)uVar5);

        base = (a53_u64)(uVar4 & 0xfc000000U) | ((a53_u64)uVar5 << 32);
        syshub_init_for_io(base);
        lVar9 = ((a53_u64)uVar4 | ((a53_u64)uVar5 << 32)) - base;
        puVar1 = (a53_u16 *)(lVar9 + 0x54000000ULL);

        printf_low("%d:%s:syshub_base    0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "boot_config_io_phase2", base);
        printf_low("%d:%s:offset         0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "boot_config_io_phase2", lVar9);
        printf_low("%d:%s:src            0x%016lx\n",
                   (a53_u64)mp4_get_cpu(), "boot_config_io_phase2",
                   (a53_u64)puVar1);
        printf_low("%d:%s:(dsrt %p, src %p, newbase 0x%016lx)\n",
                   (a53_u64)mp4_get_cpu(), "boot_config_header_init",
                   dst, puVar1, 0x54000000ULL);
        printf_low("%d:%s:version 0x%04x\n",
                   (a53_u64)mp4_get_cpu(), "boot_config_header_init",
                   (a53_u64)*puVar1);
        printf_low("%d:%s:num     0x%04x\n",
                   (a53_u64)mp4_get_cpu(), "boot_config_header_init",
                   (a53_u64)*(a53_u16 *)(lVar9 + 0x54000002ULL));
        printf_low("%d:%s:size    0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "boot_config_header_init",
                   (a53_u64)*(a53_u64 *)(lVar9 + 0x54000008ULL));

        puVar10 = (a53_u32 *)(lVar9 + 0x54000040ULL);
        for (uVar7 = 0;
             uVar7 < *(a53_u16 *)(lVar9 + 0x54000002ULL);
             ++uVar7) {
            uVar8 = *puVar10 - 0x21U;
            if (uVar8 < 0x31U) {
                if ((1ULL << uVar8 & 0x3f0001001fULL) == 0) {
                    if (uVar8 == 0x30U) {
                        a53_u32 *puVar11;
                        a53_u32 uVar8_inner;

                        printf_low("%d:%s:(dst %p, src %p, newbase 0x%016lx)\n",
                                   (a53_u64)mp4_get_cpu(), "boot_config_dram",
                                   dst, puVar10, 0x54000000ULL);
                        printf_low("%d:%s:type     0x%08x\n",
                                   (a53_u64)mp4_get_cpu(), "boot_config_dram",
                                   (a53_u64)*puVar10);
                        printf_low("%d:%s:size     0x%08x\n",
                                   (a53_u64)mp4_get_cpu(), "boot_config_dram",
                                   (a53_u64)puVar10[1]);
                        printf_low("%d:%s:num      0x%08x\n",
                                   (a53_u64)mp4_get_cpu(), "boot_config_dram",
                                   (a53_u64)puVar10[2]);

                        puVar11 = puVar10 + 4;
                        for (uVar8_inner = 0;
                             uVar8_inner < puVar10[2];
                             ++uVar8_inner) {
                            printf_low("%d:%s:type     0x%08x\n",
                                       (a53_u64)mp4_get_cpu(),
                                       "boot_config_dram",
                                       (a53_u64)*puVar11);
                            printf_low("%d:%s:size     0x%08x\n",
                                       (a53_u64)mp4_get_cpu(),
                                       "boot_config_dram",
                                       (a53_u64)puVar11[1]);
                            printf_low("%d:%s:addr     0x%016lx\n",
                                       (a53_u64)mp4_get_cpu(),
                                       "boot_config_dram",
                                       *(a53_u64 *)(puVar11 + 2));
                            printf_low("%d:%s:attr     0x%08x\n",
                                       (a53_u64)mp4_get_cpu(),
                                       "boot_config_dram",
                                       (a53_u64)puVar11[4]);
                            printf_low("%d:%s:pasid    0x%04x\n",
                                       (a53_u64)mp4_get_cpu(),
                                       "boot_config_dram",
                                       (a53_u64)*(a53_u16 *)(puVar11 + 5));
                            boot_config_dram_entry(dst,
                                                    *(a53_u64 *)(puVar11 + 2),
                                                    0x54000000ULL);
                            puVar11 += 6;
                        }
                    }
                }
            } else {
                if (*puVar10 - 0x11U > 3) {
                    printf_low("%d:%s:Unknown type 0x%08x\n",
                               (a53_u64)mp4_get_cpu(),
                               "boot_config_header_init",
                               (a53_u64)*puVar10);
                }
            }
            puVar10 = (a53_u32 *)((a53_u64)puVar10 + (a53_u64)puVar10[1]);
        }
    } else {
        printf_low("%d:%s:SKIP!!! Waiting DVM_MAILBOX_IO\n",
                   (a53_u64)mp4_get_cpu(), "boot_config_io_phase2");
    }
    *out = (a53_u8 *)dst;
    return 0;
}
