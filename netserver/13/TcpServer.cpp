#include "TcpServer.h"

#include <functional>

TcpServer::TcpServer(const std::string& ip, uint16_t port)
{
    acceptor_ = new Acceptor(&loop_, ip, port);
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    loop_.run();
} 