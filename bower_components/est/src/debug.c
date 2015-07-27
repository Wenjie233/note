/*
    debug.c -- Debugging routines

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#include "est.h"

#if defined _MSC_VER && !defined  snprintf
    #define snprintf  _snprintf
#endif

#if defined _MSC_VER && !defined vsnprintf
    #define vsnprintf _vsnprintf
#endif

/*
    Safely format into a buffer. This works around inconsistencies in snprintf which is dangerous.
 */
int snfmt(char *buf, ssize bufsize, cchar *fmt, ...)
{
    va_list     ap;
    int         n;

    if (bufsize <= 0) {
        return 0;
    }
    va_start(ap, fmt);
#if _WIN32
    /* Windows does not guarantee a null will be appended */
    if ((n = vsnprintf(buf, bufsize - 1, fmt, ap)) < 0) {
        n = 0;
    }
    buf[n] = '\0';
#else
    /* Posix will return the number of characters that would fix in an unlimited buffer -- Ugh, dangerous! */
    n = vsnprintf(buf, bufsize, fmt, ap);
    n = min(n, bufsize);
#endif
    va_end(ap);
    return n;
}

#if ME_EST_LOGGING

char *debug_fmt(cchar *format, ...)
{
    va_list argp;
    static char str[512];
    int maxlen = sizeof(str) - 1;

    va_start(argp, format);
    vsnprintf(str, maxlen, format, argp);
    va_end(argp);
    str[maxlen] = '\0';
    return str;
}


void debug_print_msg(ssl_context *ssl, int level, char *text)
{
    char str[512];
    int maxlen = sizeof(str) - 1;

    if (ssl->f_dbg == NULL) {
        return;
    }
    snprintf(str, maxlen, "%s\n", text);
    str[maxlen] = '\0';
    ssl->f_dbg(ssl->p_dbg, level, str);
}


void debug_print_ret(ssl_context *ssl, int level, char *text, int ret)
{
    char str[512];
    int maxlen = sizeof(str) - 1;

    if (ssl->f_dbg == NULL) {
        return;
    }
    snprintf(str, maxlen, "%s() returned %d (0x%x)\n", text, ret, ret);
    str[maxlen] = '\0';
    ssl->f_dbg(ssl->p_dbg, level, str);
}


void debug_print_buf(ssl_context *ssl, int level, char *text, uchar *buf, int len)
{
    char str[512];
    int i, maxlen = sizeof(str) - 1;

    if (ssl->f_dbg == NULL || len < 0) {
        return;
    }
    snfmt(str, maxlen, "dumping '%s' (%d bytes)\n", text, len);
    str[maxlen] = '\0';
    ssl->f_dbg(ssl->p_dbg, level, str);

    for (i = 0; i < len; i++) {
        if (i >= 4096) {
            break;
        }
        if (i % 16 == 0) {
            if (i > 0) {
                ssl->f_dbg(ssl->p_dbg, level, "\n");
            }
            snfmt(str, maxlen, "%04x: ", i);
            str[maxlen] = '\0';
            ssl->f_dbg(ssl->p_dbg, level, str);
        }
        snfmt(str, maxlen, " %02x", (uint)buf[i]);
        str[maxlen] = '\0';
        ssl->f_dbg(ssl->p_dbg, level, str);
    }
    if (len > 0) {
        ssl->f_dbg(ssl->p_dbg, level, "\n");
    }
}


void debug_print_mpi(ssl_context *ssl, int level, char *text, mpi * X)
{
    char str[512];
    int i, j, k, n, maxlen = sizeof(str) - 1;

    if (ssl->f_dbg == NULL || X == NULL) {
        return;
    }
    for (n = X->n - 1; n >= 0; n--) {
        if (X->p[n] != 0) {
            break;
        }
    }
    snfmt(str, maxlen, "value of '%s' (%u bits) is:\n", text, (int) ((n + 1) * sizeof(t_int)) << 3);

    str[maxlen] = '\0';
    ssl->f_dbg(ssl->p_dbg, level, str);

    for (i = n, j = 0; i >= 0; i--, j++) {
        if (j % (16 / sizeof(t_int)) == 0) {
            if (j > 0) {
                ssl->f_dbg(ssl->p_dbg, level, "\n");
            }
            snfmt(str, maxlen, " ");
            str[maxlen] = '\0';
            ssl->f_dbg(ssl->p_dbg, level, str);
        }
        for (k = sizeof(t_int) - 1; k >= 0; k--) {
            snfmt(str, maxlen, " %02x", (uint) (X->p[i] >> (k << 3)) & 0xFF);
            str[maxlen] = '\0';
            ssl->f_dbg(ssl->p_dbg, level, str);
        }
    }
    ssl->f_dbg(ssl->p_dbg, level, "\n");
}


void debug_print_crt(ssl_context *ssl, int level, char *text, x509_cert * crt)
{
    char cbuf[5120], str[512], *p;
    int i, maxlen;

    if (ssl->f_dbg == NULL || crt == NULL) {
        return;
    }
    maxlen = sizeof(str) - 1;

    i = 0;
    while (crt != NULL && crt->next != NULL) {
        p = x509parse_cert_info("", cbuf, sizeof(cbuf), crt);
        snfmt(str, maxlen, "%s #%d:\n%s", text, ++i, p);
        str[maxlen] = '\0';
        ssl->f_dbg(ssl->p_dbg, level, str);
        debug_print_mpi(ssl, level, "crt->rsa.N", &crt->rsa.N);
        debug_print_mpi(ssl, level, "crt->rsa.E", &crt->rsa.E);
        crt = crt->next;
    }
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
