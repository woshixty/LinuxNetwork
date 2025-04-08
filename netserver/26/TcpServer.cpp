#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip,const uint16_t port)
{
    acceptor_=new Acceptor(&loop_,ip,port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
    loop_.setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
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
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));
    
    conns_[conn->fd()]=conn;            // 把conn存放map容器中。
    newconnectioncb_(conn);
}

 // 关闭客户端的连接，在Connection类中回调此函数。 
 void TcpServer::closeconnection(Connection *conn)
 {
    closecb_(conn);
    conns_.erase(conn->fd());
    delete conn;
 }

// 客户端的连接错误，在Connection类中回调此函数。
void TcpServer::errorconnection(Connection *conn)
{
    errorcb_(conn);
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::onmessage(Connection* conn, std::string& message)
{
    onmessagecb_(conn, message);
}

void TcpServer::sendcomplete(Connection* conn)
{
    // 根据业务需求增加代码
    sendcompletecb_(conn);
}

void TcpServer::epolltimeout(EventLoop* loop)
{
    // 根据业务需求增加代码
    epolltimeoutcb_(loop);
}