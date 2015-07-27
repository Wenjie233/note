/*
    aes.h -- 

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_AES_H
#define EST_AES_H

#define AES_ENCRYPT     1
#define AES_DECRYPT     0

/**
    @brief AES context structure
 */
typedef struct {
    int     nr;         /**< number of rounds */
    ulong   *rk;        /**< AES round keys */
    ulong   buf[68];    /**<  unaligned data */
} aes_context;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          AES key schedule (encryption)
       @param ctx      AES context to be initialized
       @param key      encryption key
       @param keysize  must be 128, 192 or 256
     */
    PUBLIC void aes_setkey_enc(aes_context *ctx, uchar *key, int keysize);

    /**
       @brief          AES key schedule (decryption)
       @param ctx      AES context to be initialized
       @param key      decryption key
       @param keysize  must be 128, 192 or 256
     */
    PUBLIC void aes_setkey_dec(aes_context *ctx, uchar *key, int keysize);

    /**
       @brief          AES-ECB block encryption/decryption
       @param ctx      AES context
       @param mode     AES_ENCRYPT or AES_DECRYPT
       @param input    16-byte input block
       @param output   16-byte output block
     */
    PUBLIC void aes_crypt_ecb(aes_context *ctx, int mode, uchar input[16], uchar output[16]);

    /**
       @brief          AES-CBC buffer encryption/decryption
       @param ctx      AES context
       @param mode     AES_ENCRYPT or AES_DECRYPT
       @param length   length of the input data
       @param iv       initialization vector (updated after use)
       @param input    buffer holding the input data
       @param output   buffer holding the output data
     */
    PUBLIC void aes_crypt_cbc(aes_context *ctx, int mode, int length, uchar iv[16], uchar *input, uchar *output);

    /**
       @brief          AES-CFB128 buffer encryption/decryption
       @param ctx      AES context
       @param mode     AES_ENCRYPT or AES_DECRYPT
       @param length   length of the input data
       @param iv_off   offset in IV (updated after use)
       @param iv       initialization vector (updated after use)
       @param input    buffer holding the input data
       @param output   buffer holding the output data
     */
    PUBLIC void aes_crypt_cfb128(aes_context *ctx, int mode, int length, int *iv_off, uchar iv[16], 
            uchar *input, uchar *output);

#if UNUSED
    /**
       @brief          Checkup routine
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int aes_self_test(int verbose);
#endif

#ifdef __cplusplus
}
#endif
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
