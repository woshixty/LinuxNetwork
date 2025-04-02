#pragma once

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include <map>

// TCP 网络服务类
class TcpServer
{
private:
    EventLoop loop_;        // 一个TcpServer可以有多个事件循环 目前是单线程 之用一个事件循环
    Acceptor* acceptor_;     // 一个TcpServer只能有一个Acceptor
    std::map<int, Connection*> conns_;

public:
    TcpServer(const std::string& ip, uint16_t port);
    ~TcpServer();

    void start();

    void newconnection(Socket* clientsock); 
    void closeconnection(Connection* conn);
    void errorconnection(Connection* conn);
};