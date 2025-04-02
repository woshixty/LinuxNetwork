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
    std::function<void(Connection*)> closecallback_;    // 关闭连接的回调函数
    std::function<void(Connection*)> errorcallback_;    // 错误的回调函数

public:
    Connection(EventLoop* loop, Socket* clientsock);
    ~Connection();

    int fd() const { return clientsock_->fd(); }    // 返回fd_成员。
    std::string ip() const { return clientsock_->ip(); }    // 返回ip_成员。
    uint16_t port() const { return clientsock_->port(); }    // 返回port_成员。

    void closecallback();    // 关闭连接的回调函数
    void errorcallback();    // 错误的回调函数

    void setclosecallback(std::function<void(Connection*)> fn) { closecallback_ = fn; }   // 设置关闭连接的回调函数
    void seterrorcallback(std::function<void(Connection*)> fn) { errorcallback_ = fn; }   // 设置错误的回调函数
};