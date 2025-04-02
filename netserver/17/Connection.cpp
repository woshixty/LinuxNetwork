#include "Connection.h"

Connection::Connection(EventLoop* loop, Socket* clientsock)
    : loop_(loop), clientsock_(clientsock)
{
    clientchannel_ = new Channel(loop_, clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Channel::onmessage, clientchannel_));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientchannel_->useet();
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    delete clientsock_;
    delete clientchannel_;
}

void Connection::closecallback()
{
    printf("client(eventfd=%d) disconnected.\n",fd());
    close(fd());
}

void Connection::errorcallback()
{
    printf("client(eventfd=%d) error.\n",fd());
    close(fd());
}