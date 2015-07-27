/*
    dh_server.c -- Diffie-Hellman-Merkle key exchange (server side)

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <string.h>
#include <stdio.h>

#include "tropicssl/net.h"
#include "tropicssl/aes.h"
#include "tropicssl/dhm.h"
#include "tropicssl/rsa.h"
#include "tropicssl/sha1.h"
#include "tropicssl/havege.h"

#define SERVER_PORT 11999
#define PLAINTEXT "==Hello there!=="

int main(void)
{
    FILE *f;

    int ret, n, buflen;
    int listen_fd = -1;
    int client_fd = -1;

    uchar buf[1024];
    uchar hash[20];
    uchar buf2[2];

    havege_state hs;
    rsa_context rsa;
    dhm_context dhm;
    aes_context aes;

    memset(&rsa, 0, sizeof(rsa));
    memset(&dhm, 0, sizeof(dhm));

    /*
     * 1. Setup the RNG
     */
    printf("\n  . Seeding the random number generator");
    fflush(stdout);

    havege_init(&hs);

    /*
     * 2a. Read the server's private RSA key
     */
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
     * 2b. Get the DHM modulus and generator
     */
    printf("\n  . Reading DH parameters from dh_prime.txt");
    fflush(stdout);

    if ((f = fopen("dh_prime.txt", "rb")) == NULL) {
        ret = 1;
        printf(" failed\n  ! Could not open dh_prime.txt\n"
               "  ! Please run dh_genprime first\n\n");
        goto exit;
    }

    if (mpi_read_file(&dhm.P, 16, f) != 0 ||
        mpi_read_file(&dhm.G, 16, f) != 0) {
        printf(" failed\n  ! Invalid DH parameter file\n\n");
        goto exit;
    }

    fclose(f);

    /*
     * 3. Wait for a client to connect
     */
    printf("\n  . Waiting for a remote connection");
    fflush(stdout);

    if ((ret = net_bind(&listen_fd, NULL, SERVER_PORT)) != 0) {
        printf(" failed\n  ! net_bind returned %d\n\n", ret);
        goto exit;
    }

    if ((ret = net_accept(listen_fd, &client_fd, NULL)) != 0) {
        printf(" failed\n  ! net_accept returned %d\n\n", ret);
        goto exit;
    }

    /*
     * 4. Setup the DH parameters (P,G,Ys)
     */
    printf("\n  . Sending the server's DH parameters");
    fflush(stdout);

    memset(buf, 0, sizeof(buf));

    if ((ret = dhm_make_params(&dhm, 256, buf, &n, havege_rand, &hs)) != 0) {
        printf(" failed\n  ! dhm_make_params returned %d\n\n", ret);
        goto exit;
    }

    /*
     * 5. Sign the parameters and send them
     */
    sha1(buf, n, hash);

    buf[n] = (uchar)(rsa.len >> 8);
    buf[n + 1] = (uchar)(rsa.len);

    if ((ret = rsa_pkcs1_sign(&rsa, RSA_PRIVATE, RSA_SHA1,
                  0, hash, buf + n + 2)) != 0) {
        printf(" failed\n  ! rsa_pkcs1_sign returned %d\n\n", ret);
        goto exit;
    }

    buflen = n + 2 + rsa.len;
    buf2[0] = (uchar)(buflen >> 8);
    buf2[1] = (uchar)(buflen);

    if ((ret = net_send(&client_fd, buf2, 2)) != 2 ||
        (ret = net_send(&client_fd, buf, buflen)) != buflen) {
        printf(" failed\n  ! net_send returned %d\n\n", ret);
        goto exit;
    }

    /*
     * 6. Get the client's public value: Yc = G ^ Xc mod P
     */
    printf("\n  . Receiving the client's public value");
    fflush(stdout);

    memset(buf, 0, sizeof(buf));
    n = dhm.len;

    if ((ret = net_recv(&client_fd, buf, n)) != n) {
        printf(" failed\n  ! net_recv returned %d\n\n", ret);
        goto exit;
    }

    if ((ret = dhm_read_public(&dhm, buf, dhm.len)) != 0) {
        printf(" failed\n  ! dhm_read_public returned %d\n\n", ret);
        goto exit;
    }

    /*
     * 7. Derive the shared secret: K = Ys ^ Xc mod P
     */
    printf("\n  . Shared secret: ");
    fflush(stdout);

    if ((ret = dhm_calc_secret(&dhm, buf, &n)) != 0) {
        printf(" failed\n  ! dhm_calc_secret returned %d\n\n", ret);
        goto exit;
    }

    for (n = 0; n < 16; n++)
        printf("%02x", buf[n]);

    /*
     * 8. Setup the AES-256 encryption key
     *
     * This is an overly simplified example; best practice is
     * to hash the shared secret with a random value to derive
     * the keying material for the encryption/decryption keys
     * and MACs.
     */
    printf("...\n  . Encrypting and sending the ciphertext");
    fflush(stdout);

    aes_setkey_enc(&aes, buf, 256);
    memcpy(buf, PLAINTEXT, 16);
    aes_crypt_ecb(&aes, AES_ENCRYPT, buf, buf);

    if ((ret = net_send(&client_fd, buf, 16)) != 16) {
        printf(" failed\n  ! net_send returned %d\n\n", ret);
        goto exit;
    }

    printf("\n\n");

exit:

    net_close(client_fd);
    rsa_free(&rsa);
    dhm_free(&dhm);

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
