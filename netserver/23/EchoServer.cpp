#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip,const uint16_t port)
    : tcpserver_(ip, port)
{
    // 设置回调函数
    tcpserver_.setnewconnectioncallback(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setclosecallback(std::bind(&EchoServer::HandleCloseConnection, this, std::placeholders::_1));
    tcpserver_.seterrorcallback(std::bind(&EchoServer::HandleErrorConnection, this, std::placeholders::_1));
    tcpserver_.setonmessagecallback(std::bind(&EchoServer::HandleOnMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecallback(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    tcpserver_.setepolltimeoutcallback(std::bind(&EchoServer::HandleEpollTimeout, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{
}

void EchoServer::start()
{
    tcpserver_.start();
}

void EchoServer::HandleNewConnection(Connection *clientsock)
{
    std::cout << "New connection from " << clientsock->ip() << ":" << clientsock->port() << std::endl;
}

void EchoServer::HandleCloseConnection(Connection *conn)
{
    std::cout << "Connection closed: " << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleErrorConnection(Connection *conn)
{
    std::cerr << "Connection error: " << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleOnMessage(Connection* conn, std::string message)
{
    // 假设在这里经过复杂运算
    message = "reply: " + message;

    // 构造回复消息
    int len = message.size();
    std::string tmpbuf((char*)&len, 4);
    tmpbuf.append(message);

    // 发送回复消息
    conn->send(tmpbuf.data(), tmpbuf.size());
}

void EchoServer::HandleSendComplete(Connection* conn)
{
    std::cout << "Send complete for connection: " << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleEpollTimeout(EventLoop* loop)
{
    std::cout << "Epoll timeout occurred." << std::endl;
}