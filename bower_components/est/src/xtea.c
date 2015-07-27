/*
    xtea.c -- An 32-bit implementation of the XTEA algorithm

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#include "est.h"

#if ME_EST_XTEA

/*
   32-bit integer manipulation macros (big endian)
 */
#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n,b,i)                     \
    {                                           \
        (n) = ( (ulong) (b)[(i)    ] << 24 )    \
            | ( (ulong) (b)[(i) + 1] << 16 )    \
            | ( (ulong) (b)[(i) + 2] <<  8 )    \
            | ( (ulong) (b)[(i) + 3]       );   \
    }
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                     \
    {                                           \
        (b)[(i)    ] = (uchar) ( (n) >> 24 );   \
        (b)[(i) + 1] = (uchar) ( (n) >> 16 );   \
        (b)[(i) + 2] = (uchar) ( (n) >>  8 );   \
        (b)[(i) + 3] = (uchar) ( (n)       );   \
    }
#endif

/*
   XTEA key schedule
 */
void xtea_setup(xtea_context *ctx, uchar key[16])
{
    int i;

    memset(ctx, 0, sizeof(xtea_context));
    for (i = 0; i < 4; i++) {
        GET_ULONG_BE(ctx->k[i], key, i << 2);
    }
}


/*
   XTEA encrypt function
 */
void xtea_crypt_ecb(xtea_context *ctx, int mode, uchar input[8], uchar output[8])
{
    ulong *k, v0, v1, i;

    k = ctx->k;
    GET_ULONG_BE(v0, input, 0);
    GET_ULONG_BE(v1, input, 4);

    if (mode == XTEA_ENCRYPT) {
        ulong sum = 0, delta = 0x9E3779B9;

        for (i = 0; i < 32; i++) {
            v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
            sum += delta;
            v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
        }
    } else {
        /* XTEA_DECRYPT */
        ulong delta = 0x9E3779B9, sum = delta * 32;

        for (i = 0; i < 32; i++) {
            v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
            sum -= delta;
            v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
        }
    }
    PUT_ULONG_BE(v0, output, 0);
    PUT_ULONG_BE(v1, output, 4);
}

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
