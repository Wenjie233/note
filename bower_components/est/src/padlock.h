/*
    padlock.h -- Via padlock support

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_PADLOCK_H
#define EST_PADLOCK_H

#if (defined(__GNUC__) && defined(__i386__))
#ifndef EST_HAVE_X86
#define EST_HAVE_X86
#endif

#define PADLOCK_RNG 0x000C
#define PADLOCK_ACE 0x00C0
#define PADLOCK_PHE 0x0C00
#define PADLOCK_PMM 0x3000

#define PADLOCK_ALIGN16(x) (ulong *) (16 + ((long) x & ~15))

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          PadLock detection routine
       @return         1 if CPU has support for the feature, 0 otherwise
     */
    PUBLIC int padlock_supports(int feature);

    /**
       @brief          PadLock AES-ECB block en(de)cryption
       @param ctx      AES context
       @param mode     AES_ENCRYPT or AES_DECRYPT
       @param input    16-byte input block
       @param output   16-byte output block
       @return         0 if success, 1 if operation failed
     */
    PUBLIC int padlock_xcryptecb(aes_context *ctx, int mode, uchar input[16], uchar output[16]);

    /**
       @brief          PadLock AES-CBC buffer en(de)cryption
       @param ctx      AES context
       @param mode     AES_ENCRYPT or AES_DECRYPT
       @param length   length of the input data
       @param iv       initialization vector (updated after use)
       @param input    buffer holding the input data
       @param output   buffer holding the output data
       @return         0 if success, 1 if operation failed
     */
    PUBLIC int padlock_xcryptcbc(aes_context *ctx, int mode, int length, uchar iv[16], uchar *input, uchar *output);

#ifdef __cplusplus
}
#endif
#endif              /* HAVE_X86  */
#endif              /* padlock.h */

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
