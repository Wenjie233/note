/*
    md2.h -- RFC 1115/1319 compliant MD2 implementation

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_MD2_H
#define EST_MD2_H

/**
   @brief MD2 context structure
 */
typedef struct {
    uchar   cksum[16];      /**< checksum of the data block */
    uchar   state[48];      /**< intermediate digest state  */
    uchar   buffer[16];     /**< data block being processed */

    uchar   ipad[64];       /**< HMAC: inner padding        */
    uchar   opad[64];       /**< HMAC: outer padding        */
    int     left;           /**< amount of data in buffer   */
} md2_context;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          MD2 context setup
       @param ctx      context to be initialized
     */
    PUBLIC void md2_starts(md2_context * ctx);

    /**
       @brief          MD2 process buffer
       @param ctx      MD2 context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void md2_update(md2_context * ctx, uchar *input, int ilen);

    /**
       @brief          MD2 final digest
       @param ctx      MD2 context
     * @param output   MD2 checksum result
     */
    PUBLIC void md2_finish(md2_context * ctx, uchar output[16]);

    /**
       @brief          Output = MD2( input buffer )
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   MD2 checksum result
     */
    PUBLIC void md2(uchar *input, int ilen, uchar output[16]);

    /**
       @brief          Output = MD2( file contents )
       @param path     input file name
       @param output   MD2 checksum result
       @return         0 if successful, 1 if fopen failed, or 2 if fread failed
     */
    PUBLIC int md2_file(char *path, uchar output[16]);

    /**
       @brief          MD2 HMAC context setup
       @param ctx      HMAC context to be initialized
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
     */
    PUBLIC void md2_hmac_starts(md2_context * ctx, uchar *key, int keylen);

    /**
       @brief          MD2 HMAC process buffer
       @param ctx      HMAC context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void md2_hmac_update(md2_context * ctx, uchar *input, int ilen);

    /**
       @brief          MD2 HMAC final digest
       @param ctx      HMAC context
       @param output   MD2 HMAC checksum result
     */
    PUBLIC void md2_hmac_finish(md2_context * ctx, uchar output[16]);

    /**
       @brief          Output = HMAC-MD2( hmac key, input buffer )
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   HMAC-MD2 result
     */
    PUBLIC void md2_hmac(uchar *key, int keylen, uchar *input, int ilen, uchar output[16]);

    /**
       @brief          Checkup routine
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int md2_self_test(int verbose);

#ifdef __cplusplus
}
#endif
#endif              /* md2.h */

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
