#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>

#include "InetAddress.h"

int creatnonblocking();

class Socket
{
private:
    const int fd_;          // Socket持有的fd

public:
    Socket(int fd);
    ~Socket();

    int fd() const;
    void setreuseaddr(bool on);     // 设置SO_REUSEADDR选项
    void setreuseport(bool on);     // 设置SO_REUSEPORT选项
    void settcpnodelay(bool on);    // 设置TCP_NODELAY选项
    void setkeepalive(bool on);     // 设置SO_KEEPALIVE选项
    void bind(const InetAddress& servaddr);
    void listen(int nn = 128);
    int accept(InetAddress& clientaddr);
};