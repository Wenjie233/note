/*
    md4.c -- RFC 1186/1320 compliant MD4 implementation

    The MD4 algorithm was designed by Ron Rivest in 1990.
    http://www.ietf.org/rfc/rfc1186.txt
    http://www.ietf.org/rfc/rfc1320.txt

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#include "est.h"

#if ME_EST_MD4

/*
   32-bit integer manipulation macros (little endian)
 */
#ifndef GET_ULONG_LE
#define GET_ULONG_LE(n,b,i)                     \
    {                                           \
        (n) = ( (ulong) (b)[(i)    ]       )    \
            | ( (ulong) (b)[(i) + 1] <<  8 )    \
            | ( (ulong) (b)[(i) + 2] << 16 )    \
            | ( (ulong) (b)[(i) + 3] << 24 );   \
    }
#endif

#ifndef PUT_ULONG_LE
#define PUT_ULONG_LE(n,b,i)                     \
    {                                           \
        (b)[(i)    ] = (uchar) ( (n)       );   \
        (b)[(i) + 1] = (uchar) ( (n) >>  8 );   \
        (b)[(i) + 2] = (uchar) ( (n) >> 16 );   \
        (b)[(i) + 3] = (uchar) ( (n) >> 24 );   \
    }
#endif


/*
   MD4 context setup
 */
void md4_starts(md4_context * ctx)
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
}


static void md4_process(md4_context * ctx, uchar data[64])
{
    ulong X[16], A, B, C, D;

    GET_ULONG_LE(X[0], data, 0);
    GET_ULONG_LE(X[1], data, 4);
    GET_ULONG_LE(X[2], data, 8);
    GET_ULONG_LE(X[3], data, 12);
    GET_ULONG_LE(X[4], data, 16);
    GET_ULONG_LE(X[5], data, 20);
    GET_ULONG_LE(X[6], data, 24);
    GET_ULONG_LE(X[7], data, 28);
    GET_ULONG_LE(X[8], data, 32);
    GET_ULONG_LE(X[9], data, 36);
    GET_ULONG_LE(X[10], data, 40);
    GET_ULONG_LE(X[11], data, 44);
    GET_ULONG_LE(X[12], data, 48);
    GET_ULONG_LE(X[13], data, 52);
    GET_ULONG_LE(X[14], data, 56);
    GET_ULONG_LE(X[15], data, 60);

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];

#define F(x, y, z) ((x & y) | ((~x) & z))
#define P(a,b,c,d,x,s) { a += F(b,c,d) + x; a = S(a,s); }

    P(A, B, C, D, X[0], 3);
    P(D, A, B, C, X[1], 7);
    P(C, D, A, B, X[2], 11);
    P(B, C, D, A, X[3], 19);
    P(A, B, C, D, X[4], 3);
    P(D, A, B, C, X[5], 7);
    P(C, D, A, B, X[6], 11);
    P(B, C, D, A, X[7], 19);
    P(A, B, C, D, X[8], 3);
    P(D, A, B, C, X[9], 7);
    P(C, D, A, B, X[10], 11);
    P(B, C, D, A, X[11], 19);
    P(A, B, C, D, X[12], 3);
    P(D, A, B, C, X[13], 7);
    P(C, D, A, B, X[14], 11);
    P(B, C, D, A, X[15], 19);

#undef P
#undef F

#define F(x,y,z) ((x & y) | (x & z) | (y & z))
#define P(a,b,c,d,x,s) { a += F(b,c,d) + x + 0x5A827999; a = S(a,s); }

    P(A, B, C, D, X[0], 3);
    P(D, A, B, C, X[4], 5);
    P(C, D, A, B, X[8], 9);
    P(B, C, D, A, X[12], 13);
    P(A, B, C, D, X[1], 3);
    P(D, A, B, C, X[5], 5);
    P(C, D, A, B, X[9], 9);
    P(B, C, D, A, X[13], 13);
    P(A, B, C, D, X[2], 3);
    P(D, A, B, C, X[6], 5);
    P(C, D, A, B, X[10], 9);
    P(B, C, D, A, X[14], 13);
    P(A, B, C, D, X[3], 3);
    P(D, A, B, C, X[7], 5);
    P(C, D, A, B, X[11], 9);
    P(B, C, D, A, X[15], 13);

#undef P
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define P(a,b,c,d,x,s) { a += F(b,c,d) + x + 0x6ED9EBA1; a = S(a,s); }

    P(A, B, C, D, X[0], 3);
    P(D, A, B, C, X[8], 9);
    P(C, D, A, B, X[4], 11);
    P(B, C, D, A, X[12], 15);
    P(A, B, C, D, X[2], 3);
    P(D, A, B, C, X[10], 9);
    P(C, D, A, B, X[6], 11);
    P(B, C, D, A, X[14], 15);
    P(A, B, C, D, X[1], 3);
    P(D, A, B, C, X[9], 9);
    P(C, D, A, B, X[5], 11);
    P(B, C, D, A, X[13], 15);
    P(A, B, C, D, X[3], 3);
    P(D, A, B, C, X[11], 9);
    P(C, D, A, B, X[7], 11);
    P(B, C, D, A, X[15], 15);

#undef F
#undef P

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
}


/*
   MD4 process buffer
 */
void md4_update(md4_context * ctx, uchar *input, int ilen)
{
    int     fill;
    ulong   left;

    if (ilen <= 0)
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < (ulong) ilen) {
        ctx->total[1]++;
    }
    if (left && ilen >= fill) {
        memcpy((void *)(ctx->buffer + left), (void *)input, fill);
        md4_process(ctx, ctx->buffer);
        input += fill;
        ilen -= fill;
        left = 0;
    }
    while (ilen >= 64) {
        md4_process(ctx, input);
        input += 64;
        ilen -= 64;
    }
    if (ilen > 0) {
        memcpy((void *)(ctx->buffer + left), (void *)input, ilen);
    }
}


static const uchar md4_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


/*
    MD4 final digest
 */
void md4_finish(md4_context * ctx, uchar output[16])
{
    ulong last, padn;
    ulong high, low;
    uchar msglen[8];

    high = (ctx->total[0] >> 29) | (ctx->total[1] << 3);
    low = (ctx->total[0] << 3);

    PUT_ULONG_LE(low, msglen, 0);
    PUT_ULONG_LE(high, msglen, 4);

    last = ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    md4_update(ctx, (uchar *)md4_padding, padn);
    md4_update(ctx, msglen, 8);

    PUT_ULONG_LE(ctx->state[0], output, 0);
    PUT_ULONG_LE(ctx->state[1], output, 4);
    PUT_ULONG_LE(ctx->state[2], output, 8);
    PUT_ULONG_LE(ctx->state[3], output, 12);
}


/*
    output = MD4( input buffer )
 */
void md4(uchar *input, int ilen, uchar output[16])
{
    md4_context ctx;

    md4_starts(&ctx);
    md4_update(&ctx, input, ilen);
    md4_finish(&ctx, output);
    memset(&ctx, 0, sizeof(md4_context));
}


/*
    output = MD4( file contents )
 */
int md4_file(char *path, uchar output[16])
{
    FILE *f;
    size_t n;
    md4_context ctx;
    uchar buf[1024];

    if ((f = fopen(path, "rb")) == NULL) {
        return (1);
    }
    md4_starts(&ctx);

    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        md4_update(&ctx, buf, (int)n);
    }
    md4_finish(&ctx, output);
    memset(&ctx, 0, sizeof(md4_context));

    if (ferror(f) != 0) {
        fclose(f);
        return (2);
    }
    fclose(f);
    return 0;
}


/*
    MD4 HMAC context setup
 */
void md4_hmac_starts(md4_context * ctx, uchar *key, int keylen)
{
    int     i;
    uchar   sum[16];

    if (keylen > 64) {
        md4(key, keylen, sum);
        keylen = 16;
        key = sum;
    }
    memset(ctx->ipad, 0x36, 64);
    memset(ctx->opad, 0x5C, 64);

    for (i = 0; i < keylen; i++) {
        ctx->ipad[i] = (uchar)(ctx->ipad[i] ^ key[i]);
        ctx->opad[i] = (uchar)(ctx->opad[i] ^ key[i]);
    }
    md4_starts(ctx);
    md4_update(ctx, ctx->ipad, 64);
    memset(sum, 0, sizeof(sum));
}


/*
    MD4 HMAC process buffer
 */
void md4_hmac_update(md4_context * ctx, uchar *input, int ilen)
{
    md4_update(ctx, input, ilen);
}


/*
    MD4 HMAC final digest
 */
void md4_hmac_finish(md4_context * ctx, uchar output[16])
{
    uchar tmpbuf[16];

    md4_finish(ctx, tmpbuf);
    md4_starts(ctx);
    md4_update(ctx, ctx->opad, 64);
    md4_update(ctx, tmpbuf, 16);
    md4_finish(ctx, output);
    memset(tmpbuf, 0, sizeof(tmpbuf));
}


/*
    output = HMAC-MD4( hmac key, input buffer )
 */
void md4_hmac(uchar *key, int keylen, uchar *input, int ilen, uchar output[16])
{
    md4_context ctx;

    md4_hmac_starts(&ctx, key, keylen);
    md4_hmac_update(&ctx, input, ilen);
    md4_hmac_finish(&ctx, output);
    memset(&ctx, 0, sizeof(md4_context));
}


#undef P
#undef R
#undef SHR
#undef ROTR
#undef S0
#undef S1
#undef S2
#undef S3
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
