/*
    des.h -- Des Cipher

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_DES_H
#define EST_DES_H

#define DES_ENCRYPT     1
#define DES_DECRYPT     0

/**
   @brief DES context structure
 */
typedef struct {
    int     mode;       /**< encrypt/decrypt */
    ulong   sk[32];     /**< DES subkeys */
} des_context;


/**
   @brief Triple-DES context structure
 */
typedef struct {
    int     mode;       /**< encrypt/decrypt */
    ulong   sk[96];     /**< 3DES subkeys */
} des3_context;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          DES key schedule (56-bit, encryption)
       @param ctx      DES context to be initialized
       @param key      8-byte secret key
     */
    PUBLIC void des_setkey_enc(des_context *ctx, uchar key[8]);

    /**
       @brief          DES key schedule (56-bit, decryption)
       @param ctx      DES context to be initialized
       @param key      8-byte secret key
     */
    PUBLIC void des_setkey_dec(des_context *ctx, uchar key[8]);

    /**
       @brief          Triple-DES key schedule (112-bit, encryption)
       @param ctx      3DES context to be initialized
       @param key      16-byte secret key
     */
    PUBLIC void des3_set2key_enc(des3_context *ctx, uchar key[16]);

    /**
       @brief          Triple-DES key schedule (112-bit, decryption)
       @param ctx      3DES context to be initialized
       @param key      16-byte secret key
     */
    PUBLIC void des3_set2key_dec(des3_context *ctx, uchar key[16]);

    /**
       @brief          Triple-DES key schedule (168-bit, encryption)
       @param ctx      3DES context to be initialized
       @param key      24-byte secret key
     */
    PUBLIC void des3_set3key_enc(des3_context *ctx, uchar key[24]);

    /**
       @brief          Triple-DES key schedule (168-bit, decryption)
       @param ctx      3DES context to be initialized
       @param key      24-byte secret key
     */
    PUBLIC void des3_set3key_dec(des3_context *ctx, uchar key[24]);

    /**
       @brief          DES-ECB block encryption/decryption
       @param ctx      DES context
       @param input    64-bit input block
       @param output   64-bit output block
     */
    PUBLIC void des_crypt_ecb(des_context *ctx, uchar input[8], uchar output[8]);

    /**
       @brief          DES-CBC buffer encryption/decryption
       @param ctx      DES context
       @param mode     DES_ENCRYPT or DES_DECRYPT
       @param length   length of the input data
       @param iv       initialization vector (updated after use)
       @param input    buffer holding the input data
       @param output   buffer holding the output data
     */
    PUBLIC void des_crypt_cbc(des_context *ctx, int mode, int length, uchar iv[8], uchar *input, uchar *output);

    /**
       @brief          3DES-ECB block encryption/decryption
       @param ctx      3DES context
       @param input    64-bit input block
       @param output   64-bit output block
     */
    PUBLIC void des3_crypt_ecb(des3_context *ctx, uchar input[8], uchar output[8]);

    /**
       @brief          3DES-CBC buffer encryption/decryption
       @param ctx      3DES context
       @param mode     DES_ENCRYPT or DES_DECRYPT
       @param length   length of the input data
       @param iv       initialization vector (updated after use)
       @param input    buffer holding the input data
       @param output   buffer holding the output data
     */
    PUBLIC void des3_crypt_cbc(des3_context *ctx, int mode, int length, uchar iv[8], uchar *input, uchar *output);

#if UNUSED
    /*
       @brief          Checkup routine
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int des_self_test(int verbose);
#endif

#ifdef __cplusplus
}
#endif
#endif              /* des.h */

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
