#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"

// 创建一个非阻塞的socket。
int createnonblocking();

// socket类。
class Socket
{
private:
    const int fd_;      // Socket持有的fd，在构造函数中传进来。
    std::string ip_;    // 如果是服务端socket，ip_成员是服务端的ip地址；如果是客户端socket，ip_成员是对端的ip地址。        
    uint16_t port_;     // 如果是服务端socket，port_成员是服务端的端口号；如果是客户端socket，port_成员是对端的端口号。
    
public:
    Socket(int fd);     // 构造函数，传入一个已准备好的fd。
    ~Socket();          // 在析构函数中，将关闭fd_。

    int fd() const;                 // 返回fd_成员。
    std::string ip() const;         // 返回ip_成员。
    uint16_t port() const;          // 返回port_成员。
    void setreuseaddr(bool on);     // 设置SO_REUSEADDR选项，true-打开，false-关闭。
    void setreuseport(bool on);     // 设置SO_REUSEPORT选项。
    void settcpnodelay(bool on);    // 设置TCP_NODELAY选项。
    void setkeepalive(bool on);     // 设置SO_KEEPALIVE选项。
    void bind(const InetAddress& servaddr);        // 服务端的socket将调用此函数。
    void listen(int nn=128);                                    // 服务端的socket将调用此函数。
    int  accept(InetAddress& clientaddr);            // 服务端的socket将调用此函数。
};