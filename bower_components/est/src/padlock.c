/*
    padlock.c -- Header VIA padlock suport

    This implementation is based on the VIA PadLock Programming Guide:
    http://www.via.com.tw/en/downloads/whitepapers/initiatives/padlock/programming_guide.pdf

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#include "est.h"

#if ME_EST_PADLOCK

// #if ME_CPU_ARCH == ME_CPU_X86
#if defined(EST_HAVE_X86)

/*
    PadLock detection routine
 */
int padlock_supports(int feature)
{
    static int flags = -1;
    int ebx, edx;

    if (flags == -1) {
asm("movl  %%ebx, %0           \n" "movl  $0xC0000000, %%eax  \n" "cpuid                     \n" "cmpl  $0xC0000001, %%eax  \n" "movl  $0, %%edx           \n" "jb    unsupported         \n" "movl  $0xC0000001, %%eax  \n" "cpuid                     \n" "unsupported:              \n" "movl  %%edx, %1           \n" "movl  %2, %%ebx           \n":"=m"(ebx),
            "=m"
            (edx)
:           "m"(ebx)
:           "eax", "ecx", "edx");

        flags = edx;
    }

    return flags & feature;
}

/*
    PadLock AES-ECB block en(de)cryption
 */
int padlock_xcryptecb(aes_context * ctx, int mode, uchar input[16], uchar output[16])
{
    ulong   *rk, *blk, *ctrl, buf[256];
    int     ebx;

    rk = ctx->rk;
    blk = PADLOCK_ALIGN16(buf);
    memcpy(blk, input, 16);

    ctrl = blk + 4;
    *ctrl = 0x80 | ctx->nr | ((ctx->nr + (mode ^ 1) - 10) << 9);

asm("pushfl; popfl         \n" "movl    %%ebx, %0     \n" "movl    $1, %%ecx     \n" "movl    %2, %%edx     \n" "movl    %3, %%ebx     \n" "movl    %4, %%esi     \n" "movl    %4, %%edi     \n" ".byte  0xf3,0x0f,0xa7,0xc8\n" "movl    %1, %%ebx     \n":"=m"(ebx)
:       "m"(ebx), "m"(ctrl), "m"(rk), "m"(blk)
:       "ecx", "edx", "esi", "edi");

    memcpy(output, blk, 16);
    return 0;
}


/*
    PadLock AES-CBC buffer en(de)cryption
 */
int padlock_xcryptcbc(aes_context * ctx, int mode, int length, uchar iv[16], uchar *input, uchar *output)
{
    ulong   *rk, *iw, *ctrl, buf[256];
    int     ebx, count;

    if (((long)input & 15) != 0 || ((long)output & 15) != 0) {
        return 1;
    }
    rk = ctx->rk;
    iw = PADLOCK_ALIGN16(buf);
    memcpy(iw, iv, 16);

    ctrl = iw + 4;
    *ctrl = 0x80 | ctx->nr | ((ctx->nr + (mode ^ 1) - 10) << 9);

    count = (length + 15) >> 4;

asm("pushfl; popfl         \n" "movl    %%ebx, %0     \n" "movl    %2, %%ecx     \n" "movl    %3, %%edx     \n" "movl    %4, %%ebx     \n" "movl    %5, %%esi     \n" "movl    %6, %%edi     \n" "movl    %7, %%eax     \n" ".byte  0xf3,0x0f,0xa7,0xd0\n" "movl    %1, %%ebx     \n":"=m"(ebx)
:       "m"(ebx), "m"(count), "m"(ctrl),
        "m"(rk), "m"(input), "m"(output), "m"(iw)
:       "eax", "ecx", "edx", "esi", "edi");

    memcpy(iv, iw, 16);
    return 0;
}
#endif

#endif

/*
    @copy   default

    Copyright (c) Embedthis Software. All Rights Reserved.

    This software is distributed under commercial and open source licenses.
    You may use the Embedthis Open Source license or you may acquire a 
    commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.md distributed with
    this software for full details and other copyrights.

    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
