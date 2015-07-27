/*
    openssl.h -- 

    Copyright (c) All Rights Reserved. See details at the end of the file.

 */

#if UNUSED
/*
   OpenSSL wrapper contributed by David Barett
 */
#ifndef EST_OPENSSL_H
#define EST_OPENSSL_H

#define AES_SIZE                16
#define AES_BLOCK_SIZE          16
#define AES_KEY                 aes_context
#define MD5_CTX                 md5_context
#define SHA_CTX                 sha1_context

#define SHA1_Init(CTX) sha1_starts((CTX))
#define SHA1_Update(CTX, BUF, LEN) \
        sha1_update((CTX), (uchar *)(BUF), (LEN))
#define SHA1_Final(OUT, CTX) sha1_finish((CTX), (OUT))

#define MD5_Init(CTX) md5_starts((CTX))
#define MD5_Update(CTX, BUF, LEN) md5_update((CTX), (uchar *)(BUF), (LEN))
#define MD5_Final(OUT, CTX) md5_finish((CTX), (OUT))

#define AES_set_encrypt_key(KEY, KEYSIZE, CTX) aes_setkey_enc((CTX), (KEY), (KEYSIZE))
#define AES_set_decrypt_key(KEY, KEYSIZE, CTX) aes_setkey_dec((CTX), (KEY), (KEYSIZE))
#define AES_cbc_encrypt(INPUT, OUTPUT, LEN, CTX, IV, MODE) aes_crypt_cbc((CTX), (MODE), (LEN), (IV), (INPUT), (OUTPUT))

/*
   RSA stuff follows.
 */
inline int __RSA_Passthrough(void *output, void *input, int size)
{
    memcpy(output, input, size);
    return size;
}

inline rsa_context *d2i_RSA_PUBKEY(void *ignore, uchar **bufptr, int len)
{
    uchar *buffer = *(uchar **)bufptr;
    rsa_context *rsa;

    /*
       Not a general-purpose parser: only parses public key from *exactly*
         openssl genrsa -out privkey.pem 512 (or 1024)
         openssl rsa -in privkey.pem -out privatekey.der -outform der
         openssl rsa -in privkey.pem -out pubkey.der -outform der -pubout
     */
    if (ignore != 0 || (len != 94 && len != 162))
        return (0);

    rsa = (rsa_context *) malloc(sizeof(rsa_rsa));
    if (rsa == NULL)
        return (0);

    memset(rsa, 0, sizeof(rsa_context));

    if ((len == 94 &&
         mpi_read_binary(&rsa->N, &buffer[25], 64) == 0 &&
         mpi_read_binary(&rsa->E, &buffer[91], 3) == 0) ||
        (len == 162 &&
         mpi_read_binary(&rsa->N, &buffer[29], 128) == 0) &&
        mpi_read_binary(&rsa->E, &buffer[159], 3) == 0) {
        /*
         * key read successfully
         */
        rsa->len = (mpi_msb(&rsa->N) + 7) >> 3;
        return (rsa);
    } else {
        memset(rsa, 0, sizeof(rsa_context));
        free(rsa);
        return (0);
    }
}

#define RSA                     rsa_context
#define RSA_PKCS1_PADDING       1   /* ignored; always encrypt with this */
#define RSA_size(CTX)           (CTX)->len
#define RSA_free(CTX)           rsa_free(CTX)
#define ERR_get_error()        "ERR_get_error() not supported"
#define RSA_blinding_off(IGNORE)

#define d2i_RSAPrivateKey(a, b, c) new rsa_context    /* C++ bleh */

inline int RSA_public_decrypt(int size, uchar *input, uchar *output, RSA * key, int ignore)
{
    int outsize = size;
    if (!rsa_pkcs1_decrypt(key, RSA_PUBLIC, &outsize, input, output))
        return outsize;
    else
        return -1;
}

inline int RSA_private_decrypt(int size, uchar *input, uchar *output, RSA * key, int ignore)
{
    int outsize = size;
    if (!rsa_pkcs1_decrypt(key, RSA_PRIVATE, &outsize, input, output))
        return outsize;
    else
        return -1;
}

inline int RSA_public_encrypt(int size, uchar *input, uchar *output, RSA * key, int ignore)
{
    if (!rsa_pkcs1_encrypt(key, RSA_PUBLIC, size, input, output))
        return RSA_size(key);
    else
        return -1;
}

inline int RSA_private_encrypt(int size, uchar *input, uchar *output, RSA * key, int ignore)
{
    if (!rsa_pkcs1_encrypt(key, RSA_PRIVATE, size, input, output))
        return RSA_size(key);
    else
        return -1;
}

#endif /* openssl.h */
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
