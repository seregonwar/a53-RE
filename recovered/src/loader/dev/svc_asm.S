/*
 * A53-RE instruction-exact assembly recovery artifact.
 * Original compilation unit: src/loader/dev/svc_asm.S
 * Section virtual-address base: 0x00118560
 * `.inst` is authoritative; comments are Capstone review disassembly.
 */
.arch armv8-a
.section .text.a53.loader_dev_svc_asm.S, "ax", %progbits
.p2align 2
.globl svc_putchar
.globl svc_puts
.globl svc_sttyp_begin
.globl svc_sttyp_end
.globl svc_sttyp_putchar
.globl svc_sttyp_write

/* original range [0x00118560, 0x00118590) */
.org 0x0
svc_putchar:
    .inst 0xd4002021 /* 0x00118560: svc #0x101 */
    .inst 0xd65f03c0 /* 0x00118564: ret */
svc_puts:
    .inst 0xd4002221 /* 0x00118568: svc #0x111 */
    .inst 0xd65f03c0 /* 0x0011856c: ret */
svc_sttyp_begin:
    .inst 0xd4002401 /* 0x00118570: svc #0x120 */
    .inst 0xd65f03c0 /* 0x00118574: ret */
svc_sttyp_putchar:
    .inst 0xd4002621 /* 0x00118578: svc #0x131 */
    .inst 0xd65f03c0 /* 0x0011857c: ret */
svc_sttyp_end:
    .inst 0xd4002801 /* 0x00118580: svc #0x140 */
    .inst 0xd65f03c0 /* 0x00118584: ret */
svc_sttyp_write:
    .inst 0xd4002a41 /* 0x00118588: svc #0x152 */
    .inst 0xd65f03c0 /* 0x0011858c: ret */
