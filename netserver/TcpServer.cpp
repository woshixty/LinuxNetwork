#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip,const uint16_t port, int threadnum)
    : threadnum_(threadnum),
    mainloop_(new EventLoop()), 
    acceptor_(mainloop_, ip, port),
    threadpool_(threadnum, "IO")
{
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));

    acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
    
    for (int ii = 0; ii < threadnum_; ii++)
    {
        subloops_.emplace_back(std::make_unique<EventLoop>());
        subloops_[ii]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
        threadpool_.addtask(std::bind(&EventLoop::run,subloops_[ii].get()));
    }
}

TcpServer::~TcpServer()
{}

// 运行事件循环。
void TcpServer::start()          
{
    mainloop_->run();
}

// 处理新客户端连接请求。
void TcpServer::newconnection(std::unique_ptr<Socket> clientsock)
{
    printf("TcpServer::newconnection()\n");
    spConnection conn = std::make_shared<Connection>(
        subloops_[clientsock->fd() % threadnum_], 
        std::move(clientsock)
    );
    printf("fd=%d, ip=%s, port=%d\n", conn->fd(), conn->ip().c_str(), conn->port());
    conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));
    printf("fd=%d, ip=%s, port=%d\n", conn->fd(), conn->ip().c_str(), conn->port());
    // 把conn存放map容器中。
    conns_[conn->fd()]=conn;

    printf("fd=%d, ip=%s, port=%d\n", conn->fd(), conn->ip().c_str(), conn->port());
    newconnectioncb_(conn);
}

 // 关闭客户端的连接，在Connection类中回调此函数。 
 void TcpServer::closeconnection(spConnection conn)
 {
    closecb_(conn);
    conns_.erase(conn->fd());
 }

// 客户端的连接错误，在Connection类中回调此函数。
void TcpServer::errorconnection(spConnection conn)
{
    errorcb_(conn);
    conns_.erase(conn->fd());
}

void TcpServer::onmessage(spConnection conn, std::string& message)
{
    onmessagecb_(conn, message);
}

void TcpServer::sendcomplete(spConnection conn)
{
    // 根据业务需求增加代码
    sendcompletecb_(conn);
}

void TcpServer::epolltimeout(EventLoop* loop)
{
    // 根据业务需求增加代码
    epolltimeoutcb_(loop);
}

void TcpServer::setnewconnectioncallback(std::function<void(spConnection)> fn)
{
    newconnectioncb_=fn;
}