/*
    hello.c -- Classic "Hello, world" demonstration program

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <stdio.h>

#include "tropicssl/md5.h"

int main(void)
{
    int i;
    uchar digest[16];
    char str[] = "Hello, world!";

    printf("\n  MD5('%s') = ", str);

    md5((uchar *)str, 13, digest);

    for (i = 0; i < 16; i++)
        printf("%02x", digest[i]);

    printf("\n\n");

#ifdef WIN32
    printf("  Press Enter to exit this program.\n");
    fflush(stdout);
    getchar();
#endif

    return (0);
}

/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2014. All Rights Reserved.

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
