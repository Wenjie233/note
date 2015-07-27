/*
    bignum.h -- Bit number support

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_BIGNUM_H
#define EST_BIGNUM_H

#define EST_ERR_MPI_FILE_IO_ERROR                     -0x0002
#define EST_ERR_MPI_BAD_INPUT_DATA                    -0x0004
#define EST_ERR_MPI_INVALID_CHARACTER                 -0x0006
#define EST_ERR_MPI_BUFFER_TOO_SMALL                  -0x0008
#define EST_ERR_MPI_NEGATIVE_VALUE                    -0x000A
#define EST_ERR_MPI_DIVISION_BY_ZERO                  -0x000C
#define EST_ERR_MPI_NOT_ACCEPTABLE                    -0x000E

#define MPI_CHK(f) if( ( ret = f ) != 0 ) goto cleanup

/*
   Define the base integer type, architecture-wise
 */
#if ME_WORDSIZE == 8
    typedef uchar t_int;
    typedef ushort t_dbl;
#elif ME_WORDSIZE == 16
    typedef ushort t_int;
    typedef ulong t_dbl;
#else
    typedef ulong t_int;
    #if defined(_MSC_VER) && defined(_M_IX86)
        typedef unsigned __int64 t_dbl;
    #else
        #if defined(__amd64__) || defined(__x86_64__) || defined(__ppc64__) || defined(__powerpc64__) || defined(__mips64) || \
                defined(__ia64__)  || defined(__alpha__)
            typedef uint t_dbl __attribute__ ((mode(TI)));
        #else
            typedef unsigned long long t_dbl;
            #define ME_USE_LONG_LONG 1
        #endif
    #endif
#endif

/**
   @brief MPI structure
 */
typedef struct {
    int     s;          /**< integer sign */
    int     n;          /**< total # of limbs  */
    t_int   *p;         /**< pointer to limbs  */
} mpi;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief Initialize one or more mpi
     */
    PUBLIC void mpi_init(mpi *X, ...);

    /**
       @brief Unallocate one or more mpi
     */
    PUBLIC void mpi_free(mpi *X, ...);

    /**
       @brief          Enlarge to the specified number of limbs
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_grow(mpi *X, int nblimbs);

    /**
       @brief          Copy the contents of Y into X
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_copy(mpi *X, mpi *Y);

    /**
       @brief          Swap the contents of X and Y
     */
    PUBLIC void mpi_swap(mpi *X, mpi *Y);

    /**
       @brief          Set value from integer
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_lset(mpi *X, int z);

    /**
       @brief          Return the number of least significant bits
     */
    PUBLIC int mpi_lsb(mpi *X);

    /**
       @brief          Return the number of most significant bits
     */
    PUBLIC int mpi_msb(mpi *X);

    /**
       @brief          Return the total size in bytes
     */
    PUBLIC int mpi_size(mpi *X);

    /**
       @brief          Import from an ASCII string
       @param X        destination mpi
       @param radix    input numeric base
       @param s        null-terminated string buffer
       @return         0 if successful, or an EST_ERR_MPI_XXX error code
     */
    PUBLIC int mpi_read_string(mpi *X, int radix, char *s);

    /**
       @brief          Export into an ASCII string
       @param X        source mpi
       @param radix    output numeric base
       @param s        string buffer
       @param slen     string buffer size
       @return         0 if successful, or an EST_ERR_MPI_XXX error code
       @note           Call this function with *slen = 0 to obtain the minimum required buffer size in *slen.
     */
    PUBLIC int mpi_write_string(mpi *X, int radix, char *s, int *slen);

    /**
       @brief          Read X from an opened file
       @param X        destination mpi
       @param radix    input numeric base
       @param fin      input file handle
       @return         0 if successful, or an EST_ERR_MPI_XXX error code
     */
    PUBLIC int mpi_read_file(mpi *X, int radix, FILE * fin);

    /**
       @brief          Write X into an opened file, or stdout
       @param p        prefix, can be NULL
       @param X        source mpi
       @param radix    output numeric base
       @param fout     output file handle
       @return         0 if successful, or an EST_ERR_MPI_XXX error code
       @note           Set fout == NULL to print X on the console.
     */
    PUBLIC int mpi_write_file(char *p, mpi *X, int radix, FILE * fout);

    /**
       @brief          Import X from unsigned binary data, big endian
       @param X        destination mpi
       @param buf      input buffer
       @param buflen   input buffer size
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_read_binary(mpi *X, uchar *buf, int buflen);

    /**
       @brief          Export X into unsigned binary data, big endian
       @param X        source mpi
       @param buf      output buffer
       @param buflen   output buffer size
       @return         0 if successful, EST_ERR_MPI_BUFFER_TOO_SMALL if buf isn't large enough
       @note           Call this function with *buflen = 0 to obtain the minimum required buffer size in *buflen.
     */
    PUBLIC int mpi_write_binary(mpi *X, uchar *buf, int buflen);

    /**
       @brief          Left-shift: X <<= count
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_shift_l(mpi *X, int count);

    /**
       @brief          Right-shift: X >>= count
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_shift_r(mpi *X, int count);

    /**
       @brief          Compare unsigned values
       @return         1 if |X| is greater than |Y|,
                      -1 if |X| is lesser  than |Y| or
                       0 if |X| is equal to |Y|
     */
    PUBLIC int mpi_cmp_abs(mpi *X, mpi *Y);

    /**
       @brief          Compare signed values
       @return         1 if X is greater than Y,
                      -1 if X is lesser  than Y or
                       0 if X is equal to Y
     */
    PUBLIC int mpi_cmp_mpi(mpi *X, mpi *Y);

    /**
       @brief          Compare signed values
       @return         1 if X is greater than z,
                      -1 if X is lesser  than z or
                       0 if X is equal to z
     */
    PUBLIC int mpi_cmp_int(mpi *X, int z);

    /**
       @brief          Unsigned addition: X = |A| + |B|
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_add_abs(mpi *X, mpi *A, mpi *B);

    /**
       @brief          Unsigned substraction: X = |A| - |B|
       @return         0 if successful, EST_ERR_MPI_NEGATIVE_VALUE if B is greater than A
     */
    PUBLIC int mpi_sub_abs(mpi *X, mpi *A, mpi *B);

    /**
       @brief          Signed addition: X = A + B
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_add_mpi(mpi *X, mpi *A, mpi *B);

    /**
       @brief          Signed substraction: X = A - B
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_sub_mpi(mpi *X, mpi *A, mpi *B);

    /**
       @brief          Signed addition: X = A + b
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_add_int(mpi *X, mpi *A, int b);

    /**
       @brief          Signed substraction: X = A - b
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_sub_int(mpi *X, mpi *A, int b);

    /**
       @brief          Baseline multiplication: X = A * B
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_mul_mpi(mpi *X, mpi *A, mpi *B);

    /**
       @brief          Baseline multiplication: X = A * b
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_mul_int(mpi *X, mpi *A, t_int b);

    /**
       @brief          Division by mpi: A = Q * B + R
       @return         0 if successful, 1 if memory allocation failed, EST_ERR_MPI_DIVISION_BY_ZERO if B == 0
       @note           Either Q or R can be NULL.
     */
    PUBLIC int mpi_div_mpi(mpi *Q, mpi *R, mpi *A, mpi *B);

    /**
       @brief          Division by int: A = Q * b + R
       @return         0 if successful, 1 if memory allocation failed, EST_ERR_MPI_DIVISION_BY_ZERO if b == 0
       @note           Either Q or R can be NULL.
     */
    PUBLIC int mpi_div_int(mpi *Q, mpi *R, mpi *A, int b);

    /**
       @brief          Modulo: R = A mod B
       @return         0 if successful, 1 if memory allocation failed, EST_ERR_MPI_DIVISION_BY_ZERO if B == 0
     */
    PUBLIC int mpi_mod_mpi(mpi *R, mpi *A, mpi *B);

    /**
       @brief          Modulo: r = A mod b
       @return         0 if successful, 1 if memory allocation failed, EST_ERR_MPI_DIVISION_BY_ZERO if b == 0
     */
    PUBLIC int mpi_mod_int(t_int * r, mpi *A, int b);

    /**
       @brief          Sliding-window exponentiation: X = A^E mod N
       @return         0 if successful, 1 if memory allocation failed, EST_ERR_MPI_BAD_INPUT_DATA if N is negative or even
       @note           _RR is used to avoid re-computing R*R mod N across multiple calls, which speeds up things a bit. It
                       can be set to NULL if the extra performance is unneeded.  
     */
    PUBLIC int mpi_exp_mod(mpi *X, mpi *A, mpi *E, mpi *N, mpi *_RR);

    /**
       @brief          Greatest common divisor: G = gcd(A, B)
       @return         0 if successful, 1 if memory allocation failed
     */
    PUBLIC int mpi_gcd(mpi *G, mpi *A, mpi *B);

    /**
       @brief          Modular inverse: X = A^-1 mod N
       @return         0 if successful, 1 if memory allocation failed,
                       EST_ERR_MPI_BAD_INPUT_DATA if N is negative or nil
                       EST_ERR_MPI_NOT_ACCEPTABLE if A has no inverse mod N
     */
    PUBLIC int mpi_inv_mod(mpi *X, mpi *A, mpi *N);

    /**
       @brief          Miller-Rabin primality test
       @return         0 if successful (probably prime), 1 if memory allocation failed,
                       EST_ERR_MPI_NOT_ACCEPTABLE if X is not prime
     */
    PUBLIC int mpi_is_prime(mpi *X, int (*f_rng) (void *), void *p_rng);

    /**
       @brief          Prime number generation
       @param X        destination mpi
       @param nbits    required size of X in bits
       @param dh_flag  if 1, then (X-1)/2 will be prime too
       @param f_rng    RNG function
       @param p_rng    RNG parameter
       @return         0 if successful (probably prime), 1 if memory allocation failed,
                       EST_ERR_MPI_BAD_INPUT_DATA if nbits is < 3
     */
    PUBLIC int mpi_gen_prime(mpi *X, int nbits, int dh_flag, int (*f_rng) (void *), void *p_rng);

#if UNUSED
    /**
       @brief          Checkup routine
       @return         0 if successful, or 1 if the test failed
     */
    PUBLIC int mpi_self_test(int verbose);
#endif

#ifdef __cplusplus
}
#endif
#endif              /* bignum.h */

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
