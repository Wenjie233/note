/*
    net.c -- Network routines

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#include "est.h"

#if ME_EST_NET

#if WINDOWS || WINCE
    #undef read
    #undef write
    #undef close
    #define read(fd,buf,len)        recv(fd,buf,len,0)
    #define write(fd,buf,len)       send(fd,buf,len,0)
    #define close(fd)               closesocket(fd)
    static int wsa_init_done = 0;
#endif

/*
    htons() is not always available
 */
#if defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && __BYTE_ORDER == __BIG_ENDIAN
    #define SSL_HTONS(n) (n)
#else
    #define SSL_HTONS(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))
#endif
#define net_htons(n) SSL_HTONS(n)

/*
   Initiate a TCP connection with host:port
 */
int net_connect(int *fd, char *host, int port)
{
    struct sockaddr_in server_addr;
    struct hostent *server_host;

#if WINDOWS || WINCE
    WSADATA wsaData;

    if (!wsa_init_done) {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) == SOCKET_ERROR) {
            return EST_ERR_NET_SOCKET_FAILED;
        }
        wsa_init_done = 1;
    }
#else
    signal(SIGPIPE, SIG_IGN);
#endif

    if ((server_host = gethostbyname(host)) == NULL) {
        return EST_ERR_NET_UNKNOWN_HOST;
    }
    if ((*fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        return EST_ERR_NET_SOCKET_FAILED;
    }
    memcpy((void *)&server_addr.sin_addr, (void *)server_host->h_addr, server_host->h_length);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = net_htons(port);

    if (connect(*fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        closesocket(*fd);
        return EST_ERR_NET_CONNECT_FAILED;
    }
    return 0;
}


/*
   Create a listening socket on bind_ip:port
 */
int net_bind(int *fd, char *bind_ip, int port)
{
    int n, c[4];
    struct sockaddr_in server_addr;

#if defined(WIN32) || defined(_WIN32_WCE)
    WSADATA wsaData;

    if (wsa_init_done == 0) {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) == SOCKET_ERROR) {
            return EST_ERR_NET_SOCKET_FAILED;
        }
        wsa_init_done = 1;
    }
#else
    signal(SIGPIPE, SIG_IGN);
#endif

    if ((*fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        return EST_ERR_NET_SOCKET_FAILED;
    }
    n = 1;
    setsockopt(*fd, SOL_SOCKET, SO_REUSEADDR, (char*) &n, sizeof(n));

    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = net_htons(port);

    if (bind_ip != NULL) {
        memset(c, 0, sizeof(c));
        sscanf(bind_ip, "%d.%d.%d.%d", &c[0], &c[1], &c[2], &c[3]);

        for (n = 0; n < 4; n++) {
            if (c[n] < 0 || c[n] > 255) {
                break;
            }
        }
        if (n == 4) {
            server_addr.sin_addr.s_addr = ((ulong)c[0] << 24) | ((ulong)c[1] << 16) | ((ulong)c[2] << 8) | ((ulong)c[3]);
        }
    }
    if (bind(*fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        closesocket(*fd);
        return EST_ERR_NET_BIND_FAILED;
    }
    if (listen(*fd, 10) != 0) {
        closesocket(*fd);
        return EST_ERR_NET_LISTEN_FAILED;
    }
    return 0;
}


/*
    Check if the current operation is blocking
 */
static int net_is_blocking(void)
{
#if WINDOWS || WINCE
    return WSAGetLastError() == WSAEWOULDBLOCK;
#else
    switch (errno) {
#if defined EAGAIN
    case EAGAIN:
#endif
#if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK:
#endif
        return 1;
    }
    return 0;
#endif
}


/*
    Accept a connection from a remote client
 */
int net_accept(int bind_fd, int *client_fd, void *client_ip)
{
    struct sockaddr_in client_addr;

#if defined(__socklen_t_defined) || 1
    socklen_t n = (socklen_t) sizeof(client_addr);
#else
    int n = (int)sizeof(client_addr);
#endif

    *client_fd = accept(bind_fd, (struct sockaddr *) &client_addr, &n);

    if (*client_fd < 0) {
        if (net_is_blocking() != 0) {
            return EST_ERR_NET_TRY_AGAIN;
        }
        return EST_ERR_NET_ACCEPT_FAILED;
    }
    if (client_ip != NULL) {
        memcpy(client_ip, &client_addr.sin_addr.s_addr, sizeof(client_addr.sin_addr.s_addr));
    }
    return 0;
}


/*
    Set the socket blocking or non-blocking
 */
int net_set_block(int fd)
{
#if ME_WIN_LIKE
    long n = 0;
    return ioctlsocket(fd, FIONBIO, &n);
#else
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
#endif
}


int net_set_nonblock(int fd)
{
#if ME_WIN_LIKE
    long n = 1;
    return ioctlsocket(fd, FIONBIO, &n);
#else
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
#endif
}


/*
    Portable usleep helper
 */
void net_usleep(ulong usec)
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = usec;
    select(0, NULL, NULL, NULL, &tv);
}


/*
    Read at most 'len' characters
 */
int net_recv(void *ctx, uchar *buf, int len)
{
    int ret = read(*((int *)ctx), buf, len);

    if (len > 0 && ret == 0) {
        return EST_ERR_NET_CONN_RESET;
    }
    if (ret < 0) {
        if (net_is_blocking() != 0) {
            return EST_ERR_NET_TRY_AGAIN;
        }
#if defined(WIN32) || defined(_WIN32_WCE)
        if (WSAGetLastError() == WSAECONNRESET) {
            return EST_ERR_NET_CONN_RESET;
        }
#else
        if (errno == EPIPE || errno == ECONNRESET) {
            return EST_ERR_NET_CONN_RESET;
        }
        if (errno == EINTR) {
            return EST_ERR_NET_TRY_AGAIN;
        }
#endif
        return EST_ERR_NET_RECV_FAILED;
    }
    return ret;
}


/*
    Write at most 'len' characters
 */
int net_send(void *ctx, uchar *buf, int len)
{
    int ret = send(*((int*)ctx), buf, len, 0);

    if (ret < 0) {
        if (net_is_blocking() != 0) {
            return EST_ERR_NET_TRY_AGAIN;
        }
#if defined(WIN32) || defined(_WIN32_WCE)
        if (WSAGetLastError() == WSAECONNRESET) {
            return EST_ERR_NET_CONN_RESET;
        }
#else
        if (errno == EPIPE || errno == ECONNRESET) {
            return EST_ERR_NET_CONN_RESET;
        }
        if (errno == EINTR) {
            return EST_ERR_NET_TRY_AGAIN;
        }
#endif
        return EST_ERR_NET_SEND_FAILED;
    }
    return ret;
}


/*
    Gracefully close the connection
 */
void net_close(int fd)
{
    shutdown(fd, 2);
    closesocket(fd);
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
