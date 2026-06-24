/*
 * A53-RE instruction-exact assembly recovery artifact.
 * Original compilation unit: src/loader/el3/setjmp_asm.S
 * Section virtual-address base: 0x00107aac
 * `.inst` is authoritative; comments are Capstone review disassembly.
 */
.arch armv8-a
.section .text.a53.loader_el3_setjmp_asm.S, "ax", %progbits
.p2align 2
.globl el3_longjmp
.globl el3_setjmp
.globl el3_spinlock_lock
.globl el3_spinlock_unlock

/* original range [0x00107aac, 0x00107b40) */
.org 0x0
el3_setjmp:
    .inst 0x910003e8 /* 0x00107aac: mov x8, sp */
    .inst 0xf8008408 /* 0x00107ab0: str x8, [x0], #8 */
    .inst 0xd53c4008 /* 0x00107ab4: mrs x8, spsr_el2 */
    .inst 0xf8008408 /* 0x00107ab8: str x8, [x0], #8 */
    .inst 0xd53c4028 /* 0x00107abc: mrs x8, elr_el2 */
    .inst 0xf8008408 /* 0x00107ac0: str x8, [x0], #8 */
    .inst 0xa8815013 /* 0x00107ac4: stp x19, x20, [x0], #0x10 */
    .inst 0xa8815815 /* 0x00107ac8: stp x21, x22, [x0], #0x10 */
    .inst 0xa8816017 /* 0x00107acc: stp x23, x24, [x0], #0x10 */
    .inst 0xa8816819 /* 0x00107ad0: stp x25, x26, [x0], #0x10 */
    .inst 0xa881701b /* 0x00107ad4: stp x27, x28, [x0], #0x10 */
    .inst 0xa881781d /* 0x00107ad8: stp x29, x30, [x0], #0x10 */
    .inst 0xd2800000 /* 0x00107adc: mov x0, #0 */
    .inst 0xd65f03c0 /* 0x00107ae0: ret */
el3_longjmp:
    .inst 0xf8408408 /* 0x00107ae4: ldr x8, [x0], #8 */
    .inst 0x9100011f /* 0x00107ae8: mov sp, x8 */
    .inst 0xf8408408 /* 0x00107aec: ldr x8, [x0], #8 */
    .inst 0xd51c4008 /* 0x00107af0: msr spsr_el2, x8 */
    .inst 0xf8408408 /* 0x00107af4: ldr x8, [x0], #8 */
    .inst 0xd51c4028 /* 0x00107af8: msr elr_el2, x8 */
    .inst 0xa8c15013 /* 0x00107afc: ldp x19, x20, [x0], #0x10 */
    .inst 0xa8c15815 /* 0x00107b00: ldp x21, x22, [x0], #0x10 */
    .inst 0xa8c16017 /* 0x00107b04: ldp x23, x24, [x0], #0x10 */
    .inst 0xa8c16819 /* 0x00107b08: ldp x25, x26, [x0], #0x10 */
    .inst 0xa8c1701b /* 0x00107b0c: ldp x27, x28, [x0], #0x10 */
    .inst 0xa8c1781d /* 0x00107b10: ldp x29, x30, [x0], #0x10 */
    .inst 0xaa0103e0 /* 0x00107b14: mov x0, x1 */
    .inst 0xd65f03c0 /* 0x00107b18: ret */
el3_spinlock_lock:
    .inst 0xf9800010 /* 0x00107b1c: prfm pstl1keep, [x0] */
loop:
    .inst 0x885ffc04 /* 0x00107b20: ldaxr w4, [x0] */
    .inst 0x35ffffe4 /* 0x00107b24: cbnz w4, #0x107b20 */
    .inst 0x52800021 /* 0x00107b28: mov w1, #1 */
    .inst 0x88047c01 /* 0x00107b2c: stxr w4, w1, [x0] */
    .inst 0x35ffff84 /* 0x00107b30: cbnz w4, #0x107b20 */
    .inst 0xd65f03c0 /* 0x00107b34: ret */
el3_spinlock_unlock:
    .inst 0x889ffc1f /* 0x00107b38: stlr wzr, [x0] */
    .inst 0xd65f03c0 /* 0x00107b3c: ret */
