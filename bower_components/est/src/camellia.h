/*
    camellia.h -- Camellia Cipher

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_CAMELLIA_H
#define EST_CAMELLIA_H

#define CAMELLIA_ENCRYPT     1
#define CAMELLIA_DECRYPT     0

/**
   @brief CAMELLIA context structure
 */
typedef struct {
    int     nr;         /**< number of rounds */
    ulong   rk[68];     /**< CAMELLIA round keys */
} camellia_context;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          CAMELLIA key schedule (encryption)
       @param ctx      CAMELLIA context to be initialized
       @param key      encryption key
       @param keysize  must be 128, 192 or 256
     */
    PUBLIC void camellia_setkey_enc(camellia_context *ctx, uchar *key, int keysize);

    /**
       @brief          CAMELLIA key schedule (decryption)
       @param ctx      CAMELLIA context to be initialized
       @param key      decryption key
       @param keysize  must be 128, 192 or 256
     */
    PUBLIC void camellia_setkey_dec(camellia_context *ctx, uchar *key, int keysize);

    /**
       @brief          CAMELLIA-ECB block encryption/decryption
       @param ctx      CAMELLIA context
       @param mode     CAMELLIA_ENCRYPT or CAMELLIA_DECRYPT
       @param input    16-byte input block
       @param output   16-byte output block
     */
    PUBLIC void camellia_crypt_ecb(camellia_context *ctx, int mode, uchar input[16], uchar output[16]);

    /**
       @brief          CAMELLIA-CBC buffer encryption/decryption
       @param ctx      CAMELLIA context
       @param mode     CAMELLIA_ENCRYPT or CAMELLIA_DECRYPT
       @param length   length of the input data
       @param iv       initialization vector (updated after use)
       @param input    buffer holding the input data
       @param output   buffer holding the output data
     */
    PUBLIC void camellia_crypt_cbc(camellia_context *ctx, int mode, int length, uchar iv[16], uchar *input, uchar *output);

    /**
       @brief          CAMELLIA-CFB128 buffer encryption/decryption
       @param ctx      CAMELLIA context
       @param mode     CAMELLIA_ENCRYPT or CAMELLIA_DECRYPT
       @param length   length of the input data
       @param iv_off   offset in IV (updated after use)
       @param iv       initialization vector (updated after use)
       @param input    buffer holding the input data
       @param output   buffer holding the output data
     */
    PUBLIC void camellia_crypt_cfb128(camellia_context *ctx, int mode, int length, int *iv_off, uchar iv[16], 
            uchar *input, uchar *output);

#if UNUSED
    /**
       @brief          Checkup routine
      
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int camellia_self_test(int verbose);
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
