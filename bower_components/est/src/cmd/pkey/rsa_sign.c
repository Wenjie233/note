/*
    rsa_sign.h -- RSA/SHA-1 signature creation program

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <string.h>
#include <stdio.h>

#include "tropicssl/rsa.h"
#include "tropicssl/sha1.h"

int main(int argc, char *argv[])
{
    FILE *f;
    int ret, i;
    rsa_context rsa;
    uchar hash[20];
    uchar buf[512];

    ret = 1;

    if (argc != 2) {
        printf("usage: rsa_sign <filename>\n");

#ifdef WIN32
        printf("\n");
#endif

        goto exit;
    }

    printf("\n  . Reading private key from rsa_priv.txt");
    fflush(stdout);

    if ((f = fopen("rsa_priv.txt", "rb")) == NULL) {
        ret = 1;
        printf(" failed\n  ! Could not open rsa_priv.txt\n"
               "  ! Please run rsa_genkey first\n\n");
        goto exit;
    }

    rsa_init(&rsa, RSA_PKCS_V15, 0, NULL, NULL);

    if ((ret = mpi_read_file(&rsa.N, 16, f)) != 0 ||
        (ret = mpi_read_file(&rsa.E, 16, f)) != 0 ||
        (ret = mpi_read_file(&rsa.D, 16, f)) != 0 ||
        (ret = mpi_read_file(&rsa.P, 16, f)) != 0 ||
        (ret = mpi_read_file(&rsa.Q, 16, f)) != 0 ||
        (ret = mpi_read_file(&rsa.DP, 16, f)) != 0 ||
        (ret = mpi_read_file(&rsa.DQ, 16, f)) != 0 ||
        (ret = mpi_read_file(&rsa.QP, 16, f)) != 0) {
        printf(" failed\n  ! mpi_read_file returned %d\n\n", ret);
        goto exit;
    }

    rsa.len = (mpi_msb(&rsa.N) + 7) >> 3;

    fclose(f);

    /*
     * Compute the SHA-1 hash of the input file,
     * then calculate the RSA signature of the hash.
     */
    printf("\n  . Generating the RSA/SHA-1 signature");
    fflush(stdout);

    if ((ret = sha1_file(argv[1], hash)) != 0) {
        printf(" failed\n  ! Could not open or read %s\n\n", argv[1]);
        goto exit;
    }

    if ((ret = rsa_pkcs1_sign(&rsa, RSA_PRIVATE, RSA_SHA1,
                  20, hash, buf)) != 0) {
        printf(" failed\n  ! rsa_pkcs1_sign returned %d\n\n", ret);
        goto exit;
    }

    /*
     * Write the signature into <filename>-sig.txt
     */
    memcpy(argv[1] + strlen(argv[1]), ".sig", 5);

    if ((f = fopen(argv[1], "wb+")) == NULL) {
        ret = 1;
        printf(" failed\n  ! Could not create %s\n\n", argv[1]);
        goto exit;
    }

    for (i = 0; i < rsa.len; i++)
        fprintf(f, "%02X%s", buf[i], (i + 1) % 16 == 0 ? "\r\n" : " ");

    fclose(f);

    printf("\n  . Done (created \"%s\")\n\n", argv[1]);

exit:

#ifdef WIN32
    printf("  + Press Enter to exit this program.\n");
    fflush(stdout);
    getchar();
#endif

    return (ret);
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
