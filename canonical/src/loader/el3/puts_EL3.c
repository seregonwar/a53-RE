#include <stddef.h>

#include "a53_abi.h"
#include "a53_context.h"

extern int putchar_pericom(int c);
extern int deci5s_send_sttyp(char *msg, a53_u64 len);
extern a53_u64 strnlen(const char *string, a53_u64 maxlen);

int A53_SECTION(".text.el3.loader") write_EL3(char *msg, a53_u64 len)
{
    a53_u32 cpu;
    a53_u32 mask;
    int (*hook)(int);
    a53_u64 i;

    cpu = mp4_get_cpu();
    mask = (cpu != 0) ? 0x20000 : 0x2000000;
    hook = cp_param_check(mask) ? putchar_pericom : putchar_cp;

    (*hook)('E');
    (*hook)('L');
    (*hook)('0');
    (*hook)(':');
    (*hook)(cpu + 0x30);
    (*hook)(':');
    for (i = 0; i < len; ++i) {
        (*hook)((unsigned char)msg[i]);
    }
    return (int)len;
}

int A53_SECTION(".text.el3.loader") puts_EL3(char *msg)
{
    a53_u64 len;

    len = strnlen(msg, 0x200);
    deci5s_send_sttyp(msg, len);
    return (int)len;
}
