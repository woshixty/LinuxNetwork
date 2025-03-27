#include "TcpServer.h"

#include <functional>

TcpServer::TcpServer(const std::string& ip, uint16_t port)
{
    acceptor_ = new Acceptor(&loop_, ip, port);
    acceptor_->setnewconnectioncallback(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete acceptor_;
    for (auto& conn : conns_) {
        delete conn.second; // 释放连接对象
    }
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newconnection(Socket* clientsock)
{
    Connection* conn = new Connection(&loop_, clientsock);
    printf ("new connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());
    conns_[clientsock->fd()] = conn; // 将新连接加入到连接列表中
}