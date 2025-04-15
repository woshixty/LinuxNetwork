#pragma once

#include "TcpServer.h"
#include "ThreadPool.h"

#include <string>

class EchoServer
{
private:
    TcpServer tcpserver_;
    ThreadPool threadpool_;

public:
    EchoServer(const std::string &ip,const uint16_t port, int subthreadnum=3, int workthreadnum=5);
    ~EchoServer();

    void start();
    void HandleNewConnection(spConnection clientsock);    // 处理新客户端连接请求，在Acceptor类中回调此函数。
    void HandleCloseConnection(spConnection conn);  // 关闭客户端的连接，在Connection类中回调此函数。 
    void HandleErrorConnection(spConnection conn);  // 客户端的连接错误，在Connection类中回调此函数。

    void HandleOnMessage(spConnection conn, std::string& message);
    void HandleSendComplete(spConnection conn);
    void HandleEpollTimeout(EventLoop* loop);

    void OnMessage(spConnection conn, std::string message);
};
