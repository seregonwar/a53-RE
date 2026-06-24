#include "a53_abi.h"

#define TITANIA_UART_EL3_BASE ((volatile a53_u32 *)0xa1010100UL)

int A53_SECTION(".text.dev.loader") putchar_titania_uart_el3(int c)
{
    volatile a53_u32 *uart = TITANIA_UART_EL3_BASE;

    /* Wait for TX ready: bit 11 of status register (offset 0xc). */
    while ((uart[3] >> 11 & 1) == 0) {
    }
    uart[1] = (a53_u32)(unsigned char)c;
    return c;
}
