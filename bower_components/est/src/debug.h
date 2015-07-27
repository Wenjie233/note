/*
    debug.h -- Debugging support

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef SSL_DEBUG_H
#define SSL_DEBUG_H

#if FUTURE && ME_EST_LOGGING
#if ME_DEBUG
    #define LOG(l, ...) if (l <= logLevel) estLog(l, __VA_ARGS__) ; else
    #define RET(l, ...) if (l <= logLevel) estLog(l, __VA_ARGS__) ; else
#else
    #define LOG(l, ...) if (1) ; else
    #define RET(l, ...) if (1) ; else
#endif
    #define SSL_DEBUG_MSG(level, args)              debug_print_msg(ssl, level, debug_fmt args);
    #define SSL_DEBUG_RET(level, text, ret)         debug_print_ret(ssl, level, text, ret);
    #define SSL_DEBUG_BUF(level, text, buf, len)    debug_print_buf(ssl, level, text, buf, len);
    #define SSL_DEBUG_MPI(level, text, X)           debug_print_mpi(ssl, level, text, X);
    #define SSL_DEBUG_CRT(level, text, crt)         debug_print_crt(ssl, level, text, crt);
#endif

#if ME_EST_LOGGING
    #define SSL_DEBUG_MSG(level, args)              debug_print_msg(ssl, level, debug_fmt args);
    #define SSL_DEBUG_RET(level, text, ret)         debug_print_ret(ssl, level, text, ret);
    #define SSL_DEBUG_BUF(level, text, buf, len)    debug_print_buf(ssl, level, text, buf, len);
    #define SSL_DEBUG_MPI(level, text, X)           debug_print_mpi(ssl, level, text, X);
    #define SSL_DEBUG_CRT(level, text, crt)         debug_print_crt(ssl, level, text, crt);
#else
    #define SSL_DEBUG_MSG(level, args)            do {} while(0)
    #define SSL_DEBUG_RET(level, text, ret)       do {} while(0)
    #define SSL_DEBUG_BUF(level, text, buf, len)  do {} while(0)
    #define SSL_DEBUG_MPI(level, text, X)         do {} while(0)
    #define SSL_DEBUG_CRT(level, text, crt)       do {} while(0)
#endif

#ifdef __cplusplus
extern "C" {
#endif
    PUBLIC int snfmt(char *buf, ssize bufsize, cchar *fmt, ...);
    PUBLIC char *debug_fmt(const char *format, ...);
    PUBLIC void debug_print_msg(ssl_context *ssl, int level, char *text);
    PUBLIC void debug_print_ret(ssl_context *ssl, int level, char *text, int ret);
    PUBLIC void debug_print_buf(ssl_context *ssl, int level, char *text, uchar *buf, int len);
    PUBLIC void debug_print_mpi(ssl_context *ssl, int level, char *text, mpi * X);
    PUBLIC void debug_print_crt(ssl_context *ssl, int level, char *text, x509_cert * crt);

#ifdef __cplusplus
}
#endif
#endif  /* debug.h */

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
