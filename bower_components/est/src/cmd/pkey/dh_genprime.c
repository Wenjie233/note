/*
    dh_genprime.c -- Diffie-Hellman-Merkle key exchange (prime generation)

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <stdio.h>

#include "tropicssl/bignum.h"
#include "tropicssl/config.h"
#include "tropicssl/havege.h"

/*
 * Note: G = 4 is always a quadratic residue mod P,
 * so it is a generator of order Q (with P = 2*Q+1).
 */
#define DH_P_SIZE 1024
#define GENERATOR "4"

int main(void)
{
    int ret = 1;

#if defined(TROPICSSL_GENPRIME)
    mpi G, P, Q;
    havege_state hs;
    FILE *fout;

    mpi_init(&G, &P, &Q, NULL);
    mpi_read_string(&G, 10, GENERATOR);

    printf("\n  . Seeding the random number generator...");
    fflush(stdout);

    havege_init(&hs);

    printf(" ok\n  . Generating the modulus, please wait...");
    fflush(stdout);

    /*
     * This can take a long time...
     */
    if ((ret = mpi_gen_prime(&P, DH_P_SIZE, 1, havege_rand, &hs)) != 0) {
        printf(" failed\n  ! mpi_gen_prime returned %d\n\n", ret);
        goto exit;
    }

    printf(" ok\n  . Verifying that Q = (P-1)/2 is prime...");
    fflush(stdout);

    if ((ret = mpi_sub_int(&Q, &P, 1)) != 0) {
        printf(" failed\n  ! mpi_sub_int returned %d\n\n", ret);
        goto exit;
    }

    if ((ret = mpi_div_int(&Q, NULL, &Q, 2)) != 0) {
        printf(" failed\n  ! mpi_div_int returned %d\n\n", ret);
        goto exit;
    }

    if ((ret = mpi_is_prime(&Q, havege_rand, &hs)) != 0) {
        printf(" failed\n  ! mpi_is_prime returned %d\n\n", ret);
        goto exit;
    }

    printf(" ok\n  . Exporting the value in dh_prime.txt...");
    fflush(stdout);

    if ((fout = fopen("dh_prime.txt", "wb+")) == NULL) {
        ret = 1;
        printf(" failed\n  ! Could not create dh_prime.txt\n\n");
        goto exit;
    }

    if ((ret = mpi_write_file("P = ", &P, 16, fout) != 0) ||
        (ret = mpi_write_file("G = ", &G, 16, fout) != 0)) {
        printf(" failed\n  ! mpi_write_file returned %d\n\n", ret);
        goto exit;
    }

    printf(" ok\n\n");
    fclose(fout);

exit:

    mpi_free(&Q, &P, &G, NULL);
#else
    printf("\n  ! Prime-number generation is not available.\n\n");
#endif

#ifdef WIN32
    printf("  Press Enter to exit this program.\n");
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
