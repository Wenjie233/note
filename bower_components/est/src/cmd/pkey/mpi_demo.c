/*
    mpi_demo.c -- Simple MPI demonstration program

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <stdio.h>

#include "tropicssl/bignum.h"

int main(void)
{
    mpi E, P, Q, N, H, D, X, Y, Z;

    mpi_init(&E, &P, &Q, &N, &H, &D, &X, &Y, &Z, NULL);

    mpi_read_string(&P, 10, "2789");
    mpi_read_string(&Q, 10, "3203");
    mpi_read_string(&E, 10, "257");
    mpi_mul_mpi(&N, &P, &Q);

    printf("\n  Public key:\n\n");
    mpi_write_file("  N = ", &N, 10, NULL);
    mpi_write_file("  E = ", &E, 10, NULL);

    printf("\n  Private key:\n\n");
    mpi_write_file("  P = ", &P, 10, NULL);
    mpi_write_file("  Q = ", &Q, 10, NULL);

    mpi_sub_int(&P, &P, 1);
    mpi_sub_int(&Q, &Q, 1);
    mpi_mul_mpi(&H, &P, &Q);
    mpi_inv_mod(&D, &E, &H);

    mpi_write_file("  D = E^-1 mod (P-1)*(Q-1) = ", &D, 10, NULL);

    mpi_read_string(&X, 10, "55555");
    mpi_exp_mod(&Y, &X, &E, &N, NULL);
    mpi_exp_mod(&Z, &Y, &D, &N, NULL);

    printf("\n  RSA operation:\n\n");
    mpi_write_file("  X (plaintext)  = ", &X, 10, NULL);
    mpi_write_file("  Y (ciphertext) = X^E mod N = ", &Y, 10, NULL);
    mpi_write_file("  Z (decrypted)  = Y^D mod N = ", &Z, 10, NULL);
    printf("\n");

    mpi_free(&Z, &Y, &X, &D, &H, &N, &Q, &P, &E, NULL);

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
