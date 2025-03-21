#include "Epoll.h"

Epoll::Epoll()
{
    epollfd_ = epoll_create(1);
    if(epollfd_ == -1)
    {
        printf("epoll_create error: %d\n", errno);
        exit(-1);
    }
}

Epoll::~Epoll()
{
    ::close(epollfd_);
}

/** 
void Epoll::addfd(int fd, uint32_t op)
{
    // 为服务端的listenfd准备可读事件
    struct epoll_event ev;  // 声明事件的数据结构
    ev.data.fd = fd;        // 指定事件的自定义数据 会随着epoll_wait() 返回的事件一并返回
    ev.events = op;         // 指定事件的类型，EPOLLIN表示可读事件 采用水平触发

    // 将需要监视的socket加入到epollfd中
    if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        printf("epoll_ctl error: %d\n", errno);
        exit(-1);
    }
}
*/
void Epoll::updatechannel(Channel *ch)
{
    epoll_event ev;             // 声明事件的数据结构
    ev.data.ptr = ch;           // 指定channel 会随着epoll_wait() 返回的事件一并返回
    ev.events = ch->events();   // 指定事件的类型

    if(ch->inpoll())
    {
        // 如果已经加入过epoll了，那么只需要修改即可
        if(epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev) == -1)
        {
            perror("epoll_ctl 1 error");
            exit(-1);
        }
    }
    else
    {
        // 如果没有加入过epoll，那么需要添加
        if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev) == -1)
        {
            perror("epoll_ctl 2 error");
            exit(-1); 
        }
    }
}

/** 
std::vector<epoll_event> Epoll::loop(int timeout)
{
    std::vector<epoll_event> evs;
    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epollfd_, events_, MaxEvents, timeout);

    // 如果 infds < 0
    if(infds < 0)
    {
        perror("epoll_wait() failed");
        exit(-1);
    }

    // 超时
    if(infds == 0)
    {
        printf("timeout\n");
        return evs;
    }

    // 如果infds > 0 表示有事件发生的socket的数量
    // 遍历epoll返回的数组events
    for(int ii = 0; ii < infds; ++ii)
    {
        evs.push_back(events_[ii]);
    }
    return evs;
}
*/
std::vector<Channel *> Epoll::loop(int timeout)
{
    std::vector<Channel *> channels;
    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epollfd_, events_, MaxEvents, timeout);
    if(infds < 0)
    {
        perror("epoll_wait() failed");
        exit(-1);
    }
    if(infds == 0)
    {
        printf("timeout\n");
        return channels;
    }
    for(int ii = 0; ii < infds; ++ii)
    {
        Channel *ch = static_cast<Channel *>(events_[ii].data.ptr);
        ch->setrevents(events_[ii].events);
        channels.push_back(ch);
    }
    return channels;
}