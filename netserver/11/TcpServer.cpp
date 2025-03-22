#include "TcpServer.h"

#include <functional>

TcpServer::TcpServer(const std::string& ip, uint16_t port)
{
    Socket* servsock = new Socket(createnonblocking());
    InetAddress servaddr(ip.c_str(), port);
    servsock->setreuseaddr(true);
    servsock->settcpnodelay(true);
    servsock->setreuseport(true);
    servsock->setkeepalive(true);
    servsock->bind(servaddr);
    servsock->listen();

    Channel* servchannel = new Channel(&loop_, servsock->fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, servsock));
    servchannel->enablereading();
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    loop_.run();
} 