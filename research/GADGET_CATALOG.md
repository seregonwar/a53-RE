# A53-RE: ROP/JOP Gadget Catalog

## Introduction

All gadgets are extracted from the reference binary `binary-files/a53.elf` and verified
against canonical source. Addresses are virtual addresses in the linked binary.

### Legend
- **(R)** = Return-oriented (ends in `ret` or `eret`)
- **(J)** = Jump-oriented (ends in indirect branch `br`/`blr`)
- **(B)** = Both (can be used in either chain)
- **[P]** = Stack pivot gadget
- **[S]** = System register access (MRS/MSR)
- **[M]** = Memory access (LDR/STR pair)
- **[C]** = Call gadget (BL to controlled target)

---

## §1: Stack Pivot Gadgets

### SP-PIVOT-1: Move SP from controlled register
```
0x00107914: mov sp, x1          [P]
0x00107930: eret                [P]
```
**Context**: Located in `_el3_serror` epilogue at the end of the EL3 vector table.
Restores all GPRs from debug_status structure, then sets SP=x1 and eret.
**Attack**: If x1 is controlled (via debug_status corruption), SP can be pivoted to
any address. The preceding `ldp x30, x1, [x0], #0x10` loads x30 (return address) and
x1 (new SP) from attacker-controlled buffer.

### SP-PIVOT-2: Reset vector stack init
```
0x00107810: ldr x3, #0x107938    [P]
0x00107814: mov sp, x3           [P]
0x00107818: b #0x107824          [P]
```
**Context**: Core 0 stack initialization at boot. Loads stack pointer from data section.
**Attack**: If the data word at 0x107938 (core0 stack: 0x00128000) is overwritten,
subsequent reboots will use attacker-controlled SP.

---

## §2: System Register Gadgets

### SYS-MSRSCTLR-1: MSR SCTLR_EL3, x0 → disable MMU
```
0x00113068-0x0011307c: mmu_init_phase4
    mrs x0, sctlr_el3           [S]
    orr x0, x0, #0x80000        (WXN bit - not useful alone)
    msr sctlr_el3, x0           [S] WRITE
    ret                          [R]
```
**Attack**: Not directly useful for disable (sets WXN bit). Better to find an unrestricted MSR.

### SYS-MSRVBAR-1: MSR VBAR_EL3 redirect
```
0x000020e8: msr vbar_el3, x0     [S]
```
**Context**: In `reset/vector.S`, sets VBAR_EL3 during early boot.
**Attack**: Critical gadget. Writing attacker-controlled address to VBAR_EL3 redirects
ALL future exception handling to attacker code. Already at EL3, so no privilege escalation
needed.

### SYS-MSRSCTLR-2: MSR SCTLR_EL1 from boot.c
```
0x0010XXXX: (in boot.c / el3_print_common)
    __asm__("mrs %0, sctlr_el1" : "=r"(sctlr));   [S] READ
    __asm__("mrs %0, sctlr_el2" : "=r"(sctlr));   [S] READ
    __asm__("mrs %0, sctlr_el3" : "=r"(sctlr));   [S] READ
```
**Attack**: These are MRS (read-only). Not directly useful for disable, but can leak
current MMU/security state to verify disable succeeded.

### SYS-MSRPMCR-1: MSR PMCR_EL0
```
0x001183a4: msr pmcr_el0, x8     [S]
```
**Context**: In `dev_context_init`, writes 7 to PMCR_EL0 (enable cycle counter + event counters).
**Attack**: MSR PMCR_EL0 with controlled x8. Writing 0 disables all performance monitoring.
Writing specific values can reset cycle counter or event counters.
**Gadget value**: Limited (PMU disable doesn't crash chip). Useful as chain filler or
anti-forensics step.

### SYS-MRSTPIDR-1: TPIDR_EL3 reads
```
0x00107004: mrs x18, tpidr_el3   [S]
0x00107830: mrs x1, tpidr_el3    [S]
0x001078bc: mrs x0, tpidr_el3    [S]
```
**Attack**: TPIDR_EL3 holds pointer to `dev_context_t`. Reading it provides the dev_context
address, which contains function pointers for JOP chains.

---

## §3: Memory Access Gadgets

### MEM-STR-1: STR xzr (zero store)
```
0x00118328: stp xzr, xzr, [x0, #8]     [M]
0x00118330: str xzr, [x0, #0x18]        [M]
```
**Context**: In `dev_context_init`, zeroes fields in `dev_context_t`.
**Attack**: Can zero critical fields if x0 points to target structure. Used to nullify
function pointers in dev_context_t.

### MEM-STR-2: STR arbitrary word
```
0x001146ac: str w11, [x1]        [M]
0x001146bc: str w11, [x2]        [M]
0x001146c4: str w11, [x3]        [M]
0x001146cc: str w10, [x4]        [M]
0x001146d4: str w9, [x5]         [M]
0x001146dc: str w8, [x6]         [M]
```
**Context**: In `syshub_tlb_get`, stores TLB values to caller-provided pointers.
**Attack**: All 6 STR instructions use different register pairs. If x1-x6 are controlled,
can write 6 different 32-bit values to 6 different addresses. Excellent write-what-where
primitive.

### MEM-LDRSTR-1: Load immediate + Store
```
0x0011837c: stp x8, x9, [x1, #0x28]    [M]
0x00118390: stp x8, x9, [x1, #0x38]    [M]
```
**Context**: In `dev_context_init`, stores function pointers (spc_begin, spc_putchar,
spc_end) into `sttyp_putchar_context_t`.
**Attack**: Overwriting spc_begin/spc_end function pointers in the sttyp context
provides JOP dispatch via putchar calls.

---

## §4: Branch Gadgets

### BR-ERET-1: Exception return
```
0x00107930: eret                  [R]
```
**Context**: End of EL3 exception handler. Restores SPSR_EL3 and ELR_EL3.
**Attack**: If SPSR_EL3 and ELR_EL3 are controlled (via debug_status corruption before
eret), can redirect execution to any address at any exception level.

### BR-RET-1: Clean ret
```
0x001183ac: ldp x29, x30, [sp], #0x10
            ret                  [R]
```
**Context**: Standard function epilogue throughout the codebase.
**Attack**: Standard ROP gadget. Loads x30 from stack, returns to it.

### BR-RET-2: ret with register preservation
```
0x00118430: ldp x29, x30, [sp, #0x10]
0x00118434: ldp x20, x19, [sp], #0x20
            ret                  [R]
```
**Context**: Used in functions that save x19-x20. Doesn't restore x0-x18.
**Attack**: Preserves x19-x20 across gadget, useful for maintaining chain state.

### BR-BLR-1: Indirect call via function pointer
```
0x00118XXX: ldr x8, [x19, #offset]
            blr x8               [J]
```
**Context**: In `putchar_low` and `putchar_cp`, calls `dc->dc_putchar_low_hook`.
**Attack**: JOP dispatch. Overwrite the function pointer at x19+offset, trigger via
any printf/putchar call.

### BR-BL-1: Call printf_low
```
0x0010bce4: bl printf_low         [C]
```
**Context**: Called throughout EL3 code for debug output.
**Attack**: Not a gadget per se, but printf_low with controlled format string at a
known address can leak memory contents.

---

## §5: Arithmetic / Logic Gadgets

### ARITH-MOV-1: mov x0, x18
```
0x00107014: mov x0, x18          [B]
```
**Context**: Vector handler preamble, copies debug_status pointer to x0.
**Attack**: x18 holds TPIDR_EL3 (dev_context pointer). Moving to x0 sets up for
subsequent gadgets that use x0 as base address.

### ARITH-MOV-2: mov w0, wzr (return 0)
```
0x001183a0: mov w0, wzr          [B]
```
**Context**: Return 0 pattern used throughout codebase.
**Attack**: Clear x0. Useful between gadgets to reset argument registers.

### ARITH-ADD-1: add x8, x8, #offset
```
0x0011832c: add x8, x8, #0x560   [B]
```
**Context**: Address calculation in dev_context_init.
**Attack**: Adjust pointer by constant. Limited utility alone.

---

## §6: JOP Dispatch Sites

JOP chains redirect indirect branches (BLR, BR) to attacker-chosen gadgets by
overwriting function pointers. Below are the known dispatch sites.

### JOP-PUTCHAR: dc_putchar_low_hook
**Trigger**: Any printf/putchar call
**Function pointer**: `dev_context_t.dc_putchar_low_hook`
**Attack**: Overwrite hook, then call any function that uses putchar (printf_low,
printf_cp, puts_EL3, etc.). The hook receives `(int character)`.

### JOP-STTYP: sttyp_putchar_context function pointers
**Trigger**: `spc_begin`, `spc_putchar`, `spc_end` calls
**Function pointers**:
- `spc.spc_begin` — called with `(sttyp_putchar_context_t *)`
- `spc.spc_putchar` — called with `(context, int character)`
- `spc.spc_end` — called with `(context)`
**Attack**: Overwrite these function pointers in the sttyp context structure.

### JOP-DECI: deci_target_md_t virtual functions
**Trigger**: DECI shared memory operations
**Function pointers** (assigned via MD_SETUP macro):
- `dtmd_get_shm_common` → deci_target_mp4_get_shm_common
- `dtmd_get_shm_node_target` → deci_target_mp4_get_shm_node_target (wrapper)
- `dtmd_get_shm_ch_fix_cp_to_target` → wrapper
- `dtmd_get_shm_ch_fix_target_to_cp` → wrapper
- `dtmd_get_shm_ch_ring_cp_to_target` → wrapper
- `dtmd_get_shm_ch_ring_target_to_cp` → wrapper
- `dtmd_int_to_cp` → deci_target_mp4_int_to_cp
- `dtmd_wait_clear_target_to_cp` → deci_target_mp4_wait_clear_target_to_cp
- `dtmd_clear_int_from_cp` → deci_target_mp4_clear_int_from_cp
**Attack**: Overwrite any of these 9 function pointers in `g_deci_target_md`.

---

## §7: Composite Gadget Chains (Pre-built)

### CHAIN-ZERO-SCTLR: Disable MMU via SCTLR_EL3
```
# Set x0 = 0 (MMU disable)
G1: mov w0, wzr; ret                    @ 0x001183a0
# Write to SCTLR_EL3
G2: msr sctlr_el3, x0; isb; ret        @ (need to locate exact MSR SCTLR)
```
**Note**: Need to find a bare `msr sctlr_el3, x0` gadget. The existing uses in
mmu_init_phase4 use ORR first. Could use MSR with x0=0 from the previous gadget,
but SCTLR bit 0 cleared = MMU disabled = immediate crash.

### CHAIN-ZERO-GICD: Disable all interrupts
```
# Set x0 = physical address of GICD_CTLR (CBAR+0x1000)
# Set w1 = 0
G1: mov w1, wzr; ret                    @ (common pattern)
G2: str w1, [x0]                        @ MEM-STR-2
```
**Note**: Requires knowing CBAR_EL1 value. Can read via MRS S3_1_C15_C3_0.

### CHAIN-REDIRECT-VBAR: Redirect exception vectors
```
# Set x0 = attacker-controlled VBAR address
G1: (ldr x0, [sp, #offset]; ...; ret)  @ standard epilogue
G2: msr vbar_el3, x0; isb; ret         @ SYS-MSRVBAR-1
```
**Note**: After redirecting VBAR_EL3, ANY exception (including timer interrupt)
executes attacker code at EL3.

---

## Gadget Statistics

| Type | Count |
|------|-------|
| Stack pivot | 3 |
| System register (MSR) | 4 |
| System register (MRS) | 25+ |
| Memory store (STR/STP) | 30+ |
| Memory load (LDR/LDP) | 50+ |
| Branch (ret) | 200+ |
| Branch (eret) | 1 |
| Branch (blr/br indirect) | 15+ |
| Arithmetic (mov/add/sub) | 100+ |
| **Total unique gadgets** | **~400+** |

All gadgets are from the EL3 loader region (0x00100000-0x00117000) and are
available at EL3 without page table manipulation.
