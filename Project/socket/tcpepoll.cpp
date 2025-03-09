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

// 初始化服务端监听端口
int initserver(int port);

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: ./tcpselect port\n");
        return -1; 
    }

    // 初始化服务端用于监听的socket
    int listensock = initserver(atoi(argv[1]));
    printf("listensock = %d\n", listensock);

    if(listensock < 0)
    {
        printf("initserver error\n");
        return -1;
    }

    // 创建epoll实例
    int epollfd = epoll_create(1);

    // 为服务端的listensock转杯可读事件
    struct epoll_event ev;      // 声明事件的数据结构
    ev.data.fd = listensock;    // 指定事件的自定义数据 会随着epoll_wait() 返回的事件一并返回
    ev.events = EPOLLIN;        // 指定事件的类型，EPOLLIN表示可读事件

    // 将需要监视的socket加入到epollfd中
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listensock, &ev);
    // 存放epoll返回的事件
    epoll_event events[10];

    while(true)
    {
        // 等待监视的 socket 事件发生
        int infds = epoll_wait(epollfd, events, 10, -1);

        // 如果 infds < 0
        if(infds < 0)
        {
            perror("epoll_wait");
            break;
        }

        // 超时
        if(infds == 0)
        {
            printf("timeout\n");
            continue;
        }

        // 如果infds > 0 比哦是有事件发生的socket的数量
        // 遍历epoll返回的数组events
        for(int i = 0; i < infds; ++i)
        {
            // 如果发生事件的是listensock 表示有新的客户端连接上来了
            if(events[i].data.fd == listensock)
            {
                // 接受连接请求
                struct sockaddr_in clientaddr;
                socklen_t clientlen = sizeof(clientaddr);
                int clientsock = accept(listensock, (struct sockaddr*)&clientaddr, &clientlen);
                
                printf("new client connected, clientsock = %d\n", clientsock);

                // 为新的客户端socket设置可读事件
                ev.data.fd = clientsock;
                ev.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock, &ev);
            }
            else
            {
                // 如果发生事件的是客户端socket 表示有客户端发来数据了
                // 接收客户端发来的数据
                char buf[1024];
                memset(buf, 0, sizeof(buf));
                int ret = recv(events[i].data.fd, buf, sizeof(buf), 0);
                if(ret <= 0)
                {
                    printf("client closed, clientsock = %d\n", events[i].data.fd);
                    close(events[i].data.fd);
                    // 将客户端socket从epoll中移除
                    // 如果socket被关闭了，会自动从epoll中自动移除
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                }
                else if(ret > 0)
                {
                    printf("recv %d bytes from client, clientsock = %d, buf = %s\n", ret, events[i].data.fd, buf);
                    send(events[i].data.fd, buf, ret, 0);
                }
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