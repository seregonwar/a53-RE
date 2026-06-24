#include <stdarg.h>
#include <stddef.h>

#include "a53_abi.h"

extern int prnt(int (*pf)(void *, int), void *pfd, char *fmt0, va_list *argp);
extern void putchar_sttyp_begin(void);
extern void putchar_sttyp_end(void);
extern void putchar_sttyp(int c);

int A53_SECTION(".text.dev.loader") putchar_sttyp_hook(void *pfd, int ch)
{
    (void)pfd;
    if (ch == 0x201) {
        putchar_sttyp_end();
        return 0;
    }
    if (ch == 0x200) {
        putchar_sttyp_begin();
        return 0;
    }
    putchar_sttyp(ch);
    return 0;
}

int A53_SECTION(".text.dev.loader") printf_sttyp(char *format, ...)
{
    va_list args;
    int ret;

    va_start(args, format);
    ret = prnt(putchar_sttyp_hook, (void *)0, format, &args);
    va_end(args);
    return ret;
}

int A53_SECTION(".text.dev.loader") printf(char *format, ...)
{
    va_list args;
    int ret;

    va_start(args, format);
    ret = prnt(putchar_sttyp_hook, (void *)0, format, &args);
    va_end(args);
    return ret;
}

extern int putchar_titania_uart_el0(int c);

int A53_SECTION(".text.dev.loader") putchar_titania_uart_hook_el0(void *pfd, int ch)
{
    (void)pfd;
    if ((ch | 1U) == 0x201) {
        return 0;
    }
    putchar_titania_uart_el0(ch);
    return 0;
}

int A53_SECTION(".text.dev.loader") printf_titania_uart_el0(char *format, ...)
{
    va_list args;
    int ret;

    va_start(args, format);
    ret = prnt(putchar_titania_uart_hook_el0, (void *)0, format, &args);
    va_end(args);
    return ret;
}
