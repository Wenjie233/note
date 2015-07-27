/*
    dhm.c -- Diffie-Hellman-Merkle key exchange

    http://www.cacr.math.uwaterloo.ca/hac/ (chapter 12)
 
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#include "est.h"

#if ME_EST_DHM
/*
   helper to validate the mpi size and import it
 */
static int dhm_read_bignum(mpi * X, uchar **p, uchar *end)
{
    int ret, n;

    if (end - *p < 2) {
        return EST_ERR_DHM_BAD_INPUT_DATA;
    }
    n = ((*p)[0] << 8) | (*p)[1];
    (*p) += 2;

    if ((int)(end - *p) < n) {
        return EST_ERR_DHM_BAD_INPUT_DATA;
    }
    if ((ret = mpi_read_binary(X, *p, n)) != 0) {
        return EST_ERR_DHM_READ_PARAMS_FAILED | ret;
    }
    (*p) += n;
    return 0;
}


/*
    Parse the ServerKeyExchange parameters
 */
int dhm_read_params(dhm_context *ctx, uchar **p, uchar *end)
{
    int ret, n;

    memset(ctx, 0, sizeof(dhm_context));

    if ((ret = dhm_read_bignum(&ctx->P, p, end)) != 0 ||
        (ret = dhm_read_bignum(&ctx->G, p, end)) != 0 ||
        (ret = dhm_read_bignum(&ctx->GY, p, end)) != 0)
        return ret;

    ctx->len = mpi_size(&ctx->P);

    if (end - *p < 2)
        return EST_ERR_DHM_BAD_INPUT_DATA;

    n = ((*p)[0] << 8) | (*p)[1];
    (*p) += 2;

    if (end != *p + n)
        return EST_ERR_DHM_BAD_INPUT_DATA;
    return 0;
}


/*
   Setup and write the ServerKeyExchange parameters
 */
int dhm_make_params(dhm_context *ctx, int x_size, uchar *output, int *olen, int (*f_rng) (void *), void *p_rng)
{
    int i, ret, n, n1, n2, n3;
    uchar *p;

    /*
        Generate X and calculate GX = G^X mod P
     */
    n = x_size / sizeof(t_int);
    MPI_CHK(mpi_grow(&ctx->X, n));
    MPI_CHK(mpi_lset(&ctx->X, 0));

    n = x_size >> 3;
    p = (uchar *)ctx->X.p;
    for (i = 0; i < n; i++)
        *p++ = (uchar)f_rng(p_rng);

    while (mpi_cmp_mpi(&ctx->X, &ctx->P) >= 0)
        mpi_shift_r(&ctx->X, 1);

    MPI_CHK(mpi_exp_mod(&ctx->GX, &ctx->G, &ctx->X, &ctx->P, &ctx->RP));

    /*
        Export P, G, GX
     */
#define DHM_MPI_EXPORT(X,n) \
    MPI_CHK(mpi_write_binary( X, p + 2, n ) ); \
    *p++ = (uchar)( n >> 8 ); \
    *p++ = (uchar)( n      ); p += n;

    n1 = mpi_size(&ctx->P);
    n2 = mpi_size(&ctx->G);
    n3 = mpi_size(&ctx->GX);

    p = output;
    DHM_MPI_EXPORT(&ctx->P, n1);
    DHM_MPI_EXPORT(&ctx->G, n2);
    DHM_MPI_EXPORT(&ctx->GX, n3);

    *olen = p - output;
    ctx->len = n1;

cleanup:
    if (ret != 0)
        return ret | EST_ERR_DHM_MAKE_PARAMS_FAILED;
    return 0;
}


/*
   Import the peer's public value G^Y
 */
int dhm_read_public(dhm_context *ctx, uchar *input, int ilen)
{
    int ret;

    if (ctx == NULL || ilen < 1 || ilen > ctx->len) {
        return EST_ERR_DHM_BAD_INPUT_DATA;
    }
    if ((ret = mpi_read_binary(&ctx->GY, input, ilen)) != 0) {
        return EST_ERR_DHM_READ_PUBLIC_FAILED | ret;
    }
    return 0;
}


/*
   Create own private value X and export G^X
 */
int dhm_make_public(dhm_context *ctx, int x_size, uchar *output, int olen, int (*f_rng) (void *), void *p_rng)
{
    int ret, i, n;
    uchar *p;

    if (ctx == NULL || olen < 1 || olen > ctx->len) {
        return EST_ERR_DHM_BAD_INPUT_DATA;
    }

    /*
        generate X and calculate GX = G^X mod P
     */
    n = x_size / sizeof(t_int);
    MPI_CHK(mpi_grow(&ctx->X, n));
    MPI_CHK(mpi_lset(&ctx->X, 0));

    n = x_size >> 3;
    p = (uchar *)ctx->X.p;
    for (i = 0; i < n; i++) {
        *p++ = (uchar)f_rng(p_rng);
    }
    while (mpi_cmp_mpi(&ctx->X, &ctx->P) >= 0) {
        mpi_shift_r(&ctx->X, 1);
    }
    MPI_CHK(mpi_exp_mod(&ctx->GX, &ctx->G, &ctx->X, &ctx->P, &ctx->RP));
    MPI_CHK(mpi_write_binary(&ctx->GX, output, olen));

cleanup:
    if (ret != 0) {
        return EST_ERR_DHM_MAKE_PUBLIC_FAILED | ret;
    }
    return 0;
}


/*
    Derive and export the shared secret (G^Y)^X mod P
 */
int dhm_calc_secret(dhm_context *ctx, uchar *output, int *olen)
{
    int ret;

    if (ctx == NULL || *olen < ctx->len) {
        return EST_ERR_DHM_BAD_INPUT_DATA;
    }
    MPI_CHK(mpi_exp_mod(&ctx->K, &ctx->GY, &ctx->X, &ctx->P, &ctx->RP));
    *olen = mpi_size(&ctx->K);
    MPI_CHK(mpi_write_binary(&ctx->K, output, *olen));

cleanup:
    if (ret != 0) {
        return EST_ERR_DHM_CALC_SECRET_FAILED | ret;
    }
    return 0;
}


/*
    Free the components of a DHM key
 */
void dhm_free(dhm_context * ctx)
{
    mpi_free(&ctx->RP, &ctx->K, &ctx->GY, &ctx->GX, &ctx->X, &ctx->G, &ctx->P, NULL);
}

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
