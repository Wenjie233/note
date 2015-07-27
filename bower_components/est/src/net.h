/*
    net.h -- Networking I/O

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_NET_H
#define EST_NET_H

#define EST_ERR_NET_UNKNOWN_HOST                      -0x0F00
#define EST_ERR_NET_SOCKET_FAILED                     -0x0F10
#define EST_ERR_NET_CONNECT_FAILED                    -0x0F20
#define EST_ERR_NET_BIND_FAILED                       -0x0F30
#define EST_ERR_NET_LISTEN_FAILED                     -0x0F40
#define EST_ERR_NET_ACCEPT_FAILED                     -0x0F50
#define EST_ERR_NET_RECV_FAILED                       -0x0F60
#define EST_ERR_NET_SEND_FAILED                       -0x0F70
#define EST_ERR_NET_CONN_RESET                        -0x0F80
#define EST_ERR_NET_TRY_AGAIN                         -0x0F90

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          Initiate a TCP connection with host:port
       @return         0 if successful, or one of:
                            EST_ERR_NET_SOCKET_FAILED,
                            EST_ERR_NET_UNKNOWN_HOST,
                            EST_ERR_NET_CONNECT_FAILED
     */
    PUBLIC int net_connect(int *fd, char *host, int port);

    /**
       @brief          Create a listening socket on bind_ip:port.
                       If bind_ip == NULL, all interfaces are binded.
       @return         0 if successful, or one of:
                            EST_ERR_NET_SOCKET_FAILED,
                            EST_ERR_NET_BIND_FAILED,
                            EST_ERR_NET_LISTEN_FAILED
     */
    PUBLIC int net_bind(int *fd, char *bind_ip, int port);

    /**
       @brief          Accept a connection from a remote client
       @return         0 if successful, EST_ERR_NET_ACCEPT_FAILED, or EST_ERR_NET_WOULD_BLOCK is bind_fd was set to
                       non-blocking and accept() is blocking.
     */
    PUBLIC int net_accept(int bind_fd, int *client_fd, void *client_ip);

    /**
       @brief          Set the socket blocking
       @return         0 if successful, or a non-zero error code
     */
    PUBLIC int net_set_block(int fd);

    /**
       @brief          Set the socket non-blocking
       @return         0 if successful, or a non-zero error code
     */
    PUBLIC int net_set_nonblock(int fd);

    /**
       @brief          Portable usleep helper
       @note           Real amount of time slept will not be less than select()'s timeout granularity (typically, 10ms).
     */
    PUBLIC void net_usleep(ulong usec);

    /**
       @brief          Read at most 'len' characters. len is updated to
                       reflect the actual number of characters read.
       @return         This function returns the number of bytes received, or a negative error code; EST_ERR_NET_TRY_AGAIN
                       indicates read() is blocking.
     */
    PUBLIC int net_recv(void *ctx, uchar *buf, int len);

    /**
       @brief          Write at most 'len' characters. len is updated to
                       reflect the number of characters _not_ written.
       @return         This function returns the number of bytes sent, or a negative error code; EST_ERR_NET_TRY_AGAIN
                       indicates write() is blocking.
     */
    PUBLIC int net_send(void *ctx, uchar *buf, int len);

    /**
       @brief          Gracefully shutdown the connection
     */
    PUBLIC void net_close(int fd);

#ifdef __cplusplus
}
#endif
#endif              /* net.h */

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
