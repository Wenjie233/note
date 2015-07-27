/*
    dhm.h -- Diffie-Helman Support

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_DHM_H
#define EST_DHM_H

#define EST_ERR_DHM_BAD_INPUT_DATA                    -0x0480
#define EST_ERR_DHM_READ_PARAMS_FAILED                -0x0490
#define EST_ERR_DHM_MAKE_PARAMS_FAILED                -0x04A0
#define EST_ERR_DHM_READ_PUBLIC_FAILED                -0x04B0
#define EST_ERR_DHM_MAKE_PUBLIC_FAILED                -0x04C0
#define EST_ERR_DHM_CALC_SECRET_FAILED                -0x04D0

typedef struct {
    int len;        /**<  size(P) in chars  */
    mpi P;          /**<  prime modulus     */
    mpi G;          /**<  generator         */
    mpi X;          /**<  secret value      */
    mpi GX;         /**<  self = G^X mod P  */
    mpi GY;         /**<  peer = G^Y mod P  */
    mpi K;          /**<  key = GY^X mod P  */
    mpi RP;         /**<  cached R^2 mod P  */
} dhm_context;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          Parse the ServerKeyExchange parameters
       @param ctx      DHM context
       @param p        &(start of input buffer)
       @param end      end of buffer
       @return         0 if successful, or an EST_ERR_DHM_XXX error code
     */
    PUBLIC int dhm_read_params(dhm_context *ctx, uchar **p, uchar *end);

    /**
       @brief          Setup and write the ServerKeyExchange parameters
       @param ctx      DHM context
       @param x_size   private value size in bits
       @param output   destination buffer
       @param olen     number of chars written
       @param f_rng    RNG function
       @param p_rng    RNG parameter
       @note           This function assumes that ctx->P and ctx->G have already been properly set (for example
                       using mpi_read_string or mpi_read_binary).
       @return         0 if successful, or an EST_ERR_DHM_XXX error code
     */
    PUBLIC int dhm_make_params(dhm_context *ctx, int s_size, uchar *output, int *olen, int (*f_rng) (void *), void *p_rng);

    /**
       @brief          Import the peer's public value G^Y
       @param ctx      DHM context
       @param input    input buffer
       @param ilen     size of buffer
       @return         0 if successful, or an EST_ERR_DHM_XXX error code
     */
    PUBLIC int dhm_read_public(dhm_context *ctx, uchar *input, int ilen);

    /**
       @brief          Create own private value X and export G^X
       @param ctx      DHM context
       @param x_size   private value size in bits
       @param output   destination buffer
       @param olen     must be equal to ctx->P.len
       @param f_rng    RNG function
       @param p_rng    RNG parameter
       @return         0 if successful, or an EST_ERR_DHM_XXX error code
     */
    PUBLIC int dhm_make_public(dhm_context *ctx, int s_size, uchar *output, int olen, int (*f_rng) (void *), void *p_rng);

    /**
       @brief          Derive and export the shared secret (G^Y)^X mod P
       @param ctx      DHM context
       @param output   destination buffer
       @param olen     number of chars written
       @return         0 if successful, or an EST_ERR_DHM_XXX error code
     */
    PUBLIC int dhm_calc_secret(dhm_context *ctx, uchar *output, int *olen);

    /*
       @brief          Free the components of a DHM key
     */
    PUBLIC void dhm_free(dhm_context *ctx);

#if UNUSED
    /**
       @brief          Checkup routine
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int dhm_self_test(int verbose);
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
