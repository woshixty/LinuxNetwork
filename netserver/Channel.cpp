#include "Channel.h"

Channel::Channel(const std::unique_ptr<EventLoop>& loop,int fd)
    : loop_(loop), fd_(fd)
{
    printf("Channel::Channel()\n");
}

Channel::~Channel()
{
    // 在析构函数中，不要销毁loop_，也不能关闭fd_，因为这两个东西不属于Channel类，Channel类只是需要它们，使用它们而已。
}

int Channel::fd()
{
    return fd_;
}

void Channel::useet()
{
    events_=events_|EPOLLET;
}

void Channel::enablereading()
{
    events_|=EPOLLIN;
    loop_->updatechannel(this);
}

void Channel::disablereading()
{
    events_&=~EPOLLIN;
    loop_->updatechannel(this);
}

void Channel::enablewriting()
{
    events_|=EPOLLOUT;
    loop_->updatechannel(this);
}

void Channel::disablewriting()
{
    events_&=~EPOLLOUT;
    loop_->updatechannel(this);
}

void Channel::disableall()
{
    events_=0;
    loop_->updatechannel(this);
}

void Channel::remove()
{
    disableall();
    loop_->removechannel(this);
}

void Channel::setinepoll()
{
    inepoll_=true;
}

void Channel::setrevents(uint32_t ev)
{
    revents_=ev;
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

// 事件处理函数，epoll_wait()返回的时候，执行它。
void Channel::handleevent()
{
    // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
    if (revents_ & EPOLLRDHUP)
    {
        printf("EPOLLRDHUP\n");
        closecallback_();
    }
    // 接收缓冲区中有数据可以读。
    else if (revents_ & (EPOLLIN|EPOLLPRI))
    {
        printf("EPOLLIN|EPOLLPRI\n");
        if (readcallback_)  // 检查回调是否已设置
        {
            readcallback_();
        }
        else
        {
            fprintf(stderr, "Error: readcallback_ is not set!\n");
        }
    }
    // 有数据需要写，暂时没有代码，以后再说。
    else if (revents_ & EPOLLOUT)
    {
        printf("EPOLLOUT\n");
        writecallback_();
    }
    // 其它事件，都视为错误。
    else
    {
        errorcallback_();
    }
}

// 设置fd_读事件的回调函数。
void Channel::setreadcallback(std::function<void()> fn)    
{
    readcallback_=fn;
}

// 设置关闭fd_的回调函数。
void Channel::setclosecallback(std::function<void()> fn)    
{
    closecallback_=fn;
}

// 设置fd_发生了错误的回调函数。
void Channel::seterrorcallback(std::function<void()> fn)    
{
    errorcallback_=fn;
}

void Channel::setwritecallback(std::function<void()> fn)
{
    writecallback_=fn;
}