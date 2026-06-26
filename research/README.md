# A53-RE: ROP/JOP Chain Research — A53 Chip Disable

## Executive Summary

This research analyzes the A53 firmware/bootloader (recovered from `a53.elf`) to identify
attack surfaces, catalog ROP/JOP gadgets, and construct exploitation chains capable of
**completely disabling the A53 chip** from within EL3 privileged execution context.

### Target Architecture

- **Chip**: Custom A53-based SoC (likely AMD/PlayStation APU derivative)
- **Exception Levels**: EL3 (secure monitor), EL2 (hypervisor), EL1 (kernel), EL0 (user)
- **Memory**: SRAM at 0x00000000, DRAM at 0x88000000+, MMIO regions
- **Peripherals**: GICv2, SysHub IOMMU, MSI inter-processor messaging, Pericom UART
- **VBAR_EL3**: 0x00107000 (runtime), initially at 0x00000000 (reset vector)

### Key Findings

| Category | Count | Details |
|----------|-------|---------|
| Attack surfaces | 7 | SVC/SMC dispatch, MSI rings, Debug Status, GICD, SysHub, DebugPutChar |
| EL3-RW MMIO regions | 12+ | 0x03010000 (MSI), 0x03230000 (SysHub), 0xEC000000 (Debug) |
| ROP gadgets (direct) | 45+ | stack pivot, MRS/MSR, store/load pairs, eret, indirect branches |
| JOP dispatch sites | 8 | Virtual calls via function pointers in dev_context_t, deci_target_md_t |
| System registers writable | 15+ | SCTLR_EL1/2/3, SCR_EL3, VBAR_EL3, TCR_ELx, TTBR0_ELx, PMCR_EL0 |

### Recommended Attack Chain

1. **Gain code execution** at EL3 (assumed prerequisite — e.g., via SVC handler bug, SMC bypass, or DMA)
2. **Stack pivot** to attacker-controlled buffer (e.g., Debug Status GPR array at 0xEC000020)
3. **ROP chain**: MSR SCTLR_EL3 → MSR SCR_EL3 → disable GICD_CTLR via STR → infinite loop
4. **Result**: Chip enters unrecoverable state — MMU disabled, interrupts off, security state broken

### File Index

- [`ATTACK_SURFACE.md`](ATTACK_SURFACE.md) — Detailed attack surface analysis
- [`GADGET_CATALOG.md`](GADGET_CATALOG.md) — Complete ROP/JOP gadget catalog
- [`DISABLE_TARGETS.md`](DISABLE_TARGETS.md) — System registers and MMIO to disable
- [`CHAIN_STRATEGY.md`](CHAIN_STRATEGY.md) — Exploitation strategy and chain construction
- [`poc/`](poc/) — Proof-of-concept payloads (Python + assembly)

### Compiler & Toolchain

- **Compiler**: clang 6.0.0 AArch64 (verified byte-exact for 103/371 functions)
- **Build**: `make -f canonical/Makefile`
- **Verification**: `make -f canonical/Makefile check-canonical`
