#include "Connection.h"


Connection::Connection(EventLoop *loop,Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    // 为新客户端连接准备读事件，并添加到epoll中。
    clientchannel_=new Channel(loop_,clientsock_->fd());   
    clientchannel_->setreadcallback(std::bind(&Channel::onmessage,clientchannel_));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_->useet();                 // 客户端连上来的fd采用边缘触发。
    clientchannel_->enablereading();   // 让epoll_wait()监视clientchannel的读事件
}

Connection::~Connection()
{
    delete clientsock_;
    delete clientchannel_;
}

int Connection::fd() const                              // 返回客户端的fd。
{
    return clientsock_->fd();
}

std::string Connection::ip() const                   // 返回客户端的ip。
{
    return clientsock_->ip();
}

uint16_t Connection::port() const                  // 返回客户端的port。
{
    return clientsock_->port();
}

void Connection::closecallback()
{
    closecallback_(this);    // 回调TcpServer::closeconnection()。
}

void Connection::errorcallback()
{
    errorcallback_(this);     // 回调TcpServer::errorconnection()。
}

// 设置关闭fd_的回调函数。
void Connection::setclosecallback(std::function<void(Connection*)> fn)    
{
    closecallback_=fn;     // 回调TcpServer::closeconnection()。
}

// 设置fd_发生了错误的回调函数。
void Connection::seterrorcallback(std::function<void(Connection*)> fn)    
{
    errorcallback_=fn;     // 回调TcpServer::errorconnection()。
}