/*
 * A53-RE instruction-exact assembly recovery artifact.
 * Original compilation unit: src/loader/el3/aarch64_el3_asm.S
 * Section virtual-address base: 0x00107948
 * `.inst` is authoritative; comments are Capstone review disassembly.
 */
.arch armv8-a
.section .text.a53.loader_el3_aarch64_el3_asm.S, "ax", %progbits
.p2align 2
.globl aarch64_WFI
.globl aarch64_psci
.globl aarch64_read_CBAR_EL1
.globl aarch64_read_CLIDR_EL1
.globl aarch64_read_CONTEXTIDR_EL1
.globl aarch64_read_CPUECTLR_EL1
.globl aarch64_read_CSSELR_EL1
.globl aarch64_read_DAIF
.globl aarch64_read_ELR_EL1
.globl aarch64_read_ELR_EL2
.globl aarch64_read_ELR_EL3
.globl aarch64_read_ESR_EL1
.globl aarch64_read_ESR_EL2
.globl aarch64_read_ESR_EL3
.globl aarch64_read_ICC_IAR0_EL1
.globl aarch64_read_ICC_IAR1_EL1
.globl aarch64_read_ISR_EL1
.globl aarch64_read_MIDR_EL1
.globl aarch64_read_MPIDR_EL1
.globl aarch64_read_NZCV
.globl aarch64_read_PMCEID0_EL0
.globl aarch64_read_PMCEID1_EL0
.globl aarch64_read_SPSR_EL1
.globl aarch64_read_SPSR_EL2
.globl aarch64_read_SPSel
.globl aarch64_read_SP_EL0
.globl aarch64_read_SP_EL1
.globl aarch64_read_SP_EL2
.globl aarch64_read_TCR_EL1
.globl aarch64_read_TCR_EL2
.globl aarch64_read_TCR_EL3
.globl aarch64_read_TPIDR_EL1
.globl aarch64_read_TPIDR_EL2
.globl aarch64_read_TPIDR_EL3
.globl aarch64_read_TTBR0_EL1
.globl aarch64_read_TTBR0_EL2
.globl aarch64_read_TTBR0_EL3
.globl aarch64_read_TTBR1_EL1
.globl aarch64_read_VBAR_EL1
.globl aarch64_read_VBAR_EL2
.globl aarch64_write_DAIF
.globl aarch64_write_ICC_SGI1R_EL1
.globl jump_with_sp

/* original range [0x00107948, 0x00107aac) */
.org 0x0
aarch64_read_ICC_IAR0_EL1:
    .inst 0xd538c800 /* 0x00107948: mrs x0, icc_iar0_el1 */
    .inst 0xd65f03c0 /* 0x0010794c: ret */
aarch64_write_ICC_SGI1R_EL1:
    .inst 0xd518cba0 /* 0x00107950: msr icc_sgi1r_el1, x0 */
    .inst 0xd65f03c0 /* 0x00107954: ret */
aarch64_read_ICC_IAR1_EL1:
    .inst 0xd538cc00 /* 0x00107958: mrs x0, icc_iar1_el1 */
    .inst 0xd65f03c0 /* 0x0010795c: ret */
aarch64_read_DAIF:
    .inst 0xd53b4220 /* 0x00107960: mrs x0, daif */
    .inst 0xd65f03c0 /* 0x00107964: ret */
aarch64_write_DAIF:
    .inst 0xd51b4220 /* 0x00107968: msr daif, x0 */
    .inst 0xd65f03c0 /* 0x0010796c: ret */
aarch64_read_ELR_EL1:
    .inst 0xd5384020 /* 0x00107970: mrs x0, elr_el1 */
    .inst 0xd65f03c0 /* 0x00107974: ret */
aarch64_read_ELR_EL2:
    .inst 0xd53c4020 /* 0x00107978: mrs x0, elr_el2 */
    .inst 0xd65f03c0 /* 0x0010797c: ret */
aarch64_read_ELR_EL3:
    .inst 0xd53e4020 /* 0x00107980: mrs x0, elr_el3 */
    .inst 0xd65f03c0 /* 0x00107984: ret */
aarch64_read_NZCV:
    .inst 0xd53b4200 /* 0x00107988: mrs x0, nzcv */
    .inst 0xd65f03c0 /* 0x0010798c: ret */
aarch64_read_SP_EL0:
    .inst 0xd5384100 /* 0x00107990: mrs x0, sp_el0 */
    .inst 0xd65f03c0 /* 0x00107994: ret */
aarch64_read_SP_EL1:
    .inst 0xd53c4100 /* 0x00107998: mrs x0, sp_el1 */
    .inst 0xd65f03c0 /* 0x0010799c: ret */
aarch64_read_SP_EL2:
    .inst 0xd53e4100 /* 0x001079a0: mrs x0, sp_el2 */
    .inst 0xd65f03c0 /* 0x001079a4: ret */
aarch64_read_SPSel:
    .inst 0xd5384200 /* 0x001079a8: mrs x0, spsel */
    .inst 0xd65f03c0 /* 0x001079ac: ret */
aarch64_read_SPSR_EL1:
    .inst 0xd5384000 /* 0x001079b0: mrs x0, spsr_el1 */
    .inst 0xd65f03c0 /* 0x001079b4: ret */
aarch64_read_SPSR_EL2:
    .inst 0xd53c4000 /* 0x001079b8: mrs x0, spsr_el2 */
    .inst 0xd65f03c0 /* 0x001079bc: ret */
aarch64_WFI:
    .inst 0xd503207f /* 0x001079c0: wfi */
    .inst 0xd65f03c0 /* 0x001079c4: ret */
aarch64_read_CLIDR_EL1:
    .inst 0xd5390020 /* 0x001079c8: mrs x0, clidr_el1 */
    .inst 0xd65f03c0 /* 0x001079cc: ret */
aarch64_read_CONTEXTIDR_EL1:
    .inst 0xd538d020 /* 0x001079d0: mrs x0, contextidr_el1 */
    .inst 0xd65f03c0 /* 0x001079d4: ret */
aarch64_read_CSSELR_EL1:
    .inst 0xd53a0000 /* 0x001079d8: mrs x0, csselr_el1 */
    .inst 0xd65f03c0 /* 0x001079dc: ret */
aarch64_read_ESR_EL1:
    .inst 0xd5385200 /* 0x001079e0: mrs x0, esr_el1 */
    .inst 0xd65f03c0 /* 0x001079e4: ret */
aarch64_read_ESR_EL2:
    .inst 0xd53c5200 /* 0x001079e8: mrs x0, esr_el2 */
    .inst 0xd65f03c0 /* 0x001079ec: ret */
aarch64_read_ESR_EL3:
    .inst 0xd53e5200 /* 0x001079f0: mrs x0, esr_el3 */
    .inst 0xd65f03c0 /* 0x001079f4: ret */
aarch64_read_ISR_EL1:
    .inst 0xd538c100 /* 0x001079f8: mrs x0, isr_el1 */
    .inst 0xd65f03c0 /* 0x001079fc: ret */
aarch64_read_MIDR_EL1:
    .inst 0xd5380000 /* 0x00107a00: mrs x0, midr_el1 */
    .inst 0xd65f03c0 /* 0x00107a04: ret */
aarch64_read_MPIDR_EL1:
    .inst 0xd53800a0 /* 0x00107a08: mrs x0, mpidr_el1 */
    .inst 0xd65f03c0 /* 0x00107a0c: ret */
aarch64_read_TCR_EL1:
    .inst 0xd5382040 /* 0x00107a10: mrs x0, tcr_el1 */
    .inst 0xd65f03c0 /* 0x00107a14: ret */
aarch64_read_TCR_EL2:
    .inst 0xd53c2040 /* 0x00107a18: mrs x0, tcr_el2 */
    .inst 0xd65f03c0 /* 0x00107a1c: ret */
aarch64_read_TCR_EL3:
    .inst 0xd53e2040 /* 0x00107a20: mrs x0, tcr_el3 */
    .inst 0xd65f03c0 /* 0x00107a24: ret */
aarch64_read_TPIDR_EL1:
    .inst 0xd538d080 /* 0x00107a28: mrs x0, tpidr_el1 */
    .inst 0xd65f03c0 /* 0x00107a2c: ret */
aarch64_read_TPIDR_EL2:
    .inst 0xd53cd040 /* 0x00107a30: mrs x0, tpidr_el2 */
    .inst 0xd65f03c0 /* 0x00107a34: ret */
aarch64_read_TPIDR_EL3:
    .inst 0xd53ed040 /* 0x00107a38: mrs x0, tpidr_el3 */
    .inst 0xd65f03c0 /* 0x00107a3c: ret */
aarch64_read_TTBR0_EL1:
    .inst 0xd5382000 /* 0x00107a40: mrs x0, ttbr0_el1 */
    .inst 0xd65f03c0 /* 0x00107a44: ret */
aarch64_read_TTBR0_EL2:
    .inst 0xd53c2000 /* 0x00107a48: mrs x0, ttbr0_el2 */
    .inst 0xd65f03c0 /* 0x00107a4c: ret */
aarch64_read_TTBR0_EL3:
    .inst 0xd53e2000 /* 0x00107a50: mrs x0, ttbr0_el3 */
    .inst 0xd65f03c0 /* 0x00107a54: ret */
aarch64_read_TTBR1_EL1:
    .inst 0xd5382020 /* 0x00107a58: mrs x0, ttbr1_el1 */
    .inst 0xd65f03c0 /* 0x00107a5c: ret */
aarch64_read_VBAR_EL1:
    .inst 0xd538c000 /* 0x00107a60: mrs x0, vbar_el1 */
    .inst 0xd65f03c0 /* 0x00107a64: ret */
aarch64_read_VBAR_EL2:
    .inst 0xd53cc000 /* 0x00107a68: mrs x0, vbar_el2 */
    .inst 0xd65f03c0 /* 0x00107a6c: ret */
aarch64_read_PMCEID0_EL0:
    .inst 0xd53b9cc0 /* 0x00107a70: mrs x0, pmceid0_el0 */
    .inst 0xd65f03c0 /* 0x00107a74: ret */
aarch64_read_PMCEID1_EL0:
    .inst 0xd53b9ce0 /* 0x00107a78: mrs x0, pmceid1_el0 */
    .inst 0xd65f03c0 /* 0x00107a7c: ret */
aarch64_read_CPUECTLR_EL1:
    .inst 0xd539f220 /* 0x00107a80: mrs x0, s3_1_c15_c2_1 */
    .inst 0xd65f03c0 /* 0x00107a84: ret */
aarch64_read_CBAR_EL1:
    .inst 0xd539f300 /* 0x00107a88: mrs x0, s3_1_c15_c3_0 */
    .inst 0xd65f03c0 /* 0x00107a8c: ret */
aarch64_psci:
    .inst 0xd4000003 /* 0x00107a90: smc #0 */
    .inst 0xd65f03c0 /* 0x00107a94: ret */
jump_with_sp:
    .inst 0x9100003f /* 0x00107a98: mov sp, x1 */
    .inst 0xaa0303e1 /* 0x00107a9c: mov x1, x3 */
    .inst 0xaa0003e3 /* 0x00107aa0: mov x3, x0 */
    .inst 0xaa0203e0 /* 0x00107aa4: mov x0, x2 */
    .inst 0xd61f0060 /* 0x00107aa8: br x3 */
