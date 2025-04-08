#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip,const uint16_t port, int threadnum)
    : tcpserver_(ip, port, threadnum)
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

    printf("[%s], Thread ID: %ld\n", __FUNCTION__, syscall(SYS_gettid));
}

void EchoServer::HandleCloseConnection(Connection *conn)
{
    std::cout << "Connection closed: " << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleErrorConnection(Connection *conn)
{
    std::cerr << "Connection error: " << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleOnMessage(Connection* conn, std::string& message)
{
    // 显示线程ID
    printf("[%s], Thread ID: %ld\n", __FUNCTION__, syscall(SYS_gettid));
    // 假设在这里经过复杂运算
    message = "reply: " + message;

    // 发送回复消息
    conn->send(message.data(), message.size());
}

void EchoServer::HandleSendComplete(Connection* conn)
{
    std::cout << "Send complete for connection: " << conn->ip() << ":" << conn->port() << std::endl;
}

void EchoServer::HandleEpollTimeout(EventLoop* loop)
{
    std::cout << "Epoll timeout occurred." << std::endl;
}