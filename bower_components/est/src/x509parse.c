/*
    x509parse.c -- X.509 certificate and private key decoding

    The ITU-T X.509 standard defines a certificat format for PKI.
  
    http://www.ietf.org/rfc/rfc2459.txt
    http://www.ietf.org/rfc/rfc3279.txt
  
    ftp://ftp.rsasecurity.com/pub/pkcs/ascii/pkcs-1v2.asc
  
    http://www.itu.int/ITU-T/studygroups/com17/languages/X.680-0207.pdf
    http://www.itu.int/ITU-T/studygroups/com17/languages/X.690-0207.pdf
*/

#include "est.h"

#if ME_EST_X509

/*
    ASN.1 DER decoding routines
 */
static int asn1_get_len(uchar **p, uchar *end, int *len)
{
    if ((end - *p) < 1) {
        return EST_ERR_ASN1_OUT_OF_DATA;
    }

    if ((**p & 0x80) == 0) {
        *len = *(*p)++;
    } else {
        switch (**p & 0x7F) {
        case 1:
            if ((end - *p) < 2) {
                return EST_ERR_ASN1_OUT_OF_DATA;
            }
            *len = (*p)[1];
            (*p) += 2;
            break;

        case 2:
            if ((end - *p) < 3) {
                return EST_ERR_ASN1_OUT_OF_DATA;
            }
            *len = ((*p)[1] << 8) | (*p)[2];
            (*p) += 3;
            break;

        default:
            return EST_ERR_ASN1_INVALID_LENGTH;
        }
    }
    if (*len > (int)(end - *p)) {
        return EST_ERR_ASN1_OUT_OF_DATA;
    }
    return 0;
}


static int asn1_get_tag(uchar **p, uchar *end, int *len, int tag)
{
    if ((end - *p) < 1) {
        return EST_ERR_ASN1_OUT_OF_DATA;
    }
    if (**p != tag) {
        return EST_ERR_ASN1_UNEXPECTED_TAG;
    }
    (*p)++;
    return asn1_get_len(p, end, len);
}


static int asn1_get_bool(uchar **p, uchar *end, int *val)
{
    int ret, len;

    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_BOOLEAN)) != 0) {
        return ret;
    }
    if (len != 1) {
        return EST_ERR_ASN1_INVALID_LENGTH;
    }
    *val = (**p != 0) ? 1 : 0;
    (*p)++;
    return 0;
}


static int asn1_get_int(uchar **p, uchar *end, int *val)
{
    int ret, len;

    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_INTEGER)) != 0) {
        return ret;
    }
    if (len > (int)sizeof(int) || (**p & 0x80) != 0) {
        return EST_ERR_ASN1_INVALID_LENGTH;
    }
    *val = 0;
    while (len-- > 0) {
        *val = (*val << 8) | **p;
        (*p)++;
    }
    return 0;
}


static int asn1_get_mpi(uchar **p, uchar *end, mpi * X)
{
    int ret, len;

    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_INTEGER)) != 0) {
        return ret;
    }
    ret = mpi_read_binary(X, *p, len);
    *p += len;
    return ret;
}


/*
    Version  ::=  INTEGER  {  v1(0), v2(1), v3(2)  }
 */
static int x509_get_version(uchar **p, uchar *end, int *ver)
{
    int ret, len;

    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_CONTEXT_SPECIFIC | EST_ASN1_CONSTRUCTED | 0)) != 0) {
        if (ret == EST_ERR_ASN1_UNEXPECTED_TAG) {
            return *ver = 0;
        }
        return ret;
    }
    end = *p + len;

    if ((ret = asn1_get_int(p, end, ver)) != 0) {
        return EST_ERR_X509_CERT_INVALID_VERSION | ret;
    }
    if (*p != end) {
        return EST_ERR_X509_CERT_INVALID_VERSION | EST_ERR_ASN1_LENGTH_MISMATCH;
    }
    return 0;
}


/*
    CertificateSerialNumber  ::=  INTEGER
 */
static int x509_get_serial(uchar **p, uchar *end, x509_buf * serial)
{
    int ret;

    if ((end - *p) < 1) {
        return EST_ERR_X509_CERT_INVALID_SERIAL | EST_ERR_ASN1_OUT_OF_DATA;
    }
    if (**p != (EST_ASN1_CONTEXT_SPECIFIC | EST_ASN1_PRIMITIVE | 2) && **p != EST_ASN1_INTEGER) {
        return EST_ERR_X509_CERT_INVALID_SERIAL | EST_ERR_ASN1_UNEXPECTED_TAG;
    }
    serial->tag = *(*p)++;

    if ((ret = asn1_get_len(p, end, &serial->len)) != 0) {
        return EST_ERR_X509_CERT_INVALID_SERIAL | ret;
    }
    serial->p = *p;
    *p += serial->len;
    return 0;
}


/*
    AlgorithmIdentifier  ::=  SEQUENCE  {
         algorithm               OBJECT IDENTIFIER,
         parameters              ANY DEFINED BY algorithm OPTIONAL  }
 */
static int x509_get_alg(uchar **p, uchar *end, x509_buf * alg)
{
    int ret, len;

    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
        return EST_ERR_X509_CERT_INVALID_ALG | ret;
    }
    end = *p + len;
    alg->tag = **p;

    if ((ret = asn1_get_tag(p, end, &alg->len, EST_ASN1_OID)) != 0) {
        return EST_ERR_X509_CERT_INVALID_ALG | ret;
    }
    alg->p = *p;
    *p += alg->len;

    if (*p == end) {
        return 0;
    }

    /*
        assume the algorithm parameters must be NULL
     */
    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_NULL)) != 0) {
        return EST_ERR_X509_CERT_INVALID_ALG | ret;
    }
    if (*p != end) {
        return EST_ERR_X509_CERT_INVALID_ALG | EST_ERR_ASN1_LENGTH_MISMATCH;
    }
    return 0;
}


/*
    RelativeDistinguishedName ::=
      SET OF AttributeTypeAndValue
  
    AttributeTypeAndValue ::= SEQUENCE {
      type     AttributeType,
      value    AttributeValue }
  
    AttributeType ::= OBJECT IDENTIFIER
  
    AttributeValue ::= ANY DEFINED BY AttributeType
 */
static int x509_get_name(uchar **p, uchar *end, x509_name * cur)
{
    int ret, len;
    uchar *end2;
    x509_buf *oid;
    x509_buf *val;

    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SET)) != 0) {
        return EST_ERR_X509_CERT_INVALID_NAME | ret;
    }
    end2 = end;
    end = *p + len;

    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
        return EST_ERR_X509_CERT_INVALID_NAME | ret;
    }
    if (*p + len != end) {
        return EST_ERR_X509_CERT_INVALID_NAME | EST_ERR_ASN1_LENGTH_MISMATCH;
    }
    oid = &cur->oid;
    oid->tag = **p;

    if ((ret = asn1_get_tag(p, end, &oid->len, EST_ASN1_OID)) != 0) {
        return EST_ERR_X509_CERT_INVALID_NAME | ret;
    }
    oid->p = *p;
    *p += oid->len;

    if ((end - *p) < 1) {
        return EST_ERR_X509_CERT_INVALID_NAME | EST_ERR_ASN1_OUT_OF_DATA;
    }
    if (**p != EST_ASN1_BMP_STRING && **p != EST_ASN1_UTF8_STRING &&
            **p != EST_ASN1_T61_STRING && **p != EST_ASN1_PRINTABLE_STRING &&
            **p != EST_ASN1_IA5_STRING && **p != EST_ASN1_UNIVERSAL_STRING) {
        return EST_ERR_X509_CERT_INVALID_NAME | EST_ERR_ASN1_UNEXPECTED_TAG;
    }

    val = &cur->val;
    val->tag = *(*p)++;

    if ((ret = asn1_get_len(p, end, &val->len)) != 0) {
        return EST_ERR_X509_CERT_INVALID_NAME | ret;
    }
    val->p = *p;
    *p += val->len;

    cur->next = NULL;

    if (*p != end) {
        return EST_ERR_X509_CERT_INVALID_NAME | EST_ERR_ASN1_LENGTH_MISMATCH;
    }

    /*
       recurse until end of SEQUENCE is reached
     */
    if (*p == end2) {
        return 0;
    }
    cur->next = (x509_name *) malloc(sizeof(x509_name));

    if (cur->next == NULL) {
        return 1;
    }
    return x509_get_name(p, end2, cur->next);
}


/*
    Validity ::= SEQUENCE {
         notBefore      Time,
         notAfter       Time }
  
    Time ::= CHOICE {
         utcTime        UTCTime,
         generalTime    GeneralizedTime }
 */
static int x509_get_dates(uchar **p, uchar *end, x509_time *from, x509_time *to)
{
    int ret, len;
    char date[64];

    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
        return EST_ERR_X509_CERT_INVALID_DATE | ret;
    }
    end = *p + len;

    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_UTC_TIME)) != 0) {
        return EST_ERR_X509_CERT_INVALID_DATE | ret;
    }
    memset(date, 0, sizeof(date));
    memcpy(date, *p, (len < (int)sizeof(date) - 1) ?  len : (int)sizeof(date) - 1);

    if (sscanf(date, "%2d%2d%2d%2d%2d%2d", &from->year, &from->mon, &from->day, &from->hour, &from->min, &from->sec) < 5) {
        return EST_ERR_X509_CERT_INVALID_DATE;
    }
    from->year += 100 * (from->year < 90);
    from->year += 1900;
    *p += len;

    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_UTC_TIME)) != 0) {
        return EST_ERR_X509_CERT_INVALID_DATE | ret;
    }
    memset(date, 0, sizeof(date));
    memcpy(date, *p, (len < (int)sizeof(date) - 1) ?  len : (int)sizeof(date) - 1);

    if (sscanf(date, "%2d%2d%2d%2d%2d%2d", &to->year, &to->mon, &to->day, &to->hour, &to->min, &to->sec) < 5) {
        return EST_ERR_X509_CERT_INVALID_DATE;
    }
    to->year += 100 * (to->year < 90);
    to->year += 1900;
    *p += len;

    if (*p != end) {
        return EST_ERR_X509_CERT_INVALID_DATE | EST_ERR_ASN1_LENGTH_MISMATCH;
    }
    return 0;
}


/*
    SubjectPublicKeyInfo  ::=  SEQUENCE  {
         algorithm            AlgorithmIdentifier,
         subjectPublicKey     BIT STRING }
 */
static int x509_get_pubkey(uchar **p, uchar *end, x509_buf * pk_alg_oid, mpi * N, mpi * E)
{
    int ret, len;
    uchar *end2;

    if ((ret = x509_get_alg(p, end, pk_alg_oid)) != 0) {
        return ret;
    }

    /*
     * only RSA public keys handled at this time
     */
    if (pk_alg_oid->len != 9 || memcmp(pk_alg_oid->p, OID_PKCS1_RSA, 9) != 0) {
        return EST_ERR_X509_CERT_UNKNOWN_PK_ALG;
    }
    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_ME_STRING)) != 0) {
        return EST_ERR_X509_CERT_INVALID_PUBKEY | ret;
    }
    if ((end - *p) < 1) {
        return EST_ERR_X509_CERT_INVALID_PUBKEY | EST_ERR_ASN1_OUT_OF_DATA;
    }
    end2 = *p + len;

    if (*(*p)++ != 0) {
        return EST_ERR_X509_CERT_INVALID_PUBKEY;
    }

    /*
            RSAPublicKey ::= SEQUENCE {
                    modulus                   INTEGER,      -- n
                    publicExponent    INTEGER       -- e
            }
     */
    if ((ret = asn1_get_tag(p, end2, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
        return EST_ERR_X509_CERT_INVALID_PUBKEY | ret;
    }
    if (*p + len != end2) {
        return EST_ERR_X509_CERT_INVALID_PUBKEY | EST_ERR_ASN1_LENGTH_MISMATCH;
    }
    if ((ret = asn1_get_mpi(p, end2, N)) != 0 || (ret = asn1_get_mpi(p, end2, E)) != 0) {
        return EST_ERR_X509_CERT_INVALID_PUBKEY | ret;
    }
    if (*p != end) {
        return EST_ERR_X509_CERT_INVALID_PUBKEY | EST_ERR_ASN1_LENGTH_MISMATCH;
    }
    return 0;
}


static int x509_get_sig(uchar **p, uchar *end, x509_buf * sig)
{
    int ret, len;

    sig->tag = **p;

    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_ME_STRING)) != 0) {
        return EST_ERR_X509_CERT_INVALID_SIGNATURE | ret;
    }
    if (--len < 1 || *(*p)++ != 0) {
        return EST_ERR_X509_CERT_INVALID_SIGNATURE;
    }
    sig->len = len;
    sig->p = *p;
    *p += len;
    return 0;
}


/*
    X.509 v2/v3 unique identifier (not parsed)
 */
static int x509_get_uid(uchar **p, uchar *end, x509_buf * uid, int n)
{
    int ret;

    if (*p == end) {
        return 0;
    }
    uid->tag = **p;

    if ((ret = asn1_get_tag(p, end, &uid->len, EST_ASN1_CONTEXT_SPECIFIC | EST_ASN1_CONSTRUCTED | n)) != 0) {
        if (ret == EST_ERR_ASN1_UNEXPECTED_TAG) {
            return 0;
        }
        return ret;
    }
    uid->p = *p;
    *p += uid->len;
    return 0;
}


/*
   X.509 v3 extensions (only BasicConstraints are parsed)
 */
static int x509_get_ext(uchar **p, uchar *end, x509_buf * ext, int *ca_istrue, int *max_pathlen)
{
    int ret, len;
    int is_critical = 1;
    int is_cacert = 0;
    uchar *end2;

    if (*p == end) {
        return 0;
    }
    ext->tag = **p;

    if ((ret = asn1_get_tag(p, end, &ext->len, EST_ASN1_CONTEXT_SPECIFIC | EST_ASN1_CONSTRUCTED | 3)) != 0) {
        if (ret == EST_ERR_ASN1_UNEXPECTED_TAG) {
            return 0;
        }
        return ret;
    }
    ext->p = *p;
    end = *p + ext->len;

    /*
       Extensions  ::=      SEQUENCE SIZE (1..MAX) OF Extension
      
       Extension  ::=  SEQUENCE      {
                    extnID          OBJECT IDENTIFIER,
                    critical        BOOLEAN DEFAULT FALSE,
                    extnValue       OCTET STRING  }
     */
    if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
        return EST_ERR_X509_CERT_INVALID_EXTENSIONS | ret;
    }
    if (end != *p + len) {
        return EST_ERR_X509_CERT_INVALID_EXTENSIONS | EST_ERR_ASN1_LENGTH_MISMATCH;
    }

    while (*p < end) {
        if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
            return EST_ERR_X509_CERT_INVALID_EXTENSIONS | ret;
        }
        if (memcmp(*p, "\x06\x03\x55\x1D\x13", 5) != 0) {
            *p += len;
            continue;
        }

        *p += 5;

        if ((ret = asn1_get_bool(p, end, &is_critical)) != 0 && (ret != EST_ERR_ASN1_UNEXPECTED_TAG)) {
            return EST_ERR_X509_CERT_INVALID_EXTENSIONS | ret;
        }
        if ((ret = asn1_get_tag(p, end, &len, EST_ASN1_OCTET_STRING)) != 0) {
            return EST_ERR_X509_CERT_INVALID_EXTENSIONS | ret;
        }
        /*
           BasicConstraints ::= SEQUENCE {
                        cA                                              BOOLEAN DEFAULT FALSE,
                        pathLenConstraint               INTEGER (0..MAX) OPTIONAL }
         */
        end2 = *p + len;

        if ((ret = asn1_get_tag(p, end2, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
            return EST_ERR_X509_CERT_INVALID_EXTENSIONS | ret;
        }
        if (*p == end2) {
            continue;
        }
        if ((ret = asn1_get_bool(p, end2, &is_cacert)) != 0) {
            if (ret == EST_ERR_ASN1_UNEXPECTED_TAG) {
                ret = asn1_get_int(p, end2, &is_cacert);
            }
            if (ret != 0) {
                return EST_ERR_X509_CERT_INVALID_EXTENSIONS | ret;
            }
            if (is_cacert != 0) {
                is_cacert = 1;
            }
        }
        if (*p == end2) {
            continue;
        }
        if ((ret = asn1_get_int(p, end2, max_pathlen)) != 0) {
            return EST_ERR_X509_CERT_INVALID_EXTENSIONS | ret;
        }
        if (*p != end2) {
            return EST_ERR_X509_CERT_INVALID_EXTENSIONS | EST_ERR_ASN1_LENGTH_MISMATCH;
        }
        max_pathlen++;
    }

    if (*p != end) {
        return EST_ERR_X509_CERT_INVALID_EXTENSIONS | EST_ERR_ASN1_LENGTH_MISMATCH;
    }
    *ca_istrue = is_critical & is_cacert;
    return 0;
}


/*
   Parse one or more certificates and add them to the chained list
 */
int x509parse_crt(x509_cert * chain, uchar *buf, int buflen)
{
    int ret, len;
    uchar *s1, *s2, *oldbuf;
    uchar *p, *end;
    x509_cert *crt;

    crt = chain;

    while (crt->version != 0) {
        crt = crt->next;
    }

    /*
       check if the certificate is encoded in base64
     */
    s1 = (uchar *)strstr((char *)buf, "-----BEGIN CERTIFICATE-----");

    if (s1 != NULL) {
        s2 = (uchar *)strstr((char *)buf, "-----END CERTIFICATE-----");

        if (s2 == NULL || s2 <= s1) {
            return EST_ERR_X509_CERT_INVALID_PEM;
        }
        s1 += 27;
        if (*s1 == '\r') {
            s1++;
        }
        if (*s1 == '\n') {
            s1++;
        } else {
            return EST_ERR_X509_CERT_INVALID_PEM;
        }

        /*
           get the DER data length and decode the buffer
         */
        len = 0;
        ret = base64_decode(NULL, &len, s1, s2 - s1);

        if (ret == EST_ERR_BASE64_INVALID_CHARACTER) {
            return EST_ERR_X509_CERT_INVALID_PEM | ret;
        }
        if ((p = (uchar *)malloc(len)) == NULL) {
            return 1;
        }
        if ((ret = base64_decode(p, &len, s1, s2 - s1)) != 0) {
            free(p);
            return EST_ERR_X509_CERT_INVALID_PEM | ret;
        }

        /*
            update the buffer size and offset
         */
        s2 += 25;
        if (*s2 == '\r') {
            s2++;
        }
        if (*s2 == '\n') {
            s2++;
        } else {
            free(p);
            return EST_ERR_X509_CERT_INVALID_PEM;
        }
        oldbuf = buf;
        buflen -= s2 - buf;
        buf = s2;

    } else {
        /*
           nope, copy the raw DER data
         */
        p = (uchar*) malloc(len = buflen);
        if (p == NULL) {
            return 1;
        }
        memcpy(p, buf, buflen);
        oldbuf = buf;
        buflen = 0;
    }

    crt->raw.p = p;
    crt->raw.len = len;
    end = p + len;

    /*
       Certificate  ::=      SEQUENCE  {
                    tbsCertificate           TBSCertificate,
                    signatureAlgorithm       AlgorithmIdentifier,
                    signatureValue           BIT STRING      }
     */
    if ((ret = asn1_get_tag(&p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
        x509_free(crt);
        return EST_ERR_X509_CERT_INVALID_FORMAT;
    }
    if (len != (int)(end - p)) {
        x509_free(crt);
        return EST_ERR_X509_CERT_INVALID_FORMAT | EST_ERR_ASN1_LENGTH_MISMATCH;
    }

    /*
       TBSCertificate  ::=  SEQUENCE  {
     */
    crt->tbs.p = p;
    if ((ret = asn1_get_tag(&p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
        x509_free(crt);
        return EST_ERR_X509_CERT_INVALID_FORMAT | ret;
    }
    end = p + len;
    crt->tbs.len = end - crt->tbs.p;

    /*
       Version      ::=      INTEGER  {      v1(0), v2(1), v3(2)  }
      
       CertificateSerialNumber      ::=      INTEGER
      
       signature                    AlgorithmIdentifier
     */
    if ((ret = x509_get_version(&p, end, &crt->version)) != 0 ||
        (ret = x509_get_serial(&p, end, &crt->serial)) != 0 ||
        (ret = x509_get_alg(&p, end, &crt->sig_oid1)) != 0) {
        x509_free(crt);
        return ret;
    }
    crt->version++;
    if (crt->version > 3) {
        x509_free(crt);
        return EST_ERR_X509_CERT_UNKNOWN_VERSION;
    }
    if (crt->sig_oid1.len != 9 || memcmp(crt->sig_oid1.p, OID_PKCS1, 8) != 0) {
        x509_free(crt);
#if UNUSED
if (buflen > 0) {
    goto error;
}
#endif
        return EST_ERR_X509_CERT_UNKNOWN_SIG_ALG;
    }
    if (crt->sig_oid1.p[8] < 2 || crt->sig_oid1.p[8] > 5) {
        x509_free(crt);
#if UNUSED
if (buflen > 0) {
    goto error;
}
#endif
        return EST_ERR_X509_CERT_UNKNOWN_SIG_ALG;
    }
    /*
        issuer Name
     */
    crt->issuer_raw.p = p;
    if ((ret = asn1_get_tag(&p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
        x509_free(crt);
        return EST_ERR_X509_CERT_INVALID_FORMAT | ret;
    }
    if ((ret = x509_get_name(&p, p + len, &crt->issuer)) != 0) {
        x509_free(crt);
        return ret;
    }
    crt->issuer_raw.len = p - crt->issuer_raw.p;

    /*
       Validity ::= SEQUENCE {
                    notBefore          Time,
                    notAfter           Time }
      
     */
    if ((ret = x509_get_dates(&p, end, &crt->valid_from, &crt->valid_to)) != 0) {
        x509_free(crt);
#if UNUSED
if (buflen > 0) {
    goto error;
}
#endif
        return ret;
    }

    /*
        subject Name
     */
    crt->subject_raw.p = p;

    if ((ret = asn1_get_tag(&p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
        x509_free(crt);
        return EST_ERR_X509_CERT_INVALID_FORMAT | ret;
    }
    if ((ret = x509_get_name(&p, p + len, &crt->subject)) != 0) {
        x509_free(crt);
        return ret;
    }
    crt->subject_raw.len = p - crt->subject_raw.p;

    /*
       SubjectPublicKeyInfo  ::=  SEQUENCE
            algorithm             AlgorithmIdentifier,
            subjectPublicKey      BIT STRING      }
     */
    if ((ret = asn1_get_tag(&p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
        x509_free(crt);
        return EST_ERR_X509_CERT_INVALID_FORMAT | ret;
    }
    if ((ret = x509_get_pubkey(&p, p + len, &crt->pk_oid, &crt->rsa.N, &crt->rsa.E)) != 0) {
        x509_free(crt);
        return ret;
    }
    if ((ret = rsa_check_pubkey(&crt->rsa)) != 0) {
        x509_free(crt);
        return ret;
    }
    crt->rsa.len = mpi_size(&crt->rsa.N);

    /*
        issuerUniqueID  [1]      IMPLICIT UniqueIdentifier OPTIONAL,
                                 -- If present, version shall be v2 or v3
        subjectUniqueID [2]      IMPLICIT UniqueIdentifier OPTIONAL,
                                 -- If present, version shall be v2 or v3
        extensions      [3]      EXPLICIT Extensions OPTIONAL
                                 -- If present, version shall be v3
     */
    if (crt->version == 2 || crt->version == 3) {
        ret = x509_get_uid(&p, end, &crt->issuer_id, 1);
        if (ret != 0) {
            x509_free(crt);
            return ret;
        }
    }
    if (crt->version == 2 || crt->version == 3) {
        ret = x509_get_uid(&p, end, &crt->subject_id, 2);
        if (ret != 0) {
            x509_free(crt);
            return ret;
        }
    }
    if (crt->version == 3) {
        ret = x509_get_ext(&p, end, &crt->v3_ext, &crt->ca_istrue, &crt->max_pathlen);
        if (ret != 0) {
            x509_free(crt);
            return ret;
        }
    }
    if (p != end) {
        x509_free(crt);
        return EST_ERR_X509_CERT_INVALID_FORMAT | EST_ERR_ASN1_LENGTH_MISMATCH;
    }
    end = crt->raw.p + crt->raw.len;

    /*
        signatureAlgorithm       AlgorithmIdentifier,
        signatureValue           BIT STRING
     */
    if ((ret = x509_get_alg(&p, end, &crt->sig_oid2)) != 0) {
        x509_free(crt);
        return ret;
    }
    if (memcmp(crt->sig_oid1.p, crt->sig_oid2.p, 9) != 0) {
        x509_free(crt);
        return EST_ERR_X509_CERT_SIG_MISMATCH;
    }
    if ((ret = x509_get_sig(&p, end, &crt->sig)) != 0) {
        x509_free(crt);
        return ret;
    }
    if (p != end) {
        x509_free(crt);
        return EST_ERR_X509_CERT_INVALID_FORMAT | EST_ERR_ASN1_LENGTH_MISMATCH;
    }
    crt->next = (x509_cert *) malloc(sizeof(x509_cert));

    if (crt->next == NULL) {
        x509_free(crt);
        return 1;
    }
    crt = crt->next;
    memset(crt, 0, sizeof(x509_cert));

#if UNUSED
more:
#endif
    if (buflen > 0) {
        int rc = x509parse_crt(crt, buf, buflen);
        return 0;
    }
    return 0;

#if UNUSED
error:
    {
        char msg[80], *cp;
        strncpy(msg, (char*) &oldbuf[1], sizeof(msg) - 1);
        if ((cp = strchr(msg, '\n')) != 0) {
            *cp = '\0';
        }
        printf("FAILED to parse %s\n", msg);
        memset(crt, 0, sizeof(x509_cert));
        goto more;
    }
#endif
}


/*
    Load one or more certificates and add them to the chained list
 */
int x509parse_crtfile(x509_cert * chain, char *path)
{
    int ret;
    FILE *f;
    size_t n;
    uchar *buf;

    if ((f = fopen(path, "rb")) == NULL) {
        return 1;
    }
    fseek(f, 0, SEEK_END);
    n = (size_t) ftell(f);
    fseek(f, 0, SEEK_SET);

    if ((buf = (uchar *)malloc(n + 1)) == NULL) {
        return 1;
    }
    if (fread(buf, 1, n, f) != n) {
        fclose(f);
        free(buf);
        return 1;
    }
    buf[n] = '\0';
    ret = x509parse_crt(chain, buf, (int)n);
    memset(buf, 0, n + 1);
    free(buf);
    fclose(f);
    return ret;
}


#if ME_EST_DES
/*
    Read a 16-byte hex string and convert it to binary
 */
static int x509_get_iv(uchar *s, uchar iv[8])
{
    int i, j, k;

    memset(iv, 0, 8);

    for (i = 0; i < 16; i++, s++) {
        if (*s >= '0' && *s <= '9') {
            j = *s - '0';
        } else if (*s >= 'A' && *s <= 'F') {
            j = *s - '7';
        } else if (*s >= 'a' && *s <= 'f') {
            j = *s - 'W';
        } else {
            return EST_ERR_X509_KEY_INVALID_ENC_IV;
        }
        k = ((i & 1) != 0) ? j : j << 4;
        iv[i >> 1] = (uchar)(iv[i >> 1] | k);
    }

    return 0;
}


/*
    Decrypt with 3DES-CBC, using PBKDF1 for key derivation
 */
static void x509_des3_decrypt(uchar des3_iv[8], uchar *buf, int buflen, uchar *pwd, int pwdlen)
{
    md5_context md5_ctx;
    des3_context des3_ctx;
    uchar md5sum[16];
    uchar des3_key[24];

    /*
       3DES key[ 0..15] = MD5(pwd || IV)
            key[16..23] = MD5(pwd || IV || 3DES key[ 0..15])
     */
    md5_starts(&md5_ctx);
    md5_update(&md5_ctx, pwd, pwdlen);
    md5_update(&md5_ctx, des3_iv, 8);
    md5_finish(&md5_ctx, md5sum);
    memcpy(des3_key, md5sum, 16);

    md5_starts(&md5_ctx);
    md5_update(&md5_ctx, md5sum, 16);
    md5_update(&md5_ctx, pwd, pwdlen);
    md5_update(&md5_ctx, des3_iv, 8);
    md5_finish(&md5_ctx, md5sum);
    memcpy(des3_key + 16, md5sum, 8);

    des3_set3key_dec(&des3_ctx, des3_key);
    des3_crypt_cbc(&des3_ctx, DES_DECRYPT, buflen, des3_iv, buf, buf);

    memset(&md5_ctx, 0, sizeof(md5_ctx));
    memset(&des3_ctx, 0, sizeof(des3_ctx));
    memset(md5sum, 0, 16);
    memset(des3_key, 0, 24);
}
#endif


/*
    Parse a private RSA key
 */
int x509parse_key(rsa_context * rsa, uchar *buf, int buflen, uchar *pwd, int pwdlen)
{
    int     ret, len, enc;
    uchar   *s1, *s2, *p, *end;
#if ME_EST_DES
    uchar   des3_iv[8];
#endif

    s1 = (uchar *)strstr((char *)buf, "-----BEGIN RSA PRIVATE KEY-----");

    if (s1 != NULL) {
        s2 = (uchar *)strstr((char *)buf, "-----END RSA PRIVATE KEY-----");

        if (s2 == NULL || s2 <= s1) {
            return EST_ERR_X509_KEY_INVALID_PEM;
        }
        s1 += 31;
        if (*s1 == '\r') {
            s1++;
        }
        if (*s1 == '\n') {
            s1++;
        } else {
            return EST_ERR_X509_KEY_INVALID_PEM;
        }
        enc = 0;

        if (memcmp(s1, "Proc-Type: 4,ENCRYPTED", 22) == 0) {
#if ME_EST_DES
            enc++;
            s1 += 22;
            if (*s1 == '\r') {
                s1++;
            }
            if (*s1 == '\n') {
                s1++;
            } else {
                return EST_ERR_X509_KEY_INVALID_PEM;
            }
            if (memcmp(s1, "DEK-Info: DES-EDE3-CBC,", 23) != 0) {
                return EST_ERR_X509_KEY_UNKNOWN_ENC_ALG;
            }
            s1 += 23;
            if (x509_get_iv(s1, des3_iv) != 0) {
                return EST_ERR_X509_KEY_INVALID_ENC_IV;
            }
            s1 += 16;
            if (*s1 == '\r') {
                s1++;
            }
            if (*s1 == '\n') {
                s1++;
            } else {
                return EST_ERR_X509_KEY_INVALID_PEM;
            }
#else
            return EST_ERR_X509_FEATURE_UNAVAILABLE;
#endif
        }
        len = 0;
        ret = base64_decode(NULL, &len, s1, s2 - s1);

        if (ret == EST_ERR_BASE64_INVALID_CHARACTER) {
            return ret | EST_ERR_X509_KEY_INVALID_PEM;
        }
        if ((buf = (uchar *)malloc(len)) == NULL) {
            return 1;
        }
        if ((ret = base64_decode(buf, &len, s1, s2 - s1)) != 0) {
            free(buf);
            return ret | EST_ERR_X509_KEY_INVALID_PEM;
        }
        buflen = len;

        if (enc != 0) {
#if ME_EST_DES
            if (pwd == NULL) {
                free(buf);
                return EST_ERR_X509_KEY_PASSWORD_REQUIRED;
            }
            x509_des3_decrypt(des3_iv, buf, buflen, pwd, pwdlen);

            if (buf[0] != 0x30 || buf[1] != 0x82 || buf[4] != 0x02 || buf[5] != 0x01) {
                free(buf);
                return EST_ERR_X509_KEY_PASSWORD_MISMATCH;
            }
#else
            return EST_ERR_X509_FEATURE_UNAVAILABLE;
#endif
        }
    }
    memset(rsa, 0, sizeof(rsa_context));

    p = buf;
    end = buf + buflen;

    /*
            RSAPrivateKey ::= SEQUENCE {
                    version                   Version,
                    modulus                   INTEGER,      -- n
                    publicExponent    INTEGER,      -- e
                    privateExponent   INTEGER,      -- d
                    prime1                    INTEGER,      -- p
                    prime2                    INTEGER,      -- q
                    exponent1                 INTEGER,      -- d mod (p-1)
                    exponent2                 INTEGER,      -- d mod (q-1)
                    coefficient               INTEGER,      -- (inverse of q) mod p
                    otherPrimeInfos   OtherPrimeInfos OPTIONAL
            }
     */
    if ((ret = asn1_get_tag(&p, end, &len, EST_ASN1_CONSTRUCTED | EST_ASN1_SEQUENCE)) != 0) {
        if (s1 != NULL) {
            free(buf);
        }
        rsa_free(rsa);
        return EST_ERR_X509_KEY_INVALID_FORMAT | ret;
    }
    end = p + len;

    if ((ret = asn1_get_int(&p, end, &rsa->ver)) != 0) {
        if (s1 != NULL) {
            free(buf);
        }
        rsa_free(rsa);
        return EST_ERR_X509_KEY_INVALID_FORMAT | ret;
    }
    if (rsa->ver != 0) {
        if (s1 != NULL) {
            free(buf);
        }
        rsa_free(rsa);
        return ret | EST_ERR_X509_KEY_INVALID_VERSION;
    }

    if ((ret = asn1_get_mpi(&p, end, &rsa->N)) != 0 ||
        (ret = asn1_get_mpi(&p, end, &rsa->E)) != 0 ||
        (ret = asn1_get_mpi(&p, end, &rsa->D)) != 0 ||
        (ret = asn1_get_mpi(&p, end, &rsa->P)) != 0 ||
        (ret = asn1_get_mpi(&p, end, &rsa->Q)) != 0 ||
        (ret = asn1_get_mpi(&p, end, &rsa->DP)) != 0 ||
        (ret = asn1_get_mpi(&p, end, &rsa->DQ)) != 0 ||
        (ret = asn1_get_mpi(&p, end, &rsa->QP)) != 0) {
        if (s1 != NULL) {
            free(buf);
        }
        rsa_free(rsa);
        return ret | EST_ERR_X509_KEY_INVALID_FORMAT;
    }

    rsa->len = mpi_size(&rsa->N);

    if (p != end) {
        if (s1 != NULL) {
            free(buf);
        }
        rsa_free(rsa);
        return EST_ERR_X509_KEY_INVALID_FORMAT | EST_ERR_ASN1_LENGTH_MISMATCH;
    }
    if ((ret = rsa_check_privkey(rsa)) != 0) {
        if (s1 != NULL) {
            free(buf);
        }
        rsa_free(rsa);
        return ret;
    }
    if (s1 != NULL) {
        free(buf);
    }
    return 0;
}


/*
    Load and parse a private RSA key
 */
int x509parse_keyfile(rsa_context * rsa, char *path, char *pwd)
{
    int ret;
    FILE *f;
    size_t n;
    uchar *buf;

    if ((f = fopen(path, "rb")) == NULL) {
        return 1;
    }

    fseek(f, 0, SEEK_END);
    n = (size_t) ftell(f);
    fseek(f, 0, SEEK_SET);

    if ((buf = (uchar *)malloc(n + 1)) == NULL) {
        return 1;
    }
    if (fread(buf, 1, n, f) != n) {
        fclose(f);
        free(buf);
        return 1;
    }
    buf[n] = '\0';

    if (pwd == NULL) {
        ret = x509parse_key(rsa, buf, (int)n, NULL, 0);
    } else {
        ret = x509parse_key(rsa, buf, (int)n, (uchar *)pwd, strlen(pwd));
    }
    memset(buf, 0, n + 1);
    free(buf);
    fclose(f);
    return ret;
}


/*
    Store the name in printable form into buf; no more than (end - buf) characters will be written
 */
int x509parse_dn_gets(char *prefix, char *buf, int bufsize, x509_name * dn)
{
    x509_name   *name;
    char        *end, s[128], *p;
    int         i;
    uchar       c;

    memset(s, 0, sizeof(s));
    name = dn;
    p = buf;
    end = &buf[bufsize];

    while (name != NULL) {
        p += snfmt(p, end - p, "%s", prefix);
        if (memcmp(name->oid.p, OID_X520, 2) == 0) {
            switch (name->oid.p[2]) {
            case X520_COMMON_NAME:
                p += snfmt(p, end - p, "CN=");
                break;

            case X520_COUNTRY:
                p += snfmt(p, end - p, "C=");
                break;

            case X520_LOCALITY:
                p += snfmt(p, end - p, "L=");
                break;

            case X520_STATE:
                p += snfmt(p, end - p, "ST=");
                break;

            case X520_ORGANIZATION:
                p += snfmt(p, end - p, "O=");
                break;

            case X520_ORG_UNIT:
                p += snfmt(p, end - p, "OU=");
                break;

            default:
                p += snfmt(p, end - p, "0x%02X=", name->oid.p[2]);
                break;
            }
        } else if (memcmp(name->oid.p, OID_PKCS9, 8) == 0) {
            switch (name->oid.p[8]) {
            case PKCS9_EMAIL:
                p += snfmt(p, end - p, "EMAIL=");
                break;

            default:
                p += snfmt(p, end - p, "0x%02X=", name->oid.p[8]);
                break;
            }
        } else {
            p += snfmt(p, end - p, "\?\?=");
        }
        for (i = 0; i < name->val.len; i++) {
            if (i >= (int)sizeof(s) - 1) {
                break;
            }
            c = name->val.p[i];
            if (c < 32 || c == 127 || (c > 128 && c < 160)) {
                s[i] = '?';
            } else {
                s[i] = c;
            }
        }
        s[i] = '\0';
        p += snfmt(p, end - p, "%s", s);
        name = name->next;
        p += snfmt(p, end - p, ",");
    }
    return p - buf;
}


/*
    Return an informational string about the certificate, or NULL if memory allocation failed
 */
char *x509parse_cert_info(char *prefix, char *buf, int bufsize, x509_cert *crt)
{
    char    *end, *p, *cipher, pbuf[5120];
    int     i, n;

    p = buf;
    end = &buf[bufsize];
    p += snfmt(p, end - p, "%sVERSION=%d,%sSERIAL=", prefix, crt->version, prefix);
    n = (crt->serial.len <= 32) ? crt->serial.len : 32;
    for (i = 0; i < n; i++) {
        p += snfmt(p, end - p, "%02X%s", crt->serial.p[i], (i < n - 1) ? ":" : "");
    }
    p += snfmt(p, end - p, ",");

    snfmt(pbuf, sizeof(pbuf), "%sS_", prefix);
    p += x509parse_dn_gets(pbuf, p, end - p, &crt->subject);

    snfmt(pbuf, sizeof(pbuf), "%sI_", prefix);
    p += x509parse_dn_gets(pbuf, p, end - p, &crt->issuer);

    p += snfmt(p, end - p, "%sSTART=%04d-%02d-%02d %02d:%02d:%02d,", prefix, crt->valid_from.year, crt->valid_from.mon,
        crt->valid_from.day, crt->valid_from.hour, crt->valid_from.min, crt->valid_from.sec);

    p += snfmt(p, end - p, "%sEND=%04d-%02d-%02d %02d:%02d:%02d,", prefix, crt->valid_to.year, crt->valid_to.mon, 
        crt->valid_to.day, crt->valid_to.hour, crt->valid_to.min, crt->valid_to.sec);

    switch (crt->sig_oid1.p[8]) {
    case RSA_MD2:
        cipher = "RSA_MD2";
        break;
    case RSA_MD4:
        cipher = "RSA_MD4";
        break;
    case RSA_MD5:
        cipher = "RSA_MD5";
        break;
    case RSA_SHA1:
        cipher = "RSA_SHA1";
        break;
    default:
        cipher = "RSA";
        break;
    }
    p += snfmt(p, end - p, "%sCIPHER=%s,", prefix, cipher);
    p += snfmt(p, end - p, "%sKEYSIZE=%d,", prefix, crt->rsa.N.n * (int)sizeof(ulong) * 8);
    return buf;
}


/*
    Return 0 if the certificate is still valid, or BADCERT_EXPIRED
 */
int x509parse_expired(x509_cert * crt)
{
    struct tm *lt;
    time_t tt;

    tt = time(NULL);
    lt = localtime(&tt);

    if (lt->tm_year > crt->valid_to.year - 1900) {
        return BADCERT_EXPIRED;
    }
    if (lt->tm_year == crt->valid_to.year - 1900 && lt->tm_mon > crt->valid_to.mon - 1) {
        return BADCERT_EXPIRED;
    }
    if (lt->tm_year == crt->valid_to.year - 1900 && lt->tm_mon == crt->valid_to.mon - 1 && lt->tm_mday > crt->valid_to.day) {
        return BADCERT_EXPIRED;
    }
    return 0;
}

static void x509_hash(uchar *in, int len, int alg, uchar *out)
{
    switch (alg) {
#if ME_EST_MD2
    case RSA_MD2:
        md2(in, len, out);
        break;
#endif
#if ME_EST_MD4
    case RSA_MD4:
        md4(in, len, out);
        break;
#endif
    case RSA_MD5:
        md5(in, len, out);
        break;
    case RSA_SHA1:
        sha1(in, len, out);
        break;
    default:
        memset(out, '\xFF', len);
        break;
    }
}


/*
    Verify the certificate validity
 */
int x509parse_verify(x509_cert *crt, x509_cert *trust_ca, char *cn, int *flags)
{
    x509_cert   *cur;
    x509_name   *name;
    uchar       hash[20];
    char        *domain, *peer;
    int         cn_len, hash_id, pathlen;

    *flags = x509parse_expired(crt);

    if (cn != NULL) {
        name = &crt->subject;
        cn_len = strlen(cn);

        while (name != NULL) {
            if (memcmp(name->oid.p, OID_CN, 3) == 0) {
                peer = (char*) name->val.p;
                if (name->val.len == cn_len && memcmp(peer, cn, cn_len) == 0) {
                    break;
                }
                /* 
                    Cert peer name must be of the form *.domain.tld. i.e. *.com is not valid 
                 */
                if (name->val.len > 2 && peer[0] == '*' && peer[1] == '.' && strchr(&peer[2], '.')) {
                    /* 
                        Required peer name must have a domain portion. i.e. domain.tld 
                     */
                    if ((domain = strchr(cn, '.')) != 0 && strncmp(&peer[2], &domain[1], name->val.len - 2) == 0) {
                        break;
                    }
                }
            }
            name = name->next;
        }
        if (name == NULL) {
            *flags |= BADCERT_CN_MISMATCH;
        }
    }
    *flags |= BADCERT_NOT_TRUSTED;

    /*
        Iterate upwards in the given cert chain, ignoring any upper cert with CA != TRUE.
     */
    cur = crt->next;
    pathlen = 1;

    while (cur->version != 0) {
        if (cur->ca_istrue == 0 || crt->issuer_raw.len != cur->subject_raw.len ||
            memcmp(crt->issuer_raw.p, cur->subject_raw.p, crt->issuer_raw.len) != 0) {
            cur = cur->next;
            continue;
        }
        hash_id = crt->sig_oid1.p[8];
        x509_hash(crt->tbs.p, crt->tbs.len, hash_id, hash);
        if (rsa_pkcs1_verify(&cur->rsa, RSA_PUBLIC, hash_id, 0, hash, crt->sig.p) != 0) {
            return EST_ERR_X509_CERT_VERIFY_FAILED;
        }
        pathlen++;
        crt = cur;
        cur = crt->next;
    }

    /*
        Atempt to validate topmost cert with our CA chain.
     */
    while (trust_ca->version != 0) {
        if (crt->issuer_raw.len != trust_ca->subject_raw.len ||
                memcmp(crt->issuer_raw.p, trust_ca->subject_raw.p, crt->issuer_raw.len) != 0) {
            trust_ca = trust_ca->next;
            continue;
        }
        if (trust_ca->max_pathlen > 0 && trust_ca->max_pathlen < pathlen) {
            break;
        }
        hash_id = crt->sig_oid1.p[8];
        x509_hash(crt->tbs.p, crt->tbs.len, hash_id, hash);

        if (rsa_pkcs1_verify(&trust_ca->rsa, RSA_PUBLIC, hash_id, 0, hash, crt->sig.p) == 0) {
            /*
                cert. is signed by a trusted CA
             */
            *flags &= ~BADCERT_NOT_TRUSTED;
            break;
        }
        trust_ca = trust_ca->next;
    }
    if (*flags & BADCERT_NOT_TRUSTED) {
        if (crt->issuer_raw.len == crt->subject_raw.len && 
                memcmp(crt->issuer_raw.p, crt->subject_raw.p, crt->issuer_raw.len) == 0) {
            *flags |= BADCERT_SELF_SIGNED;
        }
    }
    if (*flags != 0) {
        return EST_ERR_X509_CERT_VERIFY_FAILED;
    }
    return 0;
}


/*
    Unallocate all certificate data
 */
void x509_free(x509_cert * crt)
{
    x509_cert *cert_cur = crt;
    x509_cert *cert_prv;
    x509_name *name_cur;
    x509_name *name_prv;

    if (crt == NULL) {
        return;
    }
    do {
        rsa_free(&cert_cur->rsa);

        name_cur = cert_cur->issuer.next;
        while (name_cur != NULL) {
            name_prv = name_cur;
            name_cur = name_cur->next;
            memset(name_prv, 0, sizeof(x509_name));
            free(name_prv);
        }
        name_cur = cert_cur->subject.next;
        while (name_cur != NULL) {
            name_prv = name_cur;
            name_cur = name_cur->next;
            memset(name_prv, 0, sizeof(x509_name));
            free(name_prv);
        }
        if (cert_cur->raw.p != NULL) {
            memset(cert_cur->raw.p, 0, cert_cur->raw.len);
            free(cert_cur->raw.p);
        }
        cert_cur = cert_cur->next;
    } while (cert_cur != NULL);

    cert_cur = crt;
    do {
        cert_prv = cert_cur;
        cert_cur = cert_cur->next;

        memset(cert_prv, 0, sizeof(x509_cert));
        if (cert_prv != crt) {
            free(cert_prv);
        }
    } while (cert_cur != NULL);
}


#endif
