#include "a53_abi.h"

int A53_SECTION(".text.dev.loader") toupper(int c)
{
    if ((unsigned)c < 0x80u && (unsigned)(c - 'a') <= (unsigned)('z' - 'a')) {
        c -= 0x20;
    }
    return c;
}

int A53_SECTION(".text.dev.loader") tolower(int c)
{
    if ((unsigned)c < 0x80u && (unsigned)(c - 'A') <= (unsigned)('Z' - 'A')) {
        c += 0x20;
    }
    return c;
}
