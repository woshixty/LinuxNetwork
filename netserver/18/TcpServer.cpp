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
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newconnection(Socket* clientsock)
{
    Connection* conn = new Connection(&loop_, clientsock);
    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    conns_[clientsock->fd()] = conn;
    printf("new connection(eventfd=%d) from %s:%d\n", conn->fd(), conn->ip().c_str(), conn->port());
}

void TcpServer::closeconnection(Connection* conn)
{
    printf("client(eventfd=%d) disconnected.\n",conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorconnection(Connection* conn)
{
    printf("client(eventfd=%d) error.\n",conn->fd());
    conn->errorcallback();
    delete conn;
}