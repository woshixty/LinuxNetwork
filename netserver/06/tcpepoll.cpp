// 演示采用epoll模型实现网络通讯的服务端

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>

#include <fcntl.h>
#include <poll.h>

#include "Socket.h"
#include "InetAddress.h"
#include "Epoll.h"

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("Usage: ./tcpepoll ip port\n");
        return -1; 
    }

    Socket servsock(creatnonblocking());
    InetAddress servaddr(argv[1], atoi(argv[2]));
    servsock.setreuseaddr(true);
    servsock.settcpnodelay(true);
    servsock.setreuseport(true);
    servsock.setkeepalive(true);
    servsock.bind(servaddr);
    servsock.listen();

    Epoll ep;
    Channel* servchannel = new Channel(&ep, servsock.fd());
    servchannel->enablereading();

    while(true)
    {
        std::vector<Channel*> channels = ep.loop();
        for(auto& ch : channels)
        {
            // 如果发生事件的是客户端socket 表示有客户端发来数据了
            if(ch->events() & EPOLLRDHUP)   // 客户端关闭连接 有些系统监测不到 可以使用 EPOLLRDHUP recv() 返回 0
            {
                printf("1 client %d closed\n", ch->fd());
                close(ch->fd());
            }
            else if(ch->events() & EPOLLIN | EPOLLPRI)  // 接收缓冲区有数据可以写
            {
                // 如果发生事件的是listenfd 表示有新的客户端连接上来了
                if(ch == servchannel)
                {
                    // 接受连接请求
                    InetAddress clientaddr;
                    Socket* clientsock = new Socket(servsock.accept(clientaddr));

                    // 打印客户端连接信息：socket、客户端地址和端口
                    printf("new client connected, clientsock = %d, clientaddr = %s, clientport = %d\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());
                    Channel* clientchannel = new Channel(&ep, clientsock->fd());
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
                        int nread = read(ch->fd(), buffer, sizeof(buffer));
                        if(nread > 0)
                        {
                            printf("client %d send data: %s\n", ch->fd(), buffer);
                            send(ch->fd(), buffer, nread, 0);
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
                            printf("2 client %d closed\n", ch->fd());
                            close(ch->fd());
                            break;
                        }
                    }
                }
            }
            else if(ch->events() & EPOLLOUT)    // 发送缓冲区有空间可以写
            {}
            else
            {
                // 视为错误 打印socket
                printf("client %d error\n", ch->fd());
                close(ch->fd());
            }
        }
    }
    return 0;
}

int initserver(int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        return -1;
    }
    
    // 设置套接字选项 SO_REUSEADDR，允许地址（IP + 端口）在关闭后立即被重用
    int opt = 1;
    unsigned int len = sizeof(opt);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, len);

    // 将服务端通信的IP和端口绑定值socket
    struct sockaddr_in servaddr;
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(port);
    
    // 绑定服务端ip和端口
    if(::bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        close(sock);
        return -1;
    }
    
    if(::listen(sock, 5) != 0)
    {
        perror("listen");
        close(sock);
        return -1;
    }
    
    return sock;
}