/**
    est.h - Embedded Security Transport
 */

#ifndef _h_EST
#define _h_EST 1

/********************************** Includes **********************************/

#include "me.h"
#include "osdep.h"

/*********************************** Forwards *********************************/

#if ME_CPU_ARCH == ME_CPU_X86 || ME_CPU_ARCH == ME_CPU_X64
    #define EST_HAVE_ASM 1
#endif

/* Enable if using Intel CPU with SSE2 */
#define ME_EST_SSE2 0
/*
    Default configuration, optionally overridden by me.h
 */
#ifndef ME_EST_AES
    #define ME_EST_AES 1
#endif
#ifndef ME_EST_BIGNUM
    #define ME_EST_BIGNUM 1
#endif
#ifndef ME_EST_BASE64
    #define ME_EST_BASE64 1
#endif
#ifndef ME_EST_CAMELLIA
    #define ME_EST_CAMELLIA 0
#endif
#ifndef ME_EST_DES
    #define ME_EST_DES 0
#endif
#ifndef ME_EST_DHM
    #define ME_EST_DHM 1
#endif
#ifndef ME_EST_GEN_PRIME
    #define ME_EST_GEN_PRIME 1
#endif
#ifndef ME_EST_HAVEGE
    #define ME_EST_HAVEGE 1
#endif
#ifndef ME_EST_LOGGING
    #define ME_EST_LOGGING 0
#endif
#ifndef ME_EST_MD2
    #define ME_EST_MD2 0
#endif
#ifndef ME_EST_MD4
    #define ME_EST_MD4 0
#endif
#ifndef ME_EST_MD5
    #define ME_EST_MD5 1
#endif
#ifndef ME_EST_NET
    #define ME_EST_NET 1
#endif
#ifndef ME_EST_PADLOCK
    #define ME_EST_PADLOCK 1
#endif
#ifndef ME_EST_RC4
    #define ME_EST_RC4 1
#endif
#ifndef ME_EST_ROM_TABLES
    #define ME_EST_ROM_TABLES 1
#endif
#ifndef ME_EST_RSA
    #define ME_EST_RSA 1
#endif
#ifndef ME_EST_SELF_TEST
    #define ME_EST_SELF_TEST 0
#endif
#ifndef ME_EST_SHA1
    #define ME_EST_SHA1 1
#endif
#ifndef ME_EST_SHA2
    #define ME_EST_SHA2 1
#endif
#ifndef ME_EST_SHA4
    #define ME_EST_SHA4 1
#endif
#ifndef ME_EST_CLIENT
    #define ME_EST_CLIENT 1
    #undef ME_EST_MD5
    #define ME_EST_MD5 1
#endif
#ifndef ME_EST_SERVER
    #undef ME_EST_MD5
    #define ME_EST_MD5 1
    #define ME_EST_SERVER 1
#endif
#ifndef ME_EST_TEST_CERTS
    #define ME_EST_TEST_CERTS 1
#endif
#ifndef ME_EST_X509
    #define ME_EST_X509 1
#endif
#ifndef ME_EST_X509_WRITE
    #define ME_EST_X509_WRITE 1
#endif
#ifndef ME_EST_XTEA
    #define ME_EST_XTEA 1
#endif

/*
    Required settings
 */
#define ME_EST_SSL 1
#define ME_EST_TIMING 1

#if UNUSED
#define EST_CA_CERT "ca.crt"
#endif

/*
    Include all EST headers
 */
#include "bignum.h"
#include "net.h"
#include "dhm.h"
#include "rsa.h"
#include "md5.h"
#include "sha1.h"
#include "x509.h"
#include "ssl.h"
#include "aes.h"
#include "arc4.h"
#include "base64.h"
#include "bn_mul.h"
#include "camellia.h"
#if UNUSED
#include "certs.h"
#endif
#include "debug.h"
#include "des.h"
#include "havege.h"
#include "md2.h"
#include "md4.h"
#include "padlock.h"
#include "sha2.h"
#include "sha4.h"
#include "timing.h"
#include "xtea.h"

#ifdef __cplusplus
}
#endif

#endif /* _h_EST */

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
