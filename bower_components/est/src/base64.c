/*
    base64.c -- RFC 1521 base64 encoding/decoding

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#include "est.h"

#if ME_EST_BASE64
static const uchar base64_enc_map[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/'
};

static const uchar base64_dec_map[128] = {
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 62, 127, 127, 127, 63, 52, 53,
    54, 55, 56, 57, 58, 59, 60, 61, 127, 127,
    127, 64, 127, 127, 127, 0, 1, 2, 3, 4,
    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 127, 127, 127, 127, 127, 127, 26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 127, 127, 127, 127, 127
};

/*
 * Encode a buffer into base64 format
 */
int base64_encode(uchar *dst, int *dlen, uchar *src, int slen)
{
    int i, n;
    int C1, C2, C3;
    uchar *p;

    if (slen == 0) {
        return 0;
    }
    n = (slen << 3) / 6;

    switch ((slen << 3) - (n * 6)) {
    case 2:
        n += 3;
        break;
    case 4:
        n += 2;
        break;
    default:
        break;
    }

    if (*dlen < n + 1) {
        *dlen = n + 1;
        return EST_ERR_BASE64_BUFFER_TOO_SMALL;
    }

    n = (slen / 3) * 3;

    for (i = 0, p = dst; i < n; i += 3) {
        C1 = *src++;
        C2 = *src++;
        C3 = *src++;
        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];
        *p++ = base64_enc_map[(((C2 & 15) << 2) + (C3 >> 6)) & 0x3F];
        *p++ = base64_enc_map[C3 & 0x3F];
    }

    if (i < slen) {
        C1 = *src++;
        C2 = ((i + 1) < slen) ? *src++ : 0;

        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];

        if ((i + 1) < slen) {
            *p++ = base64_enc_map[((C2 & 15) << 2) & 0x3F];
        } else {
            *p++ = '=';
        }
        *p++ = '=';
    }
    *dlen = p - dst;
    *p = 0;
    return 0;
}

/*
    Decode a base64-formatted buffer
 */
int base64_decode(uchar *dst, int *dlen, uchar *src, int slen)
{
    int i, j, n;
    ulong x;
    uchar *p;

    for (i = j = n = 0; i < slen; i++) {
        if ((slen - i) >= 2 && src[i] == '\r' && src[i + 1] == '\n') {
            continue;
        }
        if (src[i] == '\n') {
            continue;
        }
        if (src[i] == '=' && ++j > 2) {
            return EST_ERR_BASE64_INVALID_CHARACTER;
        }
        if (src[i] > 127 || base64_dec_map[src[i]] == 127) {
            return EST_ERR_BASE64_INVALID_CHARACTER;
        }
        if (base64_dec_map[src[i]] < 64 && j != 0) {
            return EST_ERR_BASE64_INVALID_CHARACTER;
        }
        n++;
    }

    if (n == 0) {
        return 0;
    }
    n = ((n * 6) + 7) >> 3;

    if (*dlen < n) {
        *dlen = n;
        return EST_ERR_BASE64_BUFFER_TOO_SMALL;
    }
    for (j = 3, n = x = 0, p = dst; i > 0; i--, src++) {
        if (*src == '\r' || *src == '\n')
            continue;

        j -= (base64_dec_map[*src] == 64);
        x = (x << 6) | (base64_dec_map[*src] & 0x3F);

        if (++n == 4) {
            n = 0;
            if (j > 0) {
                *p++ = (uchar)(x >> 16);
            }
            if (j > 1) {
                *p++ = (uchar)(x >> 8);
            }
            if (j > 2) {
                *p++ = (uchar)(x);
            }
        }
    }
    *dlen = p - dst;
    return 0;
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
