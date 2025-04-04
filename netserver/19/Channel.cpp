#include "Channel.h"

Channel::Channel(EventLoop* loop,int fd):loop_(loop),fd_(fd)
{

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
        closecallback_();
    }
    // 接收缓冲区中有数据可以读。
    else if (revents_ & (EPOLLIN|EPOLLPRI))
    {
        // 如果是acceptchannel，将回调Acceptor::newconnection()，如果是clientchannel，将回调Channel::onmessage()。
        readcallback_();
    }
    // 有数据需要写，暂时没有代码，以后再说。
    else if (revents_ & EPOLLOUT)
    {
    }
    // 其它事件，都视为错误。
    else
    {
        errorcallback_();
    }
}

// 处理对端发送过来的消息。
void Channel::onmessage()
{
    char buffer[1024];
    // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    while (true)
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd_, buffer, sizeof(buffer));
        if (nread > 0)      // 成功的读取到了数据。
        {
            // 把接收到的报文内容原封不动的发回去。
            printf("recv(eventfd=%d):%s\n",fd_,buffer);
            send(fd_,buffer,strlen(buffer),0);
        }
        // 读取数据的时候被信号中断，继续读取。
        else if (nread == -1 && errno == EINTR)
        {
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            break;
        }
        // 客户端连接已断开。
        else if (nread == 0)
        {
            closecallback_();
            break;
        }
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