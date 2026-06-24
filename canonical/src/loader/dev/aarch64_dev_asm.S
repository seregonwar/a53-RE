/*
 * A53-RE instruction-exact assembly recovery artifact.
 * Original compilation unit: src/loader/dev/aarch64_dev_asm.S
 * Section virtual-address base: 0x0011853c
 * `.inst` is authoritative; comments are Capstone review disassembly.
 */
.arch armv8-a
.section .text.a53.loader_dev_aarch64_dev_asm.S, "ax", %progbits
.p2align 2
.globl aarch64_BRK
.globl aarch64_BRK_200
.globl aarch64_BRK_with_arg

/* original range [0x0011853c, 0x00118560) */
.org 0x0
aarch64_BRK:
    .inst 0xd4202000 /* 0x0011853c: brk #0x100 */
    .inst 0xd503201f /* 0x00118540: nop */
    .inst 0xd65f03c0 /* 0x00118544: ret */
aarch64_BRK_200:
    .inst 0xd4204000 /* 0x00118548: brk #0x200 */
    .inst 0xd503201f /* 0x0011854c: nop */
    .inst 0xd65f03c0 /* 0x00118550: ret */
aarch64_BRK_with_arg:
    .inst 0xd4202020 /* 0x00118554: brk #0x101 */
    .inst 0xd503201f /* 0x00118558: nop */
    .inst 0xd65f03c0 /* 0x0011855c: ret */
