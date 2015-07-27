/*
    sha1.h -- 

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_SHA1_H
#define EST_SHA1_H

/**
   @brief SHA-1 context structure
 */
typedef struct {
    ulong total[2];     /**< number of bytes processed  */
    ulong state[5];     /**< intermediate digest state  */
    uchar buffer[64];   /**< data block being processed */
    uchar ipad[64];     /**< HMAC: inner padding        */
    uchar opad[64];     /**< HMAC: outer padding        */
} sha1_context;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          SHA-1 context setup
       @param ctx      context to be initialized
     */
    PUBLIC void sha1_starts(sha1_context * ctx);

    /**
       @brief          SHA-1 process buffer
       @param ctx      SHA-1 context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void sha1_update(sha1_context * ctx, uchar *input, int ilen);

    /**
       @brief          SHA-1 final digest
       @param ctx      SHA-1 context
       @param output   SHA-1 checksum result
     */
    PUBLIC void sha1_finish(sha1_context * ctx, uchar output[20]);

    /**
       @brief          Output = SHA-1( input buffer )
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   SHA-1 checksum result
     */
    PUBLIC void sha1(uchar *input, int ilen, uchar output[20]);

    /**
       @brief          Output = SHA-1( file contents )
       @param path     input file name
       @param output   SHA-1 checksum result
       @return         0 if successful, 1 if fopen failed, or 2 if fread failed
     */
    int sha1_file(char *path, uchar output[20]);

    /**
       @brief          SHA-1 HMAC context setup
       @param ctx      HMAC context to be initialized
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
     */
    PUBLIC void sha1_hmac_starts(sha1_context * ctx, uchar *key, int keylen);

    /**
       @brief          SHA-1 HMAC process buffer
       @param ctx      HMAC context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void sha1_hmac_update(sha1_context * ctx, uchar *input, int ilen);

    /**
       @brief          SHA-1 HMAC final digest
       @param ctx      HMAC context
       @param output   SHA-1 HMAC checksum result
     */
    PUBLIC void sha1_hmac_finish(sha1_context * ctx, uchar output[20]);

    /**
       @brief          Output = HMAC-SHA-1( hmac key, input buffer )
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   HMAC-SHA-1 result
     */
    PUBLIC void sha1_hmac(uchar *key, int keylen, uchar *input, int ilen, uchar output[20]);

    /**
       @brief          Checkup routine
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int sha1_self_test(int verbose);

#ifdef __cplusplus
}
#endif
#endif              /* sha1.h */

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
