#include "a53_abi.h"
#include "a53_context.h"

/* Global MSI state variables (defined in data section) */
extern a53_u32 g_msi_addr;
extern a53_u32 g_msi_offset;
extern a53_u32 g_msi_vector;
extern a53_u32 g_msi_p2c_count_core0;
extern a53_u32 g_msi_p2c_count_core1;
extern a53_u32 g_msi_p2c_prev_core0;
extern a53_u32 g_msi_p2c_prev_core1;
extern a53_u32 g_flags;

#define MAIN_PARAM_BASE ((volatile main_mp4_param_t *)0x88000c00UL)
#define SCRATCH(n) ((volatile a53_u32 *)(0x03010050UL + (n) * 4))

void A53_SECTION(".text.el3.loader") msi_write_scratch0(a53_u32 v) { *SCRATCH(0) = v; }
void A53_SECTION(".text.el3.loader") msi_write_scratch1(a53_u32 v) { *SCRATCH(1) = v; }
void A53_SECTION(".text.el3.loader") msi_write_scratch2(a53_u32 v) { *SCRATCH(2) = v; }
void A53_SECTION(".text.el3.loader") msi_write_scratch3(a53_u32 v) { *SCRATCH(3) = v; }

void A53_SECTION(".text.el3.loader") msi_write_p2c_command(a53_u32 core, a53_u32 v)
{
    a53_u64 addr;

    if (core == 1) {
        addr = 0x30f1000ULL;
    } else {
        if (core != 0) {
            printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                       (a53_u64)mp4_get_cpu(), "msi_write_p2c_command",
                       (a53_u64)core);
        }
        addr = 0x3010500ULL;
    }
    *(volatile a53_u32 *)addr = v;
}

a53_u32 A53_SECTION(".text.el3.loader") msi_read_c2p_command(a53_u32 core)
{
    volatile a53_u32 *ptr;

    ptr = (volatile a53_u32 *)0x030f6000UL;
    if (core == 1) {
        ptr = (volatile a53_u32 *)0x030fb000UL;
    } else if (core != 0) {
        printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                   (a53_u64)mp4_get_cpu(), "msi_read_c2p_command",
                   (a53_u64)core);
    }
    return *ptr;
}

a53_u32 A53_SECTION(".text.el3.loader") msi_read_c2p_arg1(a53_u32 core)
{
    volatile a53_u32 *ptr;

    ptr = (volatile a53_u32 *)0x030f7000UL;
    if (core == 1) {
        ptr = (volatile a53_u32 *)0x030fc000UL;
    } else if (core != 0) {
        printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                   (a53_u64)mp4_get_cpu(), "msi_read_c2p_arg1",
                   (a53_u64)core);
    }
    return *ptr;
}

void A53_SECTION(".text.el3.loader") msi_write_c2p_ack(a53_u32 core, a53_u32 command)
{
    volatile a53_u32 *ptr;

    ptr = (volatile a53_u32 *)0x030fa000UL;
    if (core == 1) {
        ptr = (volatile a53_u32 *)0x030ff000UL;
    } else if (core != 0) {
        printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                   (a53_u64)mp4_get_cpu(), "msi_write_c2p_ack",
                   (a53_u64)core);
    }
    *ptr = command;
}

int A53_SECTION(".text.el3.loader") msi_send_command_nosync(a53_u32 core,
                                                            a53_u32 command0)
{
    a53_u32 cmd;
    a53_u32 addr;
    a53_u32 vector;

    if (core == 1) {
        ++g_msi_p2c_count_core1;
        g_msi_p2c_prev_core1 = (command0 & 0xfffff000)
                               | (g_msi_p2c_count_core1 & 0xff);
        msi_write_p2c_command(1, g_msi_p2c_prev_core1);
        addr = g_msi_addr;
        vector = g_msi_vector + 3;
    } else if (core == 0) {
        ++g_msi_p2c_count_core0;
        g_msi_p2c_prev_core0 = (command0 & 0xfffff000)
                               | (g_msi_p2c_count_core0 & 0xff);
        msi_write_p2c_command(0, g_msi_p2c_prev_core0);
        addr = g_msi_addr;
        vector = g_msi_vector;
    } else {
        printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                   (a53_u64)mp4_get_cpu(), "msi_make_command",
                   (a53_u64)core);
        ++g_msi_p2c_count_core0;
        cmd = (command0 & 0xfffff000) | (g_msi_p2c_count_core0 & 0xff);
        printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                   (a53_u64)mp4_get_cpu(), "msi_set_prev",
                   (a53_u64)core);
        g_msi_p2c_prev_core0 = cmd;
        msi_write_p2c_command(core, cmd);
        printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                   (a53_u64)mp4_get_cpu(), "msi_send",
                   (a53_u64)core);
        addr = g_msi_addr;
        vector = g_msi_vector;
    }
    *(volatile a53_u32 *)((a53_u64)addr + 0xf8000000ULL) = vector;
    return 0;
}

a53_u32 A53_SECTION(".text.el3.loader") msi_get_prev(a53_u32 core)
{
    if (core != 0) {
        if (core == 1) return g_msi_p2c_prev_core1;
        printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                   (a53_u64)mp4_get_cpu(), "msi_get_prev",
                   (a53_u64)core);
    }
    return g_msi_p2c_prev_core0;
}

a53_u32 A53_SECTION(".text.el3.loader") msi_read_p2c_ack(a53_u32 core)
{
    volatile a53_u32 *ptr;

    ptr = (volatile a53_u32 *)0x030f0000UL;
    if (core == 1) {
        ptr = (volatile a53_u32 *)0x030f5000UL;
    } else if (core != 0) {
        printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                   (a53_u64)mp4_get_cpu(), "msi_read_p2c_ack",
                   (a53_u64)core);
    }
    return *ptr;
}

int A53_SECTION(".text.el3.loader") msi_send_write_sig(a53_u32 core,
                                                        a53_u32 arg1,
                                                        a53_u32 arg2,
                                                        a53_u32 arg3,
                                                        a53_u32 hint0)
{
    a53_u32 prev;

    prev = msi_get_prev(core);
    if (prev != 0) {
        a53_u32 ack;

        do {
            ack = msi_read_p2c_ack(core);
        } while (ack != prev);
    }
    if (core == 1) {
        *(volatile a53_u32 *)0x030f2000UL = arg1;
        *(volatile a53_u32 *)0x030f3000UL = arg2;
        *(volatile a53_u32 *)0x030f4000UL = arg3;
    } else {
        *(volatile a53_u32 *)0x03010504UL = arg1;
        *(volatile a53_u32 *)0x03010508UL = arg2;
        *(volatile a53_u32 *)0x0301050cUL = arg3;
        if (core != 0) {
            printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                       (a53_u64)mp4_get_cpu(), "msi_write_p2c_arg1",
                       (a53_u64)core);
            *(volatile a53_u32 *)0x03010504UL = arg1;
            printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                       (a53_u64)mp4_get_cpu(), "msi_write_p2c_arg2",
                       (a53_u64)core);
            *(volatile a53_u32 *)0x03010508UL = arg2;
            printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                       (a53_u64)mp4_get_cpu(), "msi_write_p2c_arg3",
                       (a53_u64)core);
        }
    }
    *SCRATCH(0) = hint0;
    *SCRATCH(1) = hint0;
    *SCRATCH(2) = hint0;
    *SCRATCH(3) = hint0;
    msi_send_command_nosync(core, 0x10213000);
    return 0;
}

int A53_SECTION(".text.el3.loader") msi_send_command_sync(a53_u32 core,
                                                           a53_u32 command)
{
    a53_u32 prev;

    prev = msi_get_prev(core);
    if (prev != 0) {
        a53_u32 ack;

        do {
            ack = msi_read_p2c_ack(core);
        } while (ack != prev);
    }
    msi_send_command_nosync(core, command);
    return 0;
}

int A53_SECTION(".text.el3.loader") msi_has_internal_qaf(void)
{
    return (int)(g_flags & 1);
}

a53_u32 A53_SECTION(".text.el3.loader") msi_get_msi_address32(void)
{
    return g_msi_addr;
}

a53_u32 A53_SECTION(".text.el3.loader") msi_get_msi_offset(void)
{
    return g_msi_offset;
}

a53_u16 A53_SECTION(".text.el3.loader") msi_get_vector_for_mm(void)
{
    return (a53_u16)(g_msi_vector + 2);
}

a53_u16 A53_SECTION(".text.el3.loader") msi_get_vector_for_io(void)
{
    return (a53_u16)(g_msi_vector + 1);
}

a53_u16 A53_SECTION(".text.el3.loader") msi_get_vector_1st_core(void)
{
    return (a53_u16)g_msi_vector;
}

a53_u16 A53_SECTION(".text.el3.loader") msi_get_vector_2nd_core(void)
{
    return (a53_u16)(g_msi_vector + 3);
}

main_mp4_param_t *A53_SECTION(".text.el3.loader") msi_get_main_param(void)
{
    return (main_mp4_param_t *)0x88000c00UL;
}

void A53_SECTION(".text.el3.loader")
mp4_iommu_map_info_print(mp4_iommu_map_info_t *mimi, char *member)
{
    a53_u32 cpu;

    cpu = mp4_get_cpu();
    printf_low("%d:%s:%s.mimi_pa         = 0x%016lx\n",
               (a53_u64)cpu, "mp4_iommu_map_info_print",
               member, mimi->mimi_pa);
    printf_low("%d:%s:%s.mimi_iommu_addr = 0x%016lx\n",
               (a53_u64)cpu, "mp4_iommu_map_info_print",
               member, mimi->mimi_iommu_addr);
    printf_low("%d:%s:%s.mimi_iommu_size = 0x%016lx\n",
               (a53_u64)cpu, "mp4_iommu_map_info_print",
               member, mimi->mimi_iommu_size);
}

int A53_SECTION(".text.el3.loader") msi_get_info_by_1st_core(void)
{
    volatile a53_u32 *p;
    a53_u32 cpu;

    p = (volatile a53_u32 *)0x03010500UL;
    *p = 0x10110000;
    do {
    } while ((*(volatile a53_u32 *)0x030f6000UL >> 12) != 0x20113);

    g_msi_addr = *(volatile a53_u32 *)0x030f8000UL;
    g_msi_vector = *(volatile a53_u32 *)0x030f9000UL;
    *(volatile a53_u32 *)0x030fa000UL = *(volatile a53_u32 *)0x030f6000UL;

    cpu = mp4_get_cpu();
    printf_low("%d:%s:mm4p_self_size                 = 0x%08x\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               (a53_u64)MAIN_PARAM_BASE->mm4p_self_size);
    printf_low("%d:%s:mm4p_magic                     = 0x%08x\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               (a53_u64)MAIN_PARAM_BASE->mm4p_magic);
    printf_low("%d:%s:mm4p_flags                     = 0x%08x\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               (a53_u64)MAIN_PARAM_BASE->mm4p_flags);
    printf_low("%d:%s:mm4p_reserved                  = 0x%08x\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               (a53_u64)MAIN_PARAM_BASE->mm4p_reserved);
    printf_low("%d:%s:mm4p_cyclecount                = 0x%016lx\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               MAIN_PARAM_BASE->mm4p_cyclecount);
    printf_low("%d:%s:mm4p_msi_address               = 0x%016lx\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               MAIN_PARAM_BASE->mm4p_msi_address);
    printf_low("%d:%s:mm4p_msi_vector_loader0        = 0x%08x\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               (a53_u64)MAIN_PARAM_BASE->mm4p_msi_vector_loader0);
    printf_low("%d:%s:mm4p_msi_io                    = 0x%08x\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               (a53_u64)MAIN_PARAM_BASE->mm4p_msi_io);
    printf_low("%d:%s:mm4p_msi_mm                    = 0x%08x\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               (a53_u64)MAIN_PARAM_BASE->mm4p_msi_mm);
    printf_low("%d:%s:mm4p_msi_vector_loader1        = 0x%08x\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               (a53_u64)MAIN_PARAM_BASE->mm4p_msi_vector_loader1);
    printf_low("%d:%s:mm4p_pasid_kernel              = 0x%016lx\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               MAIN_PARAM_BASE->mm4p_pasid_kernel);
    printf_low("%d:%s:mm4p_mm_rings_ioma             = 0x%016lx\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               MAIN_PARAM_BASE->mm4p_mm_rings_ioma);
    printf_low("%d:%s:mm4p_mapper_page_table_pa      = 0x%016lx\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               MAIN_PARAM_BASE->mm4p_mapper_page_table_pa);
    printf_low("%d:%s:mm4p_mapper_page_table_ioma    = 0x%016lx\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               MAIN_PARAM_BASE->mm4p_mapper_page_table_ioma);
    printf_low("%d:%s:mm4p_iommu_command_buffer_pa   = 0x%016lx\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               MAIN_PARAM_BASE->mm4p_iommu_command_buffer_pa);
    printf_low("%d:%s:mm4p_iommu_command_buffer_size = 0x%016lx\n",
               (a53_u64)cpu, "msi_get_info_by_1st_core",
               MAIN_PARAM_BASE->mm4p_iommu_command_buffer_size);

    mp4_iommu_map_info_print(
        (mp4_iommu_map_info_t *)&MAIN_PARAM_BASE->mm4p_g6_fix, "g6_fix");
    mp4_iommu_map_info_print(
        (mp4_iommu_map_info_t *)&MAIN_PARAM_BASE->mm4p_iommu_mmio, "iommu_mmio");
    mp4_iommu_map_info_print(
        (mp4_iommu_map_info_t *)&MAIN_PARAM_BASE->mm4p_sdma0_mmio, "sdma0_mmio");
    mp4_iommu_map_info_print(
        (mp4_iommu_map_info_t *)&MAIN_PARAM_BASE->mm4p_sdma1_mmio, "sdma1_mmio");
    mp4_iommu_map_info_print(
        (mp4_iommu_map_info_t *)&MAIN_PARAM_BASE->mm4p_sdma0_rb, "sdma0_rb");
    mp4_iommu_map_info_print(
        (mp4_iommu_map_info_t *)&MAIN_PARAM_BASE->mm4p_sdma1_rb, "sdma1_rb");

    if (MAIN_PARAM_BASE->mm4p_self_size != 0x248) {
        printf_low("%d:%s:Invalid mm4p_self_size = 0x%08x != 0x%08x\n",
                   (a53_u64)mp4_get_cpu(), "msi_get_info_by_1st_core",
                   (a53_u64)MAIN_PARAM_BASE->mm4p_self_size, 0x248);
        el3_assert("I:\\a53_sys\\releases_00.70\\mp4\\a53\\src\\loader\\el3\\msi.c",
                   "msi_get_info_by_1st_core", 0x278, 0, "0");
    }

    g_flags = MAIN_PARAM_BASE->mm4p_flags;
    g_msi_addr = (a53_u32)(MAIN_PARAM_BASE->mm4p_msi_address & 0xffffffff);
    g_msi_offset = g_msi_addr + 0x4000000;
    g_msi_vector = MAIN_PARAM_BASE->mm4p_msi_vector_loader0;
    return 0;
}

int A53_SECTION(".text.el3.loader") msi_get_info_by_2nd_core(void)
{
    return 0;
}

int A53_SECTION(".text.el3.loader") msi_wait_command(a53_u32 cpu)
{
    volatile a53_u32 *ack_ptr;
    a53_u32 ack;

    ack_ptr = (volatile a53_u32 *)0x030fa000UL;
    if (cpu == 1) {
        ack_ptr = (volatile a53_u32 *)0x030ff000UL;
    } else if (cpu != 0) {
        printf_low("%d:%s:Unsupport core=0x%08x Use 0\n",
                   (a53_u64)mp4_get_cpu(), "msi_read_c2p_ack",
                   (a53_u64)cpu);
    }
    ack = *ack_ptr;
    while (msi_read_c2p_command(cpu) == ack) {
    }
    return 0;
}

int A53_SECTION(".text.el3.loader") msi_test(void)
{
    a53_u32 cpu;
    volatile a53_u32 *piVar2;
    a53_u32 uVar3;

    cpu = mp4_get_cpu();
    printf_low("%d:%s:()\n", (a53_u64)cpu, "msi_test");

    piVar2 = (volatile a53_u32 *)((a53_u64)g_msi_offset + 0xf4000000ULL);
    for (uVar3 = 0; uVar3 != 10; ++uVar3) {
        cpu = mp4_get_cpu();
        printf_low("%d:%s:MSI TEST: %d: write 0x%08x + 1 to %p\n",
                   (a53_u64)cpu, "msi_test", (a53_u64)uVar3,
                   (a53_u64)g_msi_vector, (void *)piVar2);
        *piVar2 = g_msi_vector + 1;
    }
    for (uVar3 = 0; uVar3 != 10; ++uVar3) {
        cpu = mp4_get_cpu();
        printf_low("%d:%s:MSI TEST: %d: write 0x%08x + 2 to %p\n",
                   (a53_u64)cpu, "msi_test", (a53_u64)uVar3,
                   (a53_u64)g_msi_vector, (void *)piVar2);
        *piVar2 = g_msi_vector + 2;
    }
    for (uVar3 = 0; uVar3 != 10; ++uVar3) {
        cpu = mp4_get_cpu();
        printf_low("%d:%s:MSI TEST: %d: write 0x%08x + 3 to %p\n",
                   (a53_u64)cpu, "msi_test", (a53_u64)uVar3,
                   (a53_u64)g_msi_vector, (void *)piVar2);
        *piVar2 = g_msi_vector + 3;
    }
    return 0;
}
