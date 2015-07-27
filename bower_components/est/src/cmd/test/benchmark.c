/*
    benchmark.c -- Benchmark demonstration program

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tropicssl/config.h"

#include "tropicssl/md4.h"
#include "tropicssl/md5.h"
#include "tropicssl/sha1.h"
#include "tropicssl/sha2.h"
#include "tropicssl/arc4.h"
#include "tropicssl/des.h"
#include "tropicssl/aes.h"
#include "tropicssl/camellia.h"
#include "tropicssl/rsa.h"
#include "tropicssl/timing.h"

#define BUFSIZE 1024

static int myrand(void *rng_state)
{
    if (rng_state != NULL)
        rng_state = NULL;

    return (rand());
}

uchar buf[BUFSIZE];

int main(void)
{
    int keysize;
    ulong i, j, tsc;
    uchar tmp[32];
#if defined(TROPICSSL_ARC4_C)
    arc4_context arc4;
#endif
#if defined(TROPICSSL_DES_C)
    des3_context des3;
    des_context des;
#endif
#if defined(TROPICSSL_AES_C)
    aes_context aes;
#endif
#if defined(TROPICSSL_CAMELLIA_C)
    camellia_context camellia;
#endif
#if defined(TROPICSSL_RSA_C)
    rsa_context rsa;
#endif

    memset(buf, 0xAA, sizeof(buf));

    printf("\n");

#if defined(TROPICSSL_MD4_C)
    printf("  MD4       :  ");
    fflush(stdout);

    set_alarm(1);
    for (i = 1; !alarmed; i++)
        md4(buf, BUFSIZE, tmp);

    tsc = hardclock();
    for (j = 0; j < 1024; j++)
        md4(buf, BUFSIZE, tmp);

    printf("%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
           (hardclock() - tsc) / (j * BUFSIZE));
#endif

#if defined(TROPICSSL_MD5_C)
    printf("  MD5       :  ");
    fflush(stdout);

    set_alarm(1);
    for (i = 1; !alarmed; i++)
        md5(buf, BUFSIZE, tmp);

    tsc = hardclock();
    for (j = 0; j < 1024; j++)
        md5(buf, BUFSIZE, tmp);

    printf("%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
           (hardclock() - tsc) / (j * BUFSIZE));
#endif

#if defined(TROPICSSL_SHA1_C)
    printf("  SHA-1     :  ");
    fflush(stdout);

    set_alarm(1);
    for (i = 1; !alarmed; i++)
        sha1(buf, BUFSIZE, tmp);

    tsc = hardclock();
    for (j = 0; j < 1024; j++)
        sha1(buf, BUFSIZE, tmp);

    printf("%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
           (hardclock() - tsc) / (j * BUFSIZE));
#endif

#if defined(TROPICSSL_SHA2_C)
    printf("  SHA-256   :  ");
    fflush(stdout);

    set_alarm(1);
    for (i = 1; !alarmed; i++)
        sha2(buf, BUFSIZE, tmp, 0);

    tsc = hardclock();
    for (j = 0; j < 1024; j++)
        sha2(buf, BUFSIZE, tmp, 0);

    printf("%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
           (hardclock() - tsc) / (j * BUFSIZE));
#endif

#if defined(TROPICSSL_ARC4_C)
    printf("  ARC4      :  ");
    fflush(stdout);

    arc4_setup(&arc4, tmp, 32);

    set_alarm(1);
    for (i = 1; !alarmed; i++)
        arc4_crypt(&arc4, buf, BUFSIZE);

    tsc = hardclock();
    for (j = 0; j < 1024; j++)
        arc4_crypt(&arc4, buf, BUFSIZE);

    printf("%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
           (hardclock() - tsc) / (j * BUFSIZE));
#endif

#if defined(TROPICSSL_DES_C)
    printf("  3DES      :  ");
    fflush(stdout);

    des3_set3key_enc(&des3, tmp);

    set_alarm(1);
    for (i = 1; !alarmed; i++)
        des3_crypt_cbc(&des3, DES_ENCRYPT, BUFSIZE, tmp, buf, buf);

    tsc = hardclock();
    for (j = 0; j < 1024; j++)
        des3_crypt_cbc(&des3, DES_ENCRYPT, BUFSIZE, tmp, buf, buf);

    printf("%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
           (hardclock() - tsc) / (j * BUFSIZE));

    printf("  DES       :  ");
    fflush(stdout);

    des_setkey_enc(&des, tmp);

    set_alarm(1);
    for (i = 1; !alarmed; i++)
        des_crypt_cbc(&des, DES_ENCRYPT, BUFSIZE, tmp, buf, buf);

    tsc = hardclock();
    for (j = 0; j < 1024; j++)
        des_crypt_cbc(&des, DES_ENCRYPT, BUFSIZE, tmp, buf, buf);

    printf("%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
           (hardclock() - tsc) / (j * BUFSIZE));
#endif

#if defined(TROPICSSL_AES_C)
    for (keysize = 128; keysize <= 256; keysize += 64) {
        printf("  AES-%d   :  ", keysize);
        fflush(stdout);

        memset(buf, 0, sizeof(buf));
        memset(tmp, 0, sizeof(tmp));
        aes_setkey_enc(&aes, tmp, keysize);

        set_alarm(1);

        for (i = 1; !alarmed; i++)
            aes_crypt_cbc(&aes, AES_ENCRYPT, BUFSIZE, tmp, buf,
                      buf);

        tsc = hardclock();
        for (j = 0; j < 4096; j++)
            aes_crypt_cbc(&aes, AES_ENCRYPT, BUFSIZE, tmp, buf,
                      buf);

        printf("%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
               (hardclock() - tsc) / (j * BUFSIZE));
    }
#endif

#if defined(TROPICSSL_CAMELLIA_C)
    for (keysize = 128; keysize <= 256; keysize += 64) {
        printf("  CAMELLIA-%d   :  ", keysize);
        fflush(stdout);

        memset(buf, 0, sizeof(buf));
        memset(tmp, 0, sizeof(tmp));
        camellia_setkey_enc(&camellia, tmp, keysize);

        set_alarm(1);

        for (i = 1; !alarmed; i++)
            camellia_crypt_cbc(&camellia, CAMELLIA_ENCRYPT, BUFSIZE,
                       tmp, buf, buf);

        tsc = hardclock();
        for (j = 0; j < 4096; j++)
            camellia_crypt_cbc(&camellia, CAMELLIA_ENCRYPT, BUFSIZE,
                       tmp, buf, buf);

        printf("%9lu Kb/s,  %9lu cycles/byte\n", i * BUFSIZE / 1024,
               (hardclock() - tsc) / (j * BUFSIZE));
    }
#endif

#if defined(TROPICSSL_RSA_C)
    rsa_init(&rsa, RSA_PKCS_V15, 0, myrand, NULL);
    rsa_gen_key(&rsa, 1024, 65537);

    printf("  RSA-1024  :  ");
    fflush(stdout);
    set_alarm(3);

    for (i = 1; !alarmed; i++) {
        buf[0] = 0;
        rsa_public(&rsa, buf, buf);
    }

    printf("%9lu  public/s\n", i / 3);

    printf("  RSA-1024  :  ");
    fflush(stdout);
    set_alarm(3);

    for (i = 1; !alarmed; i++) {
        buf[0] = 0;
        rsa_private(&rsa, buf, buf);
    }

    printf("%9lu private/s\n\n", i / 3);

    rsa_free(&rsa);
#endif

#ifdef WIN32
    printf("  Press Enter to exit this program.\n");
    fflush(stdout);
    getchar();
#endif

    return (0);
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
