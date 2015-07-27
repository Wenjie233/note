/*
    md5.h c -- RFC 1321 compliant MD5 implementation

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_MD5_H
#define EST_MD5_H

/**
   @brief          MD5 context structure
 */
typedef struct {
    ulong total[2];     /**< number of bytes processed  */
    ulong state[4];     /**< intermediate digest state  */
    uchar buffer[64];   /**< data block being processed */
    uchar ipad[64];     /**< HMAC: inner padding        */
    uchar opad[64];     /**< HMAC: outer padding        */
} md5_context;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          MD5 context setup
       @param ctx      context to be initialized
     */
    PUBLIC void md5_starts(md5_context * ctx);

    /**
       @brief          MD5 process buffer
       @param ctx      MD5 context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void md5_update(md5_context * ctx, uchar *input, int ilen);

    /**
       @brief          MD5 final digest
       @param ctx      MD5 context
       @param output   MD5 checksum result
     */
    PUBLIC void md5_finish(md5_context * ctx, uchar output[16]);

    /**
       @brief          Output = MD5( input buffer )
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   MD5 checksum result
     */
    PUBLIC void md5(uchar *input, int ilen, uchar output[16]);

    /**
       @brief          Output = MD5( file contents )
       @param path     input file name
       @param output   MD5 checksum result
      
       @return         0 if successful, 1 if fopen failed, or 2 if fread failed
     */
    PUBLIC int md5_file(char *path, uchar output[16]);

    /**
       @brief          MD5 HMAC context setup
       @param ctx      HMAC context to be initialized
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
     */
    PUBLIC void md5_hmac_starts(md5_context * ctx, uchar *key, int keylen);

    /**
       @brief          MD5 HMAC process buffer
       @param ctx      HMAC context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void md5_hmac_update(md5_context * ctx, uchar *input, int ilen);

    /**
       @brief          MD5 HMAC final digest
       @param ctx      HMAC context
       @param output   MD5 HMAC checksum result
     */
    PUBLIC void md5_hmac_finish(md5_context * ctx, uchar output[16]);

    /**
       @brief          Output = HMAC-MD5( hmac key, input buffer )
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   HMAC-MD5 result
     */
    PUBLIC void md5_hmac(uchar *key, int keylen, uchar *input, int ilen, uchar output[16]);

    /**
       @brief          Checkup routine
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int md5_self_test(int verbose);

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
