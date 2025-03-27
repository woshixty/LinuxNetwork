#pragma once

#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class Connection
{
private:
    EventLoop* loop_;     // 一个Acceptor只能有一个事件循环 从构造函数传入
    Socket* clientsock_;    // 服务端用于监听的套接字 在构造函数里面创建
    Channel* clientchannel_;    // 服务端用于监听的通道 在构造函数里面创建
public:
    Connection(EventLoop* loop, Socket* clientsock);
    ~Connection();

    int fd();
    std::string ip();
    uint16_t port();
};