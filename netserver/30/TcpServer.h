#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <map>

// TCP网络服务类。
class TcpServer
{
private:
    EventLoop *mainloop_;
    std::vector<EventLoop *> subloops_;
    ThreadPool *threadpool_;
    int threadnum_;
    Acceptor *acceptor_;
    std::map<int,spConnection> conns_;
    
    std::function<void(spConnection)> newconnectioncb_;    // 处理新客户端连接请求的回调函数，将指向TcpServer::newconnection()
    std::function<void(spConnection)> closecb_;    // 关闭fd_的回调函数，将回调TcpServer::closeconnection()。
    std::function<void(spConnection)> errorcb_;    // fd_发生了错误的回调函数，将回调TcpServer::errorconnection()。
    std::function<void(spConnection, std::string&)> onmessagecb_;
    std::function<void(spConnection)> sendcompletecb_;
    std::function<void(EventLoop*)> epolltimeoutcb_;

public:
    TcpServer(const std::string &ip,const uint16_t port, int threadnum=4);    // 构造函数，创建一个TcpServer对象，传入ip和端口号。
    ~TcpServer();

    void start();          // 运行事件循环。

    void newconnection(Socket *clientsock);    // 处理新客户端连接请求，在Acceptor类中回调此函数。
    void closeconnection(spConnection conn);  // 关闭客户端的连接，在Connection类中回调此函数。 
    void errorconnection(spConnection conn);  // 客户端的连接错误，在Connection类中回调此函数。
    void onmessage(spConnection conn, std::string& message);
    void sendcomplete(spConnection conn);
    void epolltimeout(EventLoop* loop);

    void setnewconnectioncallback(std::function<void(spConnection)> fn) { newconnectioncb_ = fn; }    // 设置处理新客户端连接请求的回调函数。
    void setclosecallback(std::function<void(spConnection)> fn) { closecb_ = fn; }    // 设置关闭fd_的回调函数。
    void seterrorcallback(std::function<void(spConnection)> fn) { errorcb_ = fn; }    // 设置fd_发生了错误的回调函数。
    void setonmessagecallback(std::function<void(spConnection, std::string&)> fn) { onmessagecb_ = fn; }
    void setsendcompletecallback(std::function<void(spConnection)> fn) { sendcompletecb_ = fn; }    // 设置发送完成的回调函数。
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn) { epolltimeoutcb_ = fn; }    // 设置epoll超时的回调函数。
};