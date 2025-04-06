#pragma once
#include <functional>
#include <string>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"

class Connection
{
private:
    EventLoop *loop_;           // Connection对应的事件循环，在构造函数中传入。 
    Socket *clientsock_;        // 与客户端通讯的Socket。
    Channel *clientchannel_;    // Connection对应的channel，在构造函数中创建。
    Buffer inputbuffer_;
    Buffer outputbuffer_;

    std::function<void(Connection*)> closecallback_;    // 关闭fd_的回调函数，将回调TcpServer::closeconnection()。
    std::function<void(Connection*)> errorcallback_;    // fd_发生了错误的回调函数，将回调TcpServer::errorconnection()。
    std::function<void(Connection*, std::string)> onmessagecallback_;

public:
    Connection(EventLoop *loop,Socket *clientsock);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;

    void closecallback();       // TCP连接关闭（断开）的回调函数，供Channel回调。
    void errorcallback();       // TCP连接错误的回调函数，供Channel回调。
    void writecallback();       // 处理写事件的回调函数，供Channel回调。

    void setclosecallback(std::function<void(Connection*)> fn);    // 设置关闭fd_的回调函数。
    void seterrorcallback(std::function<void(Connection*)> fn);    // 设置fd_发生了错误的回调函数。
    void setonmessagecallback(std::function<void(Connection*, std::string)> fn);
    void onmessage();

    void send(const char* data, size_t size);
};
