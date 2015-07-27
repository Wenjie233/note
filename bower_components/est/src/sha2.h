/*
    sha2.h -- FIPS-180-2 compliant SHA-256 implementation


    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_SHA2_H
#define EST_SHA2_H

/**
   @brief SHA-256 context structure
 */
typedef struct {
    ulong total[2];     /**< number of bytes processed  */
    ulong state[8];     /**< intermediate digest state  */
    uchar buffer[64];   /**< data block being processed */
    uchar ipad[64];     /**< HMAC: inner padding        */
    uchar opad[64];     /**< HMAC: outer padding        */
    int is224;          /**< 0 => SHA-256, else SHA-224 */
} sha2_context;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          SHA-256 context setup
       @param ctx      context to be initialized
       @param is224    0 = use SHA256, 1 = use SHA224
     */
    PUBLIC void sha2_starts(sha2_context *ctx, int is224);

    /**
       @brief          SHA-256 process buffer
       @param ctx      SHA-256 context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void sha2_update(sha2_context *ctx, uchar *input, int ilen);

    /**
       @brief          SHA-256 final digest
       @param ctx      SHA-256 context
       @param output   SHA-224/256 checksum result
     */
    PUBLIC void sha2_finish(sha2_context *ctx, uchar output[32]);

    /**
       @brief          Output = SHA-256(input buffer)
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   SHA-224/256 checksum result
       @param is224    0 = use SHA256, 1 = use SHA224
     */
    PUBLIC void sha2(uchar *input, int ilen, uchar output[32], int is224);

    /**
       @brief          Output = SHA-256(file contents)
       @param path     input file name
       @param output   SHA-224/256 checksum result
       @param is224    0 = use SHA256, 1 = use SHA224
       @return         0 if successful, 1 if fopen failed, or 2 if fread failed
     */
    PUBLIC int sha2_file(char *path, uchar output[32], int is224);

    /**
       @brief          SHA-256 HMAC context setup
       @param ctx      HMAC context to be initialized
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
       @param is224    0 = use SHA256, 1 = use SHA224
     */
    PUBLIC void sha2_hmac_starts(sha2_context *ctx, uchar *key, int keylen, int is224);

    /**
       @brief          SHA-256 HMAC process buffer
       @param ctx      HMAC context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void sha2_hmac_update(sha2_context *ctx, uchar *input, int ilen);

    /**
       @brief          SHA-256 HMAC final digest
       @param ctx      HMAC context
       @param output   SHA-224/256 HMAC checksum result
     */
    PUBLIC void sha2_hmac_finish(sha2_context *ctx, uchar output[32]);

    /**
       @brief          Output = HMAC-SHA-256(hmac key, input buffer)
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   HMAC-SHA-224/256 result
       @param is224    0 = use SHA256, 1 = use SHA224
     */
    PUBLIC void sha2_hmac(uchar *key, int keylen, uchar *input, int ilen, uchar output[32], int is224);

    /**
       @brief          Checkup routine
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int sha2_self_test(int verbose);

#ifdef __cplusplus
}
#endif
#endif              /* sha2.h */

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
