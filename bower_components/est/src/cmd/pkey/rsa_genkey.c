/*
    rsa_genkey.c -- Example RSA key generation program

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <stdio.h>

#include "tropicssl/havege.h"
#include "tropicssl/bignum.h"
#include "tropicssl/x509.h"
#include "tropicssl/rsa.h"

#define KEY_SIZE 1024
#define EXPONENT 65537

int main(void)
{
    int ret;
    rsa_context rsa;
    havege_state hs;
    FILE *fpub = NULL;
    FILE *fpriv = NULL;
    x509_raw cert;

    printf("\n  . Seeding the random number generator...");
    fflush(stdout);

    havege_init(&hs);

    printf(" ok\n  . Generating the RSA key [ %d-bit ]...", KEY_SIZE);
    fflush(stdout);

    rsa_init(&rsa, RSA_PKCS_V15, 0, havege_rand, &hs);

    if ((ret = rsa_gen_key(&rsa, KEY_SIZE, EXPONENT)) != 0) {
        printf(" failed\n  ! rsa_gen_key returned %d\n\n", ret);
        goto exit;
    }

    printf(" ok\n  . Exporting the public  key in rsa_pub.txt....");
    fflush(stdout);

    if ((fpub = fopen("rsa_pub.txt", "wb+")) == NULL) {
        printf
            (" failed\n  ! could not open rsa_pub.txt for writing\n\n");
        ret = 1;
        goto exit;
    }

    if ((ret = mpi_write_file("N = ", &rsa.N, 16, fpub)) != 0 ||
        (ret = mpi_write_file("E = ", &rsa.E, 16, fpub)) != 0) {
        printf(" failed\n  ! mpi_write_file returned %d\n\n", ret);
        goto exit;
    }

    printf(" ok\n  . Exporting the private key in rsa_priv.txt...");
    fflush(stdout);

    if ((fpriv = fopen("rsa_priv.txt", "wb+")) == NULL) {
        printf
            (" failed\n  ! could not open rsa_priv.txt for writing\n");
        ret = 1;
        goto exit;
    }

    if ((ret = mpi_write_file("N = ", &rsa.N, 16, fpriv)) != 0 ||
        (ret = mpi_write_file("E = ", &rsa.E, 16, fpriv)) != 0 ||
        (ret = mpi_write_file("D = ", &rsa.D, 16, fpriv)) != 0 ||
        (ret = mpi_write_file("P = ", &rsa.P, 16, fpriv)) != 0 ||
        (ret = mpi_write_file("Q = ", &rsa.Q, 16, fpriv)) != 0 ||
        (ret = mpi_write_file("DP = ", &rsa.DP, 16, fpriv)) != 0 ||
        (ret = mpi_write_file("DQ = ", &rsa.DQ, 16, fpriv)) != 0 ||
        (ret = mpi_write_file("QP = ", &rsa.QP, 16, fpriv)) != 0) {
        printf(" failed\n  ! mpi_write_file returned %d\n\n", ret);
        goto exit;
    }
/*
    printf( " ok\n  . Generating the certificate..." );

    x509write_init_raw( &cert );
    x509write_add_pubkey( &cert, &rsa );
    x509write_add_subject( &cert, "CN='localhost'" );
    x509write_add_validity( &cert, "2007-09-06 17:00:32",
                                   "2010-09-06 17:00:32" );
    x509write_create_selfsign( &cert, &rsa );
    x509write_crtfile( &cert, "cert.der", X509_OUTPUT_DER );
    x509write_crtfile( &cert, "cert.pem", X509_OUTPUT_PEM );
    x509write_free_raw( &cert );
*/
    printf(" ok\n\n");

exit:

    if (fpub != NULL)
        fclose(fpub);

    if (fpriv != NULL)
        fclose(fpriv);

    rsa_free(&rsa);

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
