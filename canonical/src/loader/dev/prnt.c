#include <stdarg.h>
#include <stddef.h>

#include "a53_abi.h"

#define FLAG_MINUS 0x01
#define FLAG_PLUS  0x02
#define FLAG_SPACE 0x04
#define FLAG_HASH  0x08
#define FLAG_ZERO  0x10

#define PUTC(c)       (*pf)(pfd, (c))
#define PUTC_CNT(c)   do { PUTC(c); ++count; } while (0)

static void print_str(int (*pf)(void *, int), void *pfd,
                      const char *s, int width, int precision,
                      unsigned flags, int *count)
{
    int len;
    int pad;

    for (len = 0; precision < 0 || len < precision; ++len) {
        if (s[len] == '\0') break;
    }
    pad = (width > len) ? width - len : 0;
    if ((flags & FLAG_MINUS) == 0) {
        for (; pad > 0; --pad) PUTC_CNT(' ');
    }
    for (int i = 0; i < len; ++i) PUTC_CNT(s[i]);
    for (; pad > 0; --pad) PUTC_CNT(' ');
}

static void print_pad(int (*pf)(void *, int), void *pfd, int ch,
                      int count)
{
    for (int i = 0; i < count; ++i) (*pf)(pfd, ch);
}

int A53_SECTION(".text.dev.loader") prnt(
    int (*pf)(void *, int), void *pfd,
    char *fmt0, va_list *argp)
{
    va_list args;
    int count = 0;

    if (fmt0 == NULL) {
        static const char msg[] = "fmt0 is NULL\n";
        for (size_t i = 0; i < sizeof(msg) - 1; ++i) PUTC(msg[i]);
        return 0;
    }

    va_copy(args, *argp);
    PUTC(0x200);

    for (const char *fmt = fmt0; *fmt != '\0'; ++fmt) {
        if (*fmt != '%') {
            PUTC_CNT(*fmt);
            continue;
        }

        unsigned flags = 0;
        int width = 0;
        int precision = -1;
        int length = 0; /* 0=none, 1=short, 2=long, 3=longlong */
        int negative = 0;
        a53_u64 uval;
        a53_u64 u;
        char buf[80];
        char *bp = buf + sizeof(buf);
        int base;
        const char *digits;

        /* Parse flags. */
        for (;;) {
            ++fmt;
            switch (*fmt) {
            case '-': flags |= FLAG_MINUS; continue;
            case '+': flags |= FLAG_PLUS;  continue;
            case ' ': flags |= FLAG_SPACE; continue;
            case '#': flags |= FLAG_HASH;  continue;
            case '0': flags |= FLAG_ZERO;  continue;
            }
            break;
        }

        /* Parse width. */
        if (*fmt == '*') {
            width = va_arg(args, int);
            if (width < 0) { width = -width; flags |= FLAG_MINUS; }
            ++fmt;
        } else {
            while (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + (*fmt - '0');
                ++fmt;
            }
        }

        /* Parse precision. */
        if (*fmt == '.') {
            ++fmt;
            if (*fmt == '*') {
                precision = va_arg(args, int);
                ++fmt;
            } else {
                precision = 0;
                while (*fmt >= '0' && *fmt <= '9') {
                    precision = precision * 10 + (*fmt - '0');
                    ++fmt;
                }
            }
        }

        /* Parse length. */
        for (;;) {
            if (*fmt == 'h') {
                length = (length == 1) ? -1 : 1;
                ++fmt;
            } else if (*fmt == 'l') {
                length = (length == 2) ? 3 : 2;
                ++fmt;
            } else {
                break;
            }
        }

        /* Handle the format specifier. */
        switch (*fmt) {
        case 'd':
        case 'i': {
            a53_s64 sv;
            if (length >= 2 || length == 3) {
                sv = va_arg(args, a53_s64);
            } else if (length == 1 || length == -1) {
                sv = (signed short)va_arg(args, int);
            } else {
                sv = va_arg(args, int);
            }
            if (sv < 0) { negative = 1; uval = -sv; }
            else        { uval = (a53_u64)sv; }
            base = 10;
            digits = "0123456789abcdef";
            goto format_unsigned;
        }
        case 'u': {
            if (length >= 2 || length == 3) {
                uval = va_arg(args, a53_u64);
            } else if (length == 1 || length == -1) {
                uval = (unsigned short)va_arg(args, unsigned);
            } else {
                uval = va_arg(args, unsigned);
            }
            base = 10;
            digits = "0123456789abcdef";
            goto format_unsigned;
        }
        case 'x':
            base = 16;
            digits = "0123456789abcdef";
            goto format_hex;
        case 'X':
            base = 16;
            digits = "0123456789ABCDEF";
            goto format_hex;
        case 'o':
            base = 8;
            digits = "0123456789abcdef";
            goto format_hex;
        format_hex:
            if (length >= 2 || length == 3) {
                uval = va_arg(args, a53_u64);
            } else if (length == 1 || length == -1) {
                uval = (unsigned short)va_arg(args, unsigned);
            } else {
                uval = va_arg(args, unsigned);
            }
            goto format_unsigned;
        format_unsigned: {
            (void)0;
            /* Convert to string (right-to-left in buf). */
            *--bp = '\0';
            u = uval;
            if (u == 0 && precision != 0) *--bp = '0';
            while (u != 0) {
                *--bp = digits[u % base];
                u /= base;
            }

            /* Apply precision (minimum digits). */
            {
                int ndigits = (int)((buf + sizeof(buf) - 1) - bp);
                int sign = negative || (flags & (FLAG_PLUS | FLAG_SPACE)) ? 1 : 0;
                int pad = 0;
                int total = ndigits;
                if (precision > total) total = precision;
                if (base == 16 && (flags & FLAG_HASH) && uval != 0) total += 2;
                if (base == 8 && (flags & FLAG_HASH)) total += 1;
                if (sign) total += 1;
                pad = (width > total) ? width - total : 0;

                if ((flags & FLAG_MINUS) == 0) {
                    char pc = (flags & FLAG_ZERO) ? '0' : ' ';
                    print_pad(pf, pfd, pc, pad);
                }

                /* Sign or prefix. */
                if (negative) PUTC_CNT('-');
                else if (flags & FLAG_PLUS) PUTC_CNT('+');
                else if (flags & FLAG_SPACE) PUTC_CNT(' ');

                /* 0x/0X prefix for #. */
                if (base == 16 && (flags & FLAG_HASH) && uval != 0) {
                    PUTC_CNT('0'); PUTC_CNT(digits[16] == 'A' ? 'X' : 'x');
                }
                if (base == 8 && (flags & FLAG_HASH)) {
                    PUTC_CNT('0');
                }

                /* Zero-pad to precision. */
                print_pad(pf, pfd, '0', precision - ndigits);

                /* Digits. */
                while (*bp) PUTC_CNT(*bp++);

                /* Right-pad for left-justify. */
                if (flags & FLAG_MINUS) {
                    print_pad(pf, pfd, ' ', pad);
                }
            }
            break;
        }
        case 'c': {
            char ch = (char)va_arg(args, int);
            if ((flags & FLAG_MINUS) == 0) {
                print_pad(pf, pfd, ' ', width - 1);
            }
            PUTC_CNT(ch);
            if (flags & FLAG_MINUS) {
                print_pad(pf, pfd, ' ', width - 1);
            }
            break;
        }
        case 's': {
            const char *s = va_arg(args, const char *);
            if (s == NULL) s = "(null)";
            print_str(pf, pfd, s, width, precision, flags, &count);
            break;
        }
        case 'p': {
            void *ptr = va_arg(args, void *);
            a53_u64 addr = (a53_u64)(uintptr_t)ptr;
            int ndigits = 0;
            a53_u64 tmp = addr;
            do { ++ndigits; tmp >>= 4; } while (tmp != 0);
            if (addr == 0) ndigits = 1;

            int total = ndigits + 2; /* "0x" prefix */
            int pad = (width > total) ? width - total : 0;

            if ((flags & FLAG_MINUS) == 0) {
                char pc = (flags & FLAG_ZERO) ? '0' : ' ';
                print_pad(pf, pfd, pc, pad);
            }
            PUTC_CNT('0'); PUTC_CNT('x');
            for (int i = ndigits - 1; i >= 0; --i) {
                int nib = (int)(addr >> (i * 4)) & 0xf;
                PUTC_CNT("0123456789abcdef"[nib]);
            }
            if (flags & FLAG_MINUS) {
                print_pad(pf, pfd, ' ', pad);
            }
            break;
        }
        case '%':
            PUTC_CNT('%');
            break;
        default:
            PUTC_CNT('%');
            PUTC_CNT(*fmt);
            break;
        }
    }

    PUTC(0x201);
    va_end(args);
    return count;
}
