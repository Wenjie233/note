/*
    xtea.h -- An 32-bit implementation of the XTEA algorithm


    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_XTEA_H
#define EST_XTEA_H

#define XTEA_ENCRYPT     1
#define XTEA_DECRYPT     0

/**
    @brief XTEA context structure
 */
typedef struct {
    ulong k[4];     /**< key */
} xtea_context;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          XTEA key schedule
       @param ctx      XTEA context to be initialized
       @param key      the secret key
     */
    PUBLIC void xtea_setup(xtea_context * ctx, uchar key[16]);

    /**
       @brief          XTEA cipher function
       @param ctx      XTEA context
       @param mode     XTEA_ENCRYPT or XTEA_DECRYPT
       @param input    8-byte input block
       @param output   8-byte output block
     */
    PUBLIC void xtea_crypt(xtea_context * ctx, int mode, uchar input[8], uchar output[8]);

    /*
       @brief          Checkup routine
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int xtea_self_test(int verbose);

#ifdef __cplusplus
}
#endif
#endif              /* xtea.h */

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
