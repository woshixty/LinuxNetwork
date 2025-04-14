#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip,const uint16_t port, int threadnum)
{
    mainloop_=new EventLoop();
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));

    acceptor_=new Acceptor(mainloop_,ip,port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
    
    threadnum_=threadnum;
    threadpool_=new ThreadPool(threadnum_, "IO");
    for (int ii = 0; ii < threadnum_; ii++)
    {
        subloops_.push_back(new EventLoop());
        subloops_[ii]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
        threadpool_->addtask(std::bind(&EventLoop::run,subloops_[ii]));
    }
}

TcpServer::~TcpServer()
{
    delete acceptor_;
    delete mainloop_;
    for (auto &aa:subloops_)
    {
        delete aa;
    }
    delete threadpool_;
}

// 运行事件循环。
void TcpServer::start()          
{
    mainloop_->run();
}

// 处理新客户端连接请求。
void TcpServer::newconnection(Socket* clientsock)
{
    spConnection conn(new Connection(subloops_[clientsock->fd() % threadnum_], clientsock));
    conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));
    
    conns_[conn->fd()]=conn;            // 把conn存放map容器中。
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