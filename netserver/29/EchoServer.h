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
    void HandleNewConnection(Connection *clientsock);    // 处理新客户端连接请求，在Acceptor类中回调此函数。
    void HandleCloseConnection(Connection *conn);  // 关闭客户端的连接，在Connection类中回调此函数。 
    void HandleErrorConnection(Connection *conn);  // 客户端的连接错误，在Connection类中回调此函数。

    void HandleOnMessage(Connection* conn, std::string& message);
    void HandleSendComplete(Connection* conn);
    void HandleEpollTimeout(EventLoop* loop);

    void OnMessage(Connection* conn, std::string message);
};
