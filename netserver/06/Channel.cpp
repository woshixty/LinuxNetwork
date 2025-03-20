#include "Channel.h"

Channel::Channel(Epoll* ep, int fd)
    : fd_(fd), ep_(ep)
{
}

Channel::~Channel()
{
    // 析构函数中 不要销毁ep_和fd_ 因为这两个东西不属于Channel类 Channel类只是需要他们 使用它们
}

int Channel::fd()
{
    return fd_;
}

void Channel::useet()
{
    events_ |= EPOLLET;
}

void Channel::enablereading()
{
    events_ |= EPOLLIN;
    ep_->updatechannel(this);
}

void Channel::setinepoll()
{
    inepoll_ = true;
}

void Channel::setrevents(uint32_t ev)
{
    revents_ = ev;
}

bool Channel::inpoll()
{
    return inepoll_;
}

uint32_t Channel::events()
{
    return events_;
}

uint32_t Channel::revents()
{
    return revents_;
}