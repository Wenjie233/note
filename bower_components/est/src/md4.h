/*
    md4.h -- RFC 1186/1320 compliant MD4 implementation

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_MD4_H
#define EST_MD4_H

/**
    @brief          MD4 context structure
 */
typedef struct {
    ulong   total[2];       /**< number of bytes processed  */
    ulong   state[4];       /**< intermediate digest state  */
    uchar   buffer[64];     /**< data block being processed */
    uchar   ipad[64];       /**< HMAC: inner padding        */
    uchar   opad[64];       /**< HMAC: outer padding        */
} md4_context;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          MD4 context setup
       @param ctx      context to be initialized
     */
    PUBLIC void md4_starts(md4_context * ctx);

    /**
       @brief          MD4 process buffer
       @param ctx      MD4 context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void md4_update(md4_context * ctx, uchar *input, int ilen);

    /**
       @brief          MD4 final digest
       @param ctx      MD4 context
       @param output   MD4 checksum result
     */
    PUBLIC void md4_finish(md4_context * ctx, uchar output[16]);

    /**
       @brief          Output = MD4( input buffer )
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   MD4 checksum result
     */
    PUBLIC void md4(uchar *input, int ilen, uchar output[16]);

    /**
       @brief          Output = MD4( file contents )
       @param path     input file name
       @param output   MD4 checksum result
       @return         0 if successful, 1 if fopen failed, or 2 if fread failed
     */
    PUBLIC int md4_file(char *path, uchar output[16]);

    /**
       @brief          MD4 HMAC context setup
       @param ctx      HMAC context to be initialized
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
     */
    PUBLIC void md4_hmac_starts(md4_context * ctx, uchar *key, int keylen);

    /**
       @brief          MD4 HMAC process buffer
       @param ctx      HMAC context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void md4_hmac_update(md4_context * ctx, uchar *input, int ilen);

    /**
       @brief          MD4 HMAC final digest
       @param ctx      HMAC context
       @param output   MD4 HMAC checksum result
     */
    PUBLIC void md4_hmac_finish(md4_context * ctx, uchar output[16]);

    /**
       @brief          Output = HMAC-MD4( hmac key, input buffer )
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   HMAC-MD4 result
     */
    PUBLIC void md4_hmac(uchar *key, int keylen, uchar *input, int ilen, uchar output[16]);

    /**
       @brief          Checkup routine
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int md4_self_test(int verbose);

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
