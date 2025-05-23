#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip,const uint16_t port,int threadnum)
                 :threadnum_(threadnum),mainloop_(new EventLoop(true)), acceptor_(mainloop_.get(),ip,port),threadpool_(threadnum_,"IO")
{
    // mainloop_=new EventLoop;       // 创建主事件循环。
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));   // 设置timeout超时的回调函数。

    // acceptor_=new Acceptor(mainloop_,ip,port);
    acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));

    // threadpool_=new ThreadPool(threadnum_,"IO");       // 创建线程池。

    // 创建从事件循环。
    for (int ii=0;ii<threadnum_;ii++)
    {
        // 创建从事件循环，存入subloops_容器中。
        subloops_.emplace_back(new EventLoop(false, 5, 10));
        // 设置timeout超时的回调函数。
        subloops_[ii]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
        subloops_[ii]->settimercallback(std::bind(&TcpServer::removeconnection,this,std::placeholders::_1));
        // 在线程池中运行从事件循环。
        threadpool_.addtask(std::bind(&EventLoop::run,subloops_[ii].get()));

        sleep(1);
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
    int tmpFd = clientsock->fd();
    // 把新建的conn分配给从事件循环。
    spConnection conn(new Connection(subloops_[tmpFd%threadnum_].get(),std::move(clientsock)));   
    conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));

    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_[conn->fd()]=conn;
    }
    subloops_[tmpFd%threadnum_]->newconnections(conn);

    // 回调EchoServer::HandleNewConnection()。
    if (newconnectioncb_) newconnectioncb_(conn);
}

 // 关闭客户端的连接，在Connection类中回调此函数。 
 void TcpServer::closeconnection(spConnection conn)
 {
    // 回调EchoServer::HandleClose()。
    if (closeconnectioncb_) closeconnectioncb_(conn);

    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_.erase(conn->fd());
    }
 }

// 客户端的连接错误，在Connection类中回调此函数。
void TcpServer::errorconnection(spConnection conn)
{
    // 回调EchoServer::HandleError()。
    if (errorconnectioncb_) errorconnectioncb_(conn);
    
    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_.erase(conn->fd());
    }
}

// 处理客户端的请求报文，在Connection类中回调此函数。
void TcpServer::onmessage(spConnection conn,const std::string& message)
{
    // 回调EchoServer::HandleMessage()。
    if (onmessagecb_) onmessagecb_(conn,message);
}

// 数据发送完成后，在Connection类中回调此函数。
void TcpServer::sendcomplete(spConnection conn)     
{
    // 回调EchoServer::HandleSendComplete()。
    if (sendcompletecb_) sendcompletecb_(conn);
}

// epoll_wait()超时，在EventLoop类中回调此函数。
void TcpServer::epolltimeout(EventLoop *loop)         
{
    // 回调EchoServer::HandleTimeOut()。
    if (timeoutcb_)
    {
        timeoutcb_(loop);
    }
}

void TcpServer::setnewconnectioncb(std::function<void(spConnection)> fn)
{
    newconnectioncb_=fn;
}

void TcpServer::setcloseconnectioncb(std::function<void(spConnection)> fn)
{
    closeconnectioncb_=fn;
}

void TcpServer::seterrorconnectioncb(std::function<void(spConnection)> fn)
{
    errorconnectioncb_=fn;
}

void TcpServer::setonmessagecb(std::function<void(spConnection,const std::string &message)> fn)
{
    onmessagecb_=fn;
}

void TcpServer::setsendcompletecb(std::function<void(spConnection)> fn)
{
    sendcompletecb_=fn;
}

void TcpServer::settimeoutcb(std::function<void(EventLoop*)> fn)
{
    timeoutcb_=fn;
}

void TcpServer::removeconnection(int fd)
{
    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_.erase(fd);
    }
}