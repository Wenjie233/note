/*
    arc4.c -- An implementation of the ARCFOUR algorithm

    The ARCFOUR algorithm was publicly disclosed on 94/09.

    http://groups.google.com/group/sci.crypt/msg/10a300c9d21afca0

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#include "est.h"

#if ME_EST_RC4
/*
   ARC4 key schedule
 */
void arc4_setup(arc4_context *ctx, uchar *key, int keylen)
{
    int     i, j, k, a;
    uchar   *m;

    ctx->x = 0;
    ctx->y = 0;
    m = ctx->m;

    for (i = 0; i < 256; i++) {
        m[i] = (uchar) i;
    }
    j = k = 0;
    for (i = 0; i < 256; i++, k++) {
        if (k >= keylen) {
            k = 0;
        }
        a = m[i];
        j = (j + a + key[k]) & 0xFF;
        m[i] = m[j];
        m[j] = (uchar)a;
    }
}


/*
   ARC4 cipher function
 */
void arc4_crypt(arc4_context *ctx, uchar *buf, int buflen)
{
    int     i, x, y, a, b;
    uchar   *m;

    x = ctx->x;
    y = ctx->y;
    m = ctx->m;

    for (i = 0; i < buflen; i++) {
        x = (x + 1) & 0xFF;
        a = m[x];
        y = (y + a) & 0xFF;
        b = m[y];

        m[x] = (uchar)b;
        m[y] = (uchar)a;

        buf[i] = (uchar) (buf[i] ^ m[(uchar)(a + b)]);
    }
    ctx->x = x;
    ctx->y = y;
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
