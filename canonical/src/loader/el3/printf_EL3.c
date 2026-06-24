#include <stdarg.h>

#include "a53_abi.h"
#include "a53_context.h"

extern int prnt(int (*pf)(void *, int), void *pfd, char *fmt0, va_list *argp);

int A53_SECTION(".text.el3.loader") putchar_low_hook(void *pfd, int ch)
{
    (void)pfd;
    if ((ch | 1U) == 0x201) {
        return 0;
    }
    putchar_low(ch);
    return 0;
}

int A53_SECTION(".text.el3.loader") printf_low(char *format, ...)
{
    va_list args;
    int ret;

    va_start(args, format);
    ret = prnt(putchar_low_hook, (void *)0, format, &args);
    va_end(args);
    return ret;
}

int A53_SECTION(".text.el3.loader") putchar_cp_hook(void *pfd, int ch)
{
    (void)pfd;
    if ((ch | 1U) == 0x201) {
        return 0;
    }
    putchar_cp(ch);
    return 0;
}

int A53_SECTION(".text.el3.loader") printf_cp(char *format, ...)
{
    va_list args;
    int ret;

    va_start(args, format);
    ret = prnt(putchar_cp_hook, (void *)0, format, &args);
    va_end(args);
    return ret;
}
