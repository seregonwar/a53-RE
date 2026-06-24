/*
 * A53-RE instruction-exact assembly recovery artifact.
 * Original compilation unit: src/controller/test-asm.S
 * Section virtual-address base: 0x0605cd8c
 * `.inst` is authoritative; comments are Capstone review disassembly.
 */
.arch armv8-a
.section .text.a53.controller_test_asm.S, "ax", %progbits
.p2align 2
.globl controller_test

/* original range [0x0605cd8c, 0x0605cdd4) */
.org 0x0
controller_test:
    .inst 0xaa1f03e0 /* 0x0605cd8c: mov x0, xzr */
    .inst 0xaa1f03e0 /* 0x0605cd90: mov x0, xzr */
    .inst 0xaa1f03e0 /* 0x0605cd94: mov x0, xzr */
    .inst 0xaa1f03e0 /* 0x0605cd98: mov x0, xzr */
    .inst 0xd2800004 /* 0x0605cd9c: mov x4, #0 */
    .inst 0xf9400885 /* 0x0605cda0: ldr x5, [x4, #0x10] */
    .inst 0xf9700886 /* 0x0605cda4: ldr x6, [x4, #0x6010] */
    .inst 0xd2a00404 /* 0x0605cda8: mov x4, #0x200000 */
    .inst 0xf9400885 /* 0x0605cdac: ldr x5, [x4, #0x10] */
    .inst 0xf9480886 /* 0x0605cdb0: ldr x6, [x4, #0x1010] */
    .inst 0xd4006001 /* 0x0605cdb4: svc #0x300 */
    .inst 0xd2a00404 /* 0x0605cdb8: mov x4, #0x200000 */
    .inst 0xf9400885 /* 0x0605cdbc: ldr x5, [x4, #0x10] */
    .inst 0xf9480886 /* 0x0605cdc0: ldr x6, [x4, #0x1010] */
    .inst 0xd4008001 /* 0x0605cdc4: svc #0x400 */
    .inst 0xd2a00200 /* 0x0605cdc8: mov x0, #0x100000 */
    .inst 0xd61f0000 /* 0x0605cdcc: br x0 */
    .inst 0x14000000 /* 0x0605cdd0: b #0x605cdd0 */
