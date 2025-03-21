#include "Channel.h"

Channel::Channel(Epoll* ep, int fd, bool islisten)
    : fd_(fd), ep_(ep), islisten_(islisten)
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

void Channel::handleevent(Socket* servsock)
{
    // 如果发生事件的是客户端socket 表示有客户端发来数据了
    if(revents_ & EPOLLRDHUP)   // 客户端关闭连接 有些系统监测不到 可以使用 EPOLLRDHUP recv() 返回 0
    {
        printf("1 client %d closed\n", fd_);
        close(fd_);
    }
    else if(revents_ & EPOLLIN | EPOLLPRI)  // 接收缓冲区有数据可以写
    {
        // 如果发生事件的是listenfd 表示有新的客户端连接上来了
        if(islisten_ == true)
        {
            // 接受连接请求
            InetAddress clientaddr;
            Socket* clientsock = new Socket(servsock->accept(clientaddr));

            // 打印客户端连接信息：socket、客户端地址和端口
            printf("new client connected, clientsock = %d, clientaddr = %s, clientport = %d\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());
            Channel* clientchannel = new Channel(ep_, clientsock->fd(), false);
            clientchannel->useet();
            clientchannel->enablereading();
        }
        else
        {
            // 普通数据 和 带外数据
            char buffer[1024];
            // 由于使用非阻塞IO 一次性读取buffer大小的数据 直到全部读完
            while(true)
            {
                bzero(&buffer, sizeof(buffer));
                int nread = read(fd_, buffer, sizeof(buffer));
                if(nread > 0)
                {
                    printf("client %d send data: %s\n", fd_, buffer);
                    send(fd_, buffer, nread, 0);
                }
                else if(nread == -1 && errno == EINTR)
                {
                    // 读取数据的时候被信号中断 继续读取
                    printf("continue to read\n");
                    continue;
                }
                else if(nread == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
                {
                    // 所有数据已全部被读取完毕
                    printf("all data read\n");
                    break;
                }
                else if(nread == 0)
                {
                    // 客户端关闭连接
                    printf("2 client %d closed\n", fd_);
                    close(fd_);
                    break;
                }
            }
        }
    }
    else if(revents_ & EPOLLOUT)    // 发送缓冲区有空间可以写
    {}
    else
    {
        // 视为错误 打印socket
        printf("client %d error\n", fd_);
        close(fd_);
    }
} 