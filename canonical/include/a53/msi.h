#ifndef A53_MSI_H
#define A53_MSI_H

#include "a53_abi.h"
#include "mmio.h"  /* mp4_iommu_map_info_t */

/* ---- Main MP4 parameter block at 0x88000c00 ---- */
typedef struct {
    a53_u32 mm4p_self_size;
    a53_u32 mm4p_magic;
    a53_u32 mm4p_flags;
    a53_u32 mm4p_reserved;
    a53_u64 mm4p_cyclecount;
    a53_u64 mm4p_msi_address;
    a53_u32 mm4p_msi_vector_loader0;
    a53_u32 mm4p_pad1;
    a53_u32 mm4p_msi_io;
    a53_u32 mm4p_pad2;
    a53_u32 mm4p_msi_mm;
    a53_u32 mm4p_pad3;
    a53_u32 mm4p_msi_vector_loader1;
    a53_u32 mm4p_pad4;
    a53_u64 mm4p_pasid_kernel;
    a53_u64 mm4p_mm_rings_ioma;
    a53_u64 mm4p_mapper_page_table_pa;
    a53_u64 mm4p_mapper_page_table_ioma;
    a53_u64 mm4p_iommu_command_buffer_pa;
    a53_u64 mm4p_iommu_command_buffer_size;
    a53_u8 _pad[0x78];
    mp4_iommu_map_info_t mm4p_iommu_mmio;
    mp4_iommu_map_info_t mm4p_sdma0_mmio;
    mp4_iommu_map_info_t mm4p_sdma1_mmio;
    mp4_iommu_map_info_t mm4p_sdma0_rb;
    mp4_iommu_map_info_t mm4p_sdma1_rb;
    mp4_iommu_map_info_t mm4p_g6_fix;
    mp4_iommu_map_info_t mm4p_mm_param;
    mp4_iommu_map_info_t mm4p_io_param;
    a53_u64 mm4p_mapper_private_ioma;
    a53_u64 mm4p_scf_buf_iommu_addr;
    a53_u64 mm4p_machine_part_number;
} main_mp4_param_t;

/* ---- MSI functions ---- */
void msi_write_scratch0(a53_u32 v);
void msi_write_scratch1(a53_u32 v);
void msi_write_scratch2(a53_u32 v);
void msi_write_scratch3(a53_u32 v);
void msi_write_p2c_command(a53_u32 core, a53_u32 v);
a53_u32 msi_read_c2p_command(a53_u32 core);
a53_u32 msi_read_c2p_arg1(a53_u32 core);
void msi_write_c2p_ack(a53_u32 core, a53_u32 command);
int msi_send_command_nosync(a53_u32 core, a53_u32 command0);
int msi_send_command_sync(a53_u32 core, a53_u32 command);
a53_u32 msi_get_prev(a53_u32 core);
a53_u32 msi_read_p2c_ack(a53_u32 core);
int msi_send_write_sig(a53_u32 core, a53_u32 arg1, a53_u32 arg2, a53_u32 arg3, a53_u32 hint0);
int msi_has_internal_qaf(void);
a53_u32 msi_get_msi_address32(void);
a53_u32 msi_get_msi_offset(void);
a53_u16 msi_get_vector_for_mm(void);
a53_u16 msi_get_vector_for_io(void);
a53_u16 msi_get_vector_1st_core(void);
a53_u16 msi_get_vector_2nd_core(void);
main_mp4_param_t *msi_get_main_param(void);
int msi_get_info_by_1st_core(void);
int msi_get_info_by_2nd_core(void);
int msi_wait_command(a53_u32 cpu);
int msi_test(void);

/* ---- C2P message queue ---- */
a53_u64 c2pmsg_addr64(a53_u32 ui);
a53_u32 c2pmsg_read(a53_u32 no);
void c2pmsg_write(a53_u32 no, a53_u32 data);
void c2pmsg_init(void);

#endif
