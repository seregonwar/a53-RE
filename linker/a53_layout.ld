/*
 * A53 memory layout recovered from a53.elf program and section headers.
 *
 * This is an allocation contract for the canonical rebuild, not yet a complete
 * link command: C/C++ sources must first be promoted from recovered/ and place
 * their code/data in the matching output section. `AT()` values are the
 * original physical/load addresses; VMA, flags and 64 KiB segment alignment
 * are evidence recovered directly from the reference ELF.
 */

OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(_start)

PHDRS
{
    reset             PT_LOAD FLAGS(5); /* R E */
    mm_sram           PT_LOAD FLAGS(6); /* R W */
    io_sram           PT_LOAD FLAGS(6); /* R W */
    el3_text          PT_LOAD FLAGS(5); /* R E */
    el3_data          PT_LOAD FLAGS(6); /* R W */
    el2               PT_LOAD FLAGS(7); /* R W E */
    el1_and_tables    PT_LOAD FLAGS(7); /* R W E */
    dev               PT_LOAD FLAGS(5); /* R E */
    controller        PT_LOAD FLAGS(5); /* R E */
    controller_bss    PT_LOAD FLAGS(6); /* R W */
}

SECTIONS
{
    .reset.text 0x00000000 : AT(0x00100000)
    {
        KEEP(*(.text.a53.loader_reset_vector.S))
        *(.reset.text)
    } : reset
    .stack.core0 0x00003000 (NOLOAD) : { . = . + 0x1000; } : reset
    .stack.core1 0x00004000 (NOLOAD) : { . = . + 0x1000; } : reset

    .text.mm 0x04c00000 : AT(0x00000000) { *(.text.mm) } : mm_sram
    .data.mm 0x04c06000 (NOLOAD) : { *(.data.mm) *(.bss.mm) } : mm_sram

    .text.io 0x04e0c000 : AT(0x00069000) { *(.text.io) } : io_sram
    .data.io 0x04e77000 (NOLOAD) : { *(.data.io) *(.bss.io) } : io_sram

    .text.el3.loader 0x00100000 : AT(0x80000000)
    {
        KEEP(*(.text.a53.loader_el3_el3_vector.S))
        KEEP(*(.text.a53.loader_el3_aarch64_el3_asm.S))
        KEEP(*(.text.a53.loader_el3_setjmp_asm.S))
        *(.text.el3.loader)
    } : el3_text
    .data.el3.loader 0x00115000 : AT(0x80015000)
    {
        *(.data.el3.loader) *(.bss.el3.loader)
    } : el3_data

    .text.dev.loader 0x00117000 : AT(0x80017000)
    {
        KEEP(*(.text.a53.loader_dev_aarch64_dev_asm.S))
        KEEP(*(.text.a53.loader_dev_svc_asm.S))
        *(.text.dev.loader)
    } : dev
    .data.dev.loader 0x00124000 (NOLOAD) : { *(.data.dev.loader) *(.bss.dev.loader) } : dev

    .el2.loader 0x00124000 : AT(0x80024000)
    {
        KEEP(*(.text.a53.loader_el2_el2_vector.S))
        *(.el2.loader)
    } : el2
    .el1.loader 0x00125000 : AT(0x80025000)
    {
        KEEP(*(.text.a53.loader_el1_el1_vector.S))
        *(.el1.loader)
    } : el1_and_tables
    .stack.el3.g6.core0 0x00126000 (NOLOAD) : { . = . + 0x2000; } : el1_and_tables
    .stack.el3.g6.core1 0x00128000 (NOLOAD) : { . = . + 0x2000; } : el1_and_tables
    .bss.page_table.core0 0x0012a000 (NOLOAD) : { . = . + 0x18000; } : el1_and_tables
    .bss.page_table.core1 0x00142000 (NOLOAD) : { . = . + 0x20000; } : el1_and_tables

    .text.controller.dev 0x06000000 : AT(0x80200000)
    {
        KEEP(*(.text.a53.controller_test_asm.S))
        *(.text.controller.dev)
    } : controller
    .data.controller.dev 0x06200000 (NOLOAD) :
    {
        *(.data.controller.dev) *(.bss.controller.dev)
        . = . + 0x508; /* original .data.controller.dev SHT_NOBITS size */
    } : controller_bss
}
