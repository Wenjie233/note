/*
    sha4.h -- FIPS-180-2 compliant SHA-384/512 implementation

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_SHA4_H
#define EST_SHA4_H

#if defined(_MSC_VER) || defined(__WATCOMC__)
    #define UL64(x) x##ui64
    #define int64 __int64
#else
    #define UL64(x) x##ULL
    #define int64 long long
#endif

/**
 * @brief          SHA-512 context structure
 */
typedef struct {
    uint64 total[2];    /**< number of bytes processed  */
    uint64 state[8];    /**< intermediate digest state  */
    uchar buffer[128];  /**< data block being processed */
    uchar ipad[128];    /**< HMAC: inner padding        */
    uchar opad[128];    /**< HMAC: outer padding        */
    int is384;          /**< 0 => SHA-512, else SHA-384 */
} sha4_context;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          SHA-512 context setup
       @param ctx      context to be initialized
       @param is384    0 = use SHA512, 1 = use SHA384
     */
    PUBLIC void sha4_starts(sha4_context * ctx, int is384);

    /**
       @brief          SHA-512 process buffer
       @param ctx      SHA-512 context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void sha4_update(sha4_context * ctx, uchar *input, int ilen);

    /**
       @brief          SHA-512 final digest
       @param ctx      SHA-512 context
       @param output   SHA-384/512 checksum result
     */
    PUBLIC void sha4_finish(sha4_context * ctx, uchar output[64]);

    /**
       @brief          Output = SHA-512(input buffer)
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   SHA-384/512 checksum result
       @param is384    0 = use SHA512, 1 = use SHA384
     */
    PUBLIC void sha4(uchar *input, int ilen, uchar output[64], int is384);

    /**
       @brief          Output = SHA-512(file contents)
       @param path     input file name
       @param output   SHA-384/512 checksum result
       @param is384    0 = use SHA512, 1 = use SHA384
       @return         0 if successful, 1 if fopen failed, or 2 if fread failed
     */
    PUBLIC int sha4_file(char *path, uchar output[64], int is384);

    /**
       @brief          SHA-512 HMAC context setup
       @param ctx      HMAC context to be initialized
       @param is384    0 = use SHA512, 1 = use SHA384
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
     */
    PUBLIC void sha4_hmac_starts(sha4_context * ctx, uchar *key, int keylen, int is384);

    /**
       @brief          SHA-512 HMAC process buffer
       @param ctx      HMAC context
       @param input    buffer holding the  data
       @param ilen     length of the input data
     */
    PUBLIC void sha4_hmac_update(sha4_context * ctx, uchar *input, int ilen);

    /**
       @brief          SHA-512 HMAC final digest
       @param ctx      HMAC context
       @param output   SHA-384/512 HMAC checksum result
     */
    PUBLIC void sha4_hmac_finish(sha4_context * ctx, uchar output[64]);

    /**
       @brief          Output = HMAC-SHA-512(hmac key, input buffer)
       @param key      HMAC secret key
       @param keylen   length of the HMAC key
       @param input    buffer holding the  data
       @param ilen     length of the input data
       @param output   HMAC-SHA-384/512 result
       @param is384    0 = use SHA512, 1 = use SHA384
     */
    PUBLIC void sha4_hmac(uchar *key, int keylen, uchar *input, int ilen, uchar output[64], int is384);

    /**
       @brief          Checkup routine
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int sha4_self_test(int verbose);

#ifdef __cplusplus
}
#endif
#endif              /* sha4.h */

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
