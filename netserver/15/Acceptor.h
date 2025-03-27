#pragma once

#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Connection.h"

class Acceptor
{
private:
    EventLoop* loop_;     // 一个Acceptor只能有一个事件循环 从构造函数传入
    Socket* servsock_;    // 服务端用于监听的套接字 在构造函数里面创建
    Channel* acceptchannel_;    // 服务端用于监听的通道 在构造函数里面创建
    std::function<void(Socket*)> newconnectioncb_; // 新连接的回调函数

public:
    Acceptor(EventLoop* loop, const std::string& ip, uint16_t port);
    ~Acceptor();

    void newconnection();
    void setnewconnectioncallback(std::function<void(Socket*)> fn);
};
