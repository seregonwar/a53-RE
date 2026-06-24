#include "a53_context.h"

extern void aarch64_DC_CVAC_range_bs(void *base, a53_u64 length);
extern int svc_sttyp_write(char *buffer, a53_u64 length);

int A53_SECTION(".text.dev.loader") spc_begin(sttyp_putchar_context_t *context)
{
    context->spc_count = 0;
    return 0;
}

int A53_SECTION(".text.dev.loader") spc_putchar(sttyp_putchar_context_t *context, int character)
{
    if (context->spc_count < context->spc_size) {
        context->spc_buf[context->spc_count] = (char)character;
        ++context->spc_count;
    }
    return character;
}

int A53_SECTION(".text.dev.loader") spc_end(sttyp_putchar_context_t *context)
{
    a53_u64 length = context->spc_count;

    if (length < context->spc_size) {
        context->spc_buf[length] = '\0';
    }
    aarch64_DC_CVAC_range_bs(context->spc_buf, length);
    svc_sttyp_write(context->spc_buf, context->spc_count);
    context->spc_count = 0;
    return (int)length;
}
