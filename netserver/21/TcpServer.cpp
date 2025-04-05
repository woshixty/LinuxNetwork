#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip,const uint16_t port)
{
    acceptor_=new Acceptor(&loop_,ip,port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete acceptor_;

    // 释放全部的Connection对象。
    for (auto &aa:conns_)
    {
        delete aa.second;
    }
}

// 运行事件循环。
void TcpServer::start()          
{
    loop_.run();
}

// 处理新客户端连接请求。
void TcpServer::newconnection(Socket* clientsock)
{
    Connection *conn=new Connection(&loop_,clientsock);   
    conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    printf ("new connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());

    conns_[conn->fd()]=conn;            // 把conn存放map容器中。
}

 // 关闭客户端的连接，在Connection类中回调此函数。 
 void TcpServer::closeconnection(Connection *conn)
 {
    printf("client(eventfd=%d) disconnected.\n",conn->fd());
    // close(conn->fd());            // 关闭客户端的fd。
    conns_.erase(conn->fd());   // 从map中删除conn。
    delete conn;
 }

// 客户端的连接错误，在Connection类中回调此函数。
void TcpServer::errorconnection(Connection *conn)
{
    printf("client(eventfd=%d) error.\n",conn->fd());
    // close(conn->fd());            // 关闭客户端的fd。
    conns_.erase(conn->fd());   // 从map中删除conn。
    delete conn;
}

void TcpServer::onmessage(Connection* conn, std::string message)
{
    // 假设在这里经过复杂运算
    message = "reply: " + message;

    // 构造回复消息
    int len = message.size();
    std::string tmpbuf((char*)&len, 4);
    tmpbuf.append(message);

    // 发送回复消息
    send(conn->fd(), tmpbuf.data(), tmpbuf.size(), 0);
}