/*
    rsa_verify.c -- RSA/SHA-1 signature verification program

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
    int ret, i, c;
    rsa_context rsa;
    uchar hash[20];
    uchar buf[512];

    ret = 1;
    if (argc != 2) {
        printf("usage: rsa_verify <filename>\n");

#ifdef WIN32
        printf("\n");
#endif

        goto exit;
    }

    printf("\n  . Reading public key from rsa_pub.txt");
    fflush(stdout);

    if ((f = fopen("rsa_pub.txt", "rb")) == NULL) {
        printf(" failed\n  ! Could not open rsa_pub.txt\n"
               "  ! Please run rsa_genkey first\n\n");
        goto exit;
    }

    rsa_init(&rsa, RSA_PKCS_V15, 0, NULL, NULL);

    if ((ret = mpi_read_file(&rsa.N, 16, f)) != 0 ||
        (ret = mpi_read_file(&rsa.E, 16, f)) != 0) {
        printf(" failed\n  ! mpi_read_file returned %d\n\n", ret);
        goto exit;
    }

    rsa.len = (mpi_msb(&rsa.N) + 7) >> 3;

    fclose(f);

    /*
     * Extract the RSA signature from the text file
     */
    ret = 1;
    i = strlen(argv[1]);
    memcpy(argv[1] + i, ".sig", 5);

    if ((f = fopen(argv[1], "rb")) == NULL) {
        printf("\n  ! Could not open %s\n\n", argv[1]);
        goto exit;
    }

    argv[1][i] = '\0', i = 0;

    while (fscanf(f, "%02X", &c) > 0 && i < (int)sizeof(buf))
        buf[i++] = (uchar)c;

    fclose(f);

    if (i != rsa.len) {
        printf("\n  ! Invalid RSA signature format\n\n");
        goto exit;
    }

    /*
     * Compute the SHA-1 hash of the input file and compare
     * it with the hash decrypted from the RSA signature.
     */
    printf("\n  . Verifying the RSA/SHA-1 signature");
    fflush(stdout);

    if ((ret = sha1_file(argv[1], hash)) != 0) {
        printf(" failed\n  ! Could not open or read %s\n\n", argv[1]);
        goto exit;
    }

    if ((ret = rsa_pkcs1_verify(&rsa, RSA_PUBLIC, RSA_SHA1,
                    20, hash, buf)) != 0) {
        printf(" failed\n  ! rsa_pkcs1_verify returned %d\n\n", ret);
        goto exit;
    }

    printf("\n  . OK (the decrypted SHA-1 hash matches)\n\n");

    ret = 0;

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
