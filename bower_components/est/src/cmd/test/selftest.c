/*
    selftest.h -- Self-test demonstration program

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <string.h>
#include <stdio.h>

#include "tropicssl/config.h"

#include "tropicssl/md2.h"
#include "tropicssl/md4.h"
#include "tropicssl/md5.h"
#include "tropicssl/sha1.h"
#include "tropicssl/sha2.h"
#include "tropicssl/sha4.h"
#include "tropicssl/arc4.h"
#include "tropicssl/des.h"
#include "tropicssl/aes.h"
#include "tropicssl/base64.h"
#include "tropicssl/bignum.h"
#include "tropicssl/rsa.h"
#include "tropicssl/x509.h"
#include "tropicssl/xtea.h"

int main(int argc, char *argv[])
{
    int ret, v;

    if (argc == 2 && strcmp(argv[1], "-quiet") == 0)
        v = 0;
    else {
        v = 1;
        printf("\n");
    }

#if defined(TROPICSSL_MD2_C)
    if ((ret = md2_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_MD4_C)
    if ((ret = md4_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_MD5_C)
    if ((ret = md5_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_SHA1_C)
    if ((ret = sha1_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_SHA2_C)
    if ((ret = sha2_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_SHA4_C)
    if ((ret = sha4_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_ARC4_C)
    if ((ret = arc4_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_DES_C)
    if ((ret = des_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_AES_C)
    if ((ret = aes_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_BASE64_C)
    if ((ret = base64_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_BIGNUM_C)
    if ((ret = mpi_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_RSA_C)
    if ((ret = rsa_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_X509_C)
    if ((ret = x509_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_XTEA_C)
    if ((ret = xtea_self_test(v)) != 0)
        return (ret);
#endif

#if defined(TROPICSSL_CAMELLIA_C)
    if ((ret = camellia_self_test(v)) != 0)
        return (ret);
#endif

    if (v != 0) {
        printf("  [ All tests passed ]\n\n");
#ifdef WIN32
        printf("  Press Enter to exit this program.\n");
        fflush(stdout);
        getchar();
#endif
    }
    return ret;
}

/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2014. All Rights Reserved.

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
