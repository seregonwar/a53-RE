# A53-RE: Attack Surface Analysis

## 1. SVC Handler (`svc_EL3` @ 0x0010bbdc)

**Location**: `canonical/src/loader/el3/svc.c`

The primary entry point for supervisor calls from lower exception levels. Dispatched from
`el3_serror_handler` in `boot.c` when `(ESR_EL1 >> 26) & 0x3f == 0x15` (SVC exception class).

### Dispatch Logic
```c
int svc_EL3(a53_u32 esr_el1, mp4_debug_status_t *status) {
    if ((esr_el1 & 0xffff) == 0x152) {
        ret = write_EL3((char *)status->mds_gpr[0], status->mds_gpr[1]);
        status->mds_gpr[0] = (a53_s64)ret;
        return 0;
    }
    printf_low("%d:%s:Unsupport imm16\n", ...);
    return -1;
}
```

**Key observations**:
- Only `imm16 == 0x152` is handled (calls `write_EL3` with GPR[0] as buffer, GPR[1] as length)
- `write_EL3` notably includes `dmb sy; sev` barriers — useful as gadget precursors
- **status->mds_gpr[0]** is written with return value: potential controlled-write primitive
- All other imm16 values print error and return -1: **no crash on invalid SVC**
- SVC calls from EL0: `svc #0x101` (putchar), `svc #0x111` (puts), `svc #0x120` (sttyp_begin), `svc #0x131` (sttyp_putchar), `svc #0x141` (sttyp_end), `svc #0x151` (sttyp_write)

### Attack vector
If an attacker controls `status->mds_gpr[0]` and `status->mds_gpr[1]` (through prior corruption),
an SVC #0x152 becomes an arbitrary `write_EL3` call with controlled buffer+length.

---

## 2. SMC Handlers (Vector Tables)

**Locations**: `canonical/src/loader/el3/el3_vector.S` @ 0x00107000

### EL3 Vector Table Structure
ARMv8 exception vector table at VBAR_EL3 (0x00107000). Each entry is 0x80 bytes (32 instructions),
with the first 8-10 instructions being the actual handler, the rest `udf #0` (undefined).

| Offset | Exception Type | Handler |
|--------|---------------|---------|
| 0x000 | Synchronous (EL3, SP0) | Save context, b to common handler |
| 0x080 | IRQ (EL3, SP0) | Same pattern, vector=0x80 |
| 0x100 | FIQ (EL3, SP0) | Same pattern, vector=0x100 |
| 0x180 | SError (EL3, SP0) | Same pattern, vector=0x180 |
| 0x200 | Synchronous (EL3, SPx) | Same pattern, vector=0x200 |
| 0x280 | IRQ (EL3, SPx) | Same pattern, vector=0x280 |
| 0x300 | FIQ (EL3, SPx) | Same pattern, vector=0x300 |
| 0x380 | SError (EL3, SPx) | Same pattern, vector=0x380 |
| 0x400 | Synchronous (Lower EL, AArch64) | Extra: saves TPIDRRO_EL0, vector=0x400 |
| 0x480 | IRQ (Lower EL, AArch64) | Extra: saves TPIDRRO_EL0, vector=0x480 |
| 0x500 | FIQ (Lower EL, AArch64) | Extra: saves TPIDRRO_EL0, vector=0x500 |
| 0x580 | SError (Lower EL, AArch64) | Extra: saves TPIDRRO_EL0, vector=0x580 |
| 0x600 | Synchronous (Lower EL, AArch32) | Extra: saves TPIDRRO_EL0, vector=0x600 |
| 0x680 | IRQ (Lower EL, AArch32) | Extra: saves TPIDRRO_EL0, vector=0x680 |
| 0x700 | FIQ (Lower EL, AArch32) | Extra: saves TPIDRRO_EL0, vector=0x700 |
| 0x780 | SError (Lower EL, AArch32) | Extra: saves TPIDRRO_EL0, vector=0x780 |

### Common Handler Pattern (all 16 entries)
```asm
msr tpidr_el0, x18        ; save x18
mrs x18, tpidr_el3         ; get dev_context ptr
ldr x18, [x18]             ; dereference to get mp4_debug_status_t
str x0, [x18, #0x10]       ; save x0 -> mds_gpr[0]
str x1, [x18, #0x18]       ; save x1 -> mds_gpr[1]
mov x0, x18                ; arg0 = debug_status
mov x1, #vector_offset     ; arg1 = vector number
mrs x18, tpidr_el0         ; restore x18
b #0x10782c               ; branch to _el3_serror
```

**Key observation**: The vector handlers save x0/x1 to the debug status structure BEFORE
entering the handler. This means even a corrupted x0/x1 will be preserved in the debug
status at known offsets.

### EL1/EL2 SMC Forwarding
EL1 vectors (0x00125000) and EL2 vectors (0x00124000) all forward via SMC:
- EL1 → SMC #0x00-0x680 (16 vectors, 0x80 spacing)
- EL2 → SMC #0x2000-0x2780 (15 vectors, 0x80 spacing)

These SMC calls trap to EL3 and are handled by the EL3 vector table above.

---

## 3. MSI Ring Buffers (Inter-Processor Communication)

**Location**: `canonical/src/loader/el3/msi.c`

### MMIO Addresses
| Address | Core 0 | Core 1 | Purpose |
|---------|--------|--------|---------|
| P2C Command | 0x03010500 | 0x030f1000 | Processor-to-coprocessor command |
| C2P Command | 0x030f6000 | 0x030fb000 | Coprocessor-to-processor command (read) |
| C2P Arg1 | 0x030f7000 | 0x030fc000 | C2P argument 1 |
| P2C Ack | 0x030fa000 | 0x030ff000 | P2C acknowledgment |
| MSI Address | 0x030f8000 | — | MSI address register |
| MSI Vector | 0x030f9000 | — | MSI vector register |
| Scratch 0-3 | 0x03010050+ | — | Scratch registers |
| MSI Write | addr+0xF8000000 | — | MSI interrupt trigger |

### Attack vector
- All MMIO regions are accessible from EL3
- Writing to C2P command + MSI trigger can inject fake interrupts to the other core
- Scratch registers can be used as temporary storage for chain data
- MSI vector can be read to determine interrupt routing

---

## 4. Debug Status Structure

**Location**: 0xEC000000 (core 0), 0xEC100000 (core 1)

```c
typedef struct mp4_debug_status {
    a53_u64 mds_magic1;           // +0x000: 0xcbb3d18a1aa5daef
    a53_u64 mds_vector;           // +0x008
    a53_u64 mds_gpr[31];          // +0x010: General-purpose registers (x0-x30)
    a53_u64 mds_spsr;             // +0x108: Saved SPSR_EL3
    a53_u64 mds_esr;              // +0x110: Saved ESR_EL3
    a53_u64 mds_far;              // +0x118: Saved FAR_EL3
    a53_u64 mds_tpidrro_el0;     // +0x120
    a53_u64 mds_1st_vector;      // +0x128
    // ... more fields ...
    a53_u64 mds_ttyp_buffer_offset; // +0x210
    a53_u64 mds_ttyp_buffer_last;   // +0x218
    a53_u64 mds_mbox_t2c;           // +0x220: Mailbox to coprocessor
} mp4_debug_status_t;
```

### Attack vector
- **mds_gpr[0-30]** array at offset 0x010: saved by vector handlers on every exception
- Writing controlled values here before triggering an exception provides controlled
  x0/x1 values for the SVC handler
- **mds_mbox_t2c** at 0x220: used for debug output, writable
- The entire structure is in DRAM, physically addressed

---

## 5. GICv2 Interrupt Controller

**Location**: GICD at CBAR_EL1+0x1000, GICC at CBAR_EL1+0x2000

### Key Registers (writable from EL3)
| Register | Offset | Effect of writing 0 |
|----------|--------|---------------------|
| GICD_CTLR | GICD+0x000 | **Disable all interrupt distribution** |
| GICD_ICENABLERn | GICD+0x180+ | Disable specific interrupt enables |
| GICC_CTLR | GICC+0x000 | Disable CPU interface |
| GICC_PMR | GICC+0x004 | Set minimum priority (0 = all masked) |

### Attack vector
Writing 0 to GICD_CTLR disables ALL interrupt forwarding. Combined with disabling
the GICC, the CPU becomes deaf to all hardware interrupts.

---

## 6. SysHub IOMMU (System Hub)

**Location**: MMIO at 0x03230000 range

### Key Registers
| Address | Purpose |
|---------|---------|
| 0x03230040 | TLB entry 4 |
| 0x03230060 | TLB entry 6 |
| 0x03230090 | TLB entry 9 |
| 0x032300e0 | TLB entry 14 (0x3f) |
| 0x032302b0 | TLB entry 0xb (SDMA0) |
| 0x032302c0 | TLB entry 0xc (SDMA1) |
| 0x032302d0 | TLB entry 0xd |
| 0x032303e0+ | TLB sub-page attributes |
| 0x032304d8+ | TLB attr1 |
| 0x032305d0 | Interrupt status |

### Attack vector
Corrupting IOMMU TLB entries can cause DMA faults or redirect DMA to attacker-controlled
memory. Writing 0 to TLB entries disables address translation for critical peripherals.

---

## 7. Debug Output (putchar infrastructure)

**Location**: `dev_context_t.dc_putchar_low_hook` function pointer

The putchar hook is a **function pointer** in `dev_context_t`. It is set during boot:
- Core 0: `putchar_pericom` or `putchar_cp` (based on cp_param2 flags)
- Core 1: `putchar_cp`

### Attack vector
Overwriting `dc_putchar_low_hook` redirects all debug output to an attacker-controlled
function. Since `putchar_low` is called extensively (printf_low uses it), this provides
a reliable JOP dispatch mechanism.

---

## Summary: EL3-RW Memory Map

| Region | Address | Size | Purpose |
|--------|---------|------|---------|
| SRAM | 0x00000000 | 0x40000 | Reset vector, stacks, page tables |
| EL3 Text | 0x00100000 | 0x17000 | EL3 loader code |
| Dev Text | 0x00117000 | 0xD000 | Device driver code |
| EL2 Text | 0x00124000 | 0x1000 | EL2 vectors |
| EL1 Text | 0x00125000 | 0x1000 | EL1 vectors |
| EL3 Stack C0 | 0x00126000 | 0x2000 | Core 0 EL3 stack |
| EL3 Stack C1 | 0x00128000 | 0x2000 | Core 1 EL3 stack |
| Page Tables C0 | 0x0012A000 | 0x18000 | Core 0 page tables |
| Page Tables C1 | 0x00142000 | 0x20000 | Core 1 page tables |
| MSI MMIO | 0x03010000 | 0x1000 | Inter-processor comm |
| SysHub IOMMU | 0x03230000 | 0x1000 | IOMMU TLB |
| Controller Text | 0x06000000 | 0x6A000 | Controller code |
| DRAM (EL3) | 0x88000000 | varies | EL3 data, main param block |
| Main Param Block | 0x88000C00 | 0x248 | mm4p parameter block |
| Debug Status C0 | 0xEC000000 | 0x10000 | Core 0 debug |
| Debug Status C1 | 0xEC100000 | 0x10000 | Core 1 debug |
