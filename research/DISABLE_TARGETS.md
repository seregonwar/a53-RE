# A53-RE: Disable Targets — System Registers & MMIO

## Primary Targets (Chip Disable)

These targets, when modified, can completely halt or brick the A53 core.

### 1. SCTLR_EL3 (System Control Register, EL3)

| Bit | Name | Effect of clearing |
|-----|------|--------------------|
| 0 | M | **Disable MMU** — all memory accesses become physical addresses. Page table walks stop. Execution continues using physical addresses, likely hitting unmapped/invalid memory. Immediate crash. |
| 1 | A | Disable alignment check |
| 2 | C | Disable data cache |
| 3 | SA | Disable stack alignment check |
| 12 | I | Disable instruction cache |
| 19 | WXN | Write implies execute-never |
| 25 | EE | Little-endian (already 0 on A53) |

**Disable via**: `msr sctlr_el3, xzr` (write all-zero to SCTLR)
**Effect**: MMU disabled, caches disabled, alignment checks off. The next instruction
fetch will use physical addressing. If the code was executing from a virtual address
that differs from its physical address, the CPU will fetch from the wrong location
and crash immediately.

### 2. SCR_EL3 (Secure Configuration Register)

| Bit | Name | Effect |
|-----|------|--------|
| 0 | NS | Non-secure bit — clears secure state |
| 1 | IRQ | Route IRQ to EL3 |
| 2 | FIQ | Route FIQ to EL3 |
| 3 | EA | Route external aborts to EL3 |
| 4 | FIQ | FIQ handler mode |
| 7 | SMD | Secure monitor disable |
| 10 | RW | Lower EL execution state (0=AArch32) |

**Disable via**: `msr scr_el3, xzr`
**Effect**: Sets NS=0 (secure state forced), clears all routing bits potentially causing
unexpected exception behavior. SMD=0 disables SMC trapping.

### 3. VBAR_EL3 (Vector Base Address Register, EL3)

**Disable via**: `msr vbar_el3, x0` with x0 pointing to attacker-controlled memory
**Effect**: All exceptions (IRQ, FIQ, SError, SVC, SMC) redirect to attacker code.
The GIC timer interrupt (~1ms period) guarantees execution within milliseconds.
Can also point VBAR_EL3 to invalid/unmapped address to cause double-fault crash.

### 4. GICD_CTLR (Distributor Control Register)

**Address**: CBAR_EL1 value + 0x1000
**Disable via**: `str wzr, [x0]` where x0 = GICD address
**Effect**: **Disables ALL interrupt distribution**. No IRQ, FIQ, or virtual interrupts
will reach any CPU. The chip becomes deaf to all hardware events.

### 5. GICC_CTLR (CPU Interface Control Register)

**Address**: CBAR_EL1 value + 0x2000
**Disable via**: `str wzr, [x0]` where x0 = GICC address
**Effect**: Disables CPU interface for interrupts. Combined with GICD_CTLR disable,
completely severs all interrupt handling.

---

## Secondary Targets (Degradation)

### 6. TTBR0_EL3 (Translation Table Base Register 0, EL3)

**Disable via**: `msr ttbr0_el3, xzr`
**Effect**: Sets page table base to 0x0. All page table walks start from physical
address 0, which typically contains the reset vector (not a valid translation table).
Any subsequent TLB miss causes a translation fault → crash.

### 7. TCR_EL3 (Translation Control Register, EL3)

**Disable via**: `msr tcr_el3, xzr`
**Effect**: Sets translation granule to invalid, disables all translation table walks.
Next memory access with MMU enabled causes translation fault.

### 8. TCR_EL1 (Translation Control Register, EL1)

**Disable via**: `msr tcr_el1, xzr`
**Effect**: Same as TCR_EL3 but for EL1/EL0 translations. Can selectively crash
lower exception levels while EL3 continues executing.

### 9. CPUECTLR_EL1 (CPU Extended Control Register)

**Disable via**: `msr s3_1_c15_c2_1, xzr`
**Effect**: Disables CPU-specific features (prefetch, branch prediction, etc.).
Chip continues running but with severe performance degradation.

### 10. PMCR_EL0 (Performance Monitors Control Register)

**Disable via**: `msr pmcr_el0, xzr`
**Effect**: Disables cycle counter and all event counters. Low impact, primarily
anti-forensics.

---

## MMIO Targets (Peripheral Disable)

### 11. SysHub IOMMU TLB Entries

**Address**: 0x03230000 + tlb_index * 0x10
**Disable via**: `str wzr, [addr]` for TLB entries 0-63
**Effect**: Corrupts IOMMU translation. DMA from peripherals (SDMA, NVMe, etc.)
fails or targets wrong memory. Can cause system-wide data corruption.

### 12. MSI P2C Command Register

**Address**: 0x03010500 (core 0), 0x030f1000 (core 1)
**Disable via**: Write garbage values
**Effect**: Breaks inter-processor communication. The coprocessor may hang waiting
for commands that never arrive.

### 13. Debug Status Magic Fields

**Address**: 0xEC000000 (mds_magic1), 0xEC000218 (mds_magic2), etc.
**Disable via**: Corrupt magic values
**Effect**: Debug infrastructure may fail validation checks, causing assertion failures
or infinite loops in `el3_assert`.

---

## Disable Chain: Complete Brick

Sequential execution of these operations at EL3 guarantees unrecoverable chip state:

```
1. str wzr, [gicd_addr]        // Disable all interrupts first
2. str wzr, [gicc_addr]        // Disable CPU interface
3. msr scr_el3, xzr             // Clear security state
4. msr sctlr_el3, xzr           // Disable MMU + caches
5. dsb sy; isb                  // Synchronization barriers
6. b .                          // Infinite loop at physical address
```

After step 4, the CPU is running without MMU at a physical address that may not
match the virtual address of the infinite loop. This causes:
- If physical == virtual: infinite loop (chip hung)
- If physical != virtual: fetch from wrong address → crash → unrecoverable
