#pragma once

#include <functional>
#include <string>
#include <memory>
#include <atomic>

#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"

class Connection;
using spConnection = std::shared_ptr<Connection>;    // Connection的智能指针类型，方便使用。

class Connection : public std::enable_shared_from_this<Connection>
{
private:
    const std::unique_ptr<EventLoop>& loop_;           // Connection对应的事件循环，在构造函数中传入。 
    std::unique_ptr<Socket> clientsock_;        // 与客户端通讯的Socket。
    std::unique_ptr<Channel> clientchannel_;    // Connection对应的channel，在构造函数中创建。
    Buffer inputbuffer_;
    Buffer outputbuffer_;
    std::atomic<bool> disconnect_{false};

    std::function<void(spConnection)> closecallback_;    // 关闭fd_的回调函数，将回调TcpServer::closeconnection()。
    std::function<void(spConnection)> errorcallback_;    // fd_发生了错误的回调函数，将回调TcpServer::errorconnection()。
    std::function<void(spConnection, std::string&)> onmessagecallback_;
    std::function<void(spConnection)> sendcompletecallback_;

public:
    Connection(const std::unique_ptr<EventLoop>& loop, std::unique_ptr<Socket> clientsock);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;

    void closecallback();       // TCP连接关闭（断开）的回调函数，供Channel回调。
    void errorcallback();       // TCP连接错误的回调函数，供Channel回调。
    void writecallback();       // 处理写事件的回调函数，供Channel回调。

    void setclosecallback(std::function<void(spConnection)> fn);    // 设置关闭fd_的回调函数。
    void seterrorcallback(std::function<void(spConnection)> fn);    // 设置fd_发生了错误的回调函数。
    void setonmessagecallback(std::function<void(spConnection, std::string&)> fn);
    void setsendcompletecallback(std::function<void(spConnection)> fn);
    void onmessage();

    void send(const char* data, size_t size);
};
