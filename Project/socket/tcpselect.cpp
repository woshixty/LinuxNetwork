// 演示采用select模型实现网络通讯的服务端

/**
    <stdio.h>：标准输入输出库，提供如 printf、scanf 等函数。
    <unistd.h>：Unix 标准库，包含如 read、write、close 等系统调用接口。
    <stdlib.h>：标准库函数，包括内存分配、进程控制、转换函数等。
    <string.h>：字符串操作函数，例如 strcpy、strlen、strcat 等。
    <sys/socket.h>：套接字相关函数，用于网络编程，如 socket、bind、listen、accept 等。
    <arpa/inet.h>：IP 地址转换函数，如 inet_addr、inet_ntoa 等。
    <sys/fcntl.h>：文件控制选项，包含 fcntl 函数及相关常量。
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>

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

    // 需要监视读事件socket的集合 大小为16字节（1024位）的bitmap
    // 本质为 int[32] 4*8*32=1024 
    // 下面是四个操作位图的宏
    // void FD_CLR(int fd, fd_set *set);
    // int  FD_ISSET(int fd, fd_set *set);
    // void FD_SET(int fd, fd_set *set);
    // void FD_ZERO(fd_set *set);
    fd_set readfds;
    // 初始化readfds 将bitmap的每一位都置为0
    FD_ZERO(&readfds);
    // 将服务端用于监听的socket加入到readfds中
    FD_SET(listensock, &readfds);

    // readfds中socket的最大值
    int maxfd = listensock;

    while(true)
    {
        // 用于表示超时时间的结构体
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        // select() 中 会修改bitmap 所以需要将readfds拷贝到tmpfds中 再传给select()
        fd_set tmpfds = readfds;
        fd_set tmpfds1 = readfds;

        // select() 等待监视事件的发生（监视那些socket发生了事件）
        // maxfd + 1: 告诉select bitmap有多大
        int infds = select(maxfd + 1, &tmpfds, &tmpfds1, NULL, 0);

        // 如果intfds < 0 表示出错
        if(infds < 0)
        {
            perror("select");
            break;
        }
        // 如果intfds == 0 表示超时
        if(infds == 0)
        {
            printf("timeout\n");
            continue;
        }
        /** 
        for (int eventfd = 0; eventfd <= maxfd; eventfd++)
        {
            if(FD_ISSET(eventfd, &tmpfds1) == 0)
            {
                continue;
            }
            printf("eventfd = %d 可以写\n", eventfd);
        }
        x */

        // 如果intfds > 0 表示有事件发生 intfds就是socket的个数
        for (int eventfd = 0; eventfd <= maxfd; eventfd++)
        {
            // 如果 eventfd在bitmap中的标志为0 表示没有事件
            if(FD_ISSET(eventfd, &tmpfds) == 0)
            {
                continue;
            }
            // 如果发生事件为listenfd 表示已连接队列中有准备好的socket（新客户端连接）
            if(eventfd == listensock)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                int clientsock = accept(listensock, (struct sockaddr*)&client, &len);
                if(clientsock < 0)
                {
                    perror("accept");
                    continue;
                }
                printf("accept client %d\n", clientsock);
                // 将bitmap中新连上来的客户端标志位设为1
                FD_SET(clientsock, &readfds);
                // 更新maxfd的值
                if(maxfd < clientsock)
                {
                    maxfd = clientsock;
                }
            }
            else
            {
                // 如果是客户端连接的socket发生事件 表明接收缓冲区有数据可以读（发送的报文已经送到）或者有新的客户端断开连接
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                if(recv(eventfd, buffer, sizeof(buffer), 0) <= 0)
                {
                    // 如果客户端的连接已经断开
                    printf("client %d disconnect\n", eventfd);
                    close(eventfd);
                    // 将 bitmap 中的该socket标志位设为0
                    FD_CLR(eventfd, &readfds);
                    if(eventfd == maxfd)
                    {
                        for(int ii = maxfd; ii > 0; ii--)
                        {
                            if(FD_ISSET(ii, &readfds))
                            {
                                maxfd = ii;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    // 如果客户端有数据发送过来
                    printf("recv %d bytes from client %d\n", strlen(buffer), eventfd);
                    printf("data is %s\n", buffer);
                    // 将数据原样送回
                    send(eventfd, buffer, strlen(buffer), 0);
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