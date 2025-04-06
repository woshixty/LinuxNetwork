#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include <map>

// TCP网络服务类。
class TcpServer
{
private:
    EventLoop loop_;       // 一个TcpServer可以有多个事件循环，现在是单线程，暂时只用一个事件循环。
    Acceptor *acceptor_;   // 一个TcpServer只有一个Acceptor对象。
    std::map<int,Connection*>  conns_;           // 一个TcpServer有多个Connection对象，存放在map容器中。
    
    std::function<void(Connection*)> newconnectioncb_;    // 处理新客户端连接请求的回调函数，将指向TcpServer::newconnection()
    std::function<void(Connection*)> closecb_;    // 关闭fd_的回调函数，将回调TcpServer::closeconnection()。
    std::function<void(Connection*)> errorcb_;    // fd_发生了错误的回调函数，将回调TcpServer::errorconnection()。
    std::function<void(Connection*, std::string)> onmessagecb_;
    std::function<void(Connection*)> sendcompletecb_;
    std::function<void(EventLoop*)> epolltimeoutcb_;

public:
    TcpServer(const std::string &ip,const uint16_t port);
    ~TcpServer();

    void start();          // 运行事件循环。

    void newconnection(Socket *clientsock);    // 处理新客户端连接请求，在Acceptor类中回调此函数。
    void closeconnection(Connection *conn);  // 关闭客户端的连接，在Connection类中回调此函数。 
    void errorconnection(Connection *conn);  // 客户端的连接错误，在Connection类中回调此函数。
    void onmessage(Connection* conn, std::string message);
    void sendcomplete(Connection* conn);
    void epolltimeout(EventLoop* loop);

    void setnewconnectioncallback(std::function<void(Connection*)> fn) { newconnectioncb_ = fn; }    // 设置处理新客户端连接请求的回调函数。
    void setclosecallback(std::function<void(Connection*)> fn) { closecb_ = fn; }    // 设置关闭fd_的回调函数。
    void seterrorcallback(std::function<void(Connection*)> fn) { errorcb_ = fn; }    // 设置fd_发生了错误的回调函数。
    void setonmessagecallback(std::function<void(Connection*, std::string)> fn) { onmessagecb_ = fn; }
    void setsendcompletecallback(std::function<void(Connection*)> fn) { sendcompletecb_ = fn; }    // 设置发送完成的回调函数。
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn) { epolltimeoutcb_ = fn; }    // 设置epoll超时的回调函数。
};