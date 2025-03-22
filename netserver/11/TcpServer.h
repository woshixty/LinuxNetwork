#pragma once

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

class TcpServer
{
private:
    EventLoop loop_;        // 一个TcpServer可以有多个事件循环 目前是单线程 之用一个事件循环

public:
    TcpServer(const std::string& ip, uint16_t port);
    ~TcpServer();

    void start();
};