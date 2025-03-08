// 演示采用poll模型实现网络通讯的服务端

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
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

    // fds 存放需要监视的socket
    pollfd fds[2048];

    // 初始化数组 将所有的socket置为-1 如果数组中的socket值为-1 则忽略
    for(int ii = 0; ii < 2048; ii++)
    {
        fds[ii].fd = -1;
    }

    // 让poll监视listensock读事件
    fds[listensock].fd = listensock;
    fds->events = POLLIN;

    int maxfd = listensock;
    while(true)
    {
        // 调用 poll() 等待事件的发生（监视那些socket发生了事件）
        int infds = poll(fds, maxfd + 1, 10000);
        // 如果infds < 0 表示出错
        if(infds < 0)
        {
            perror("poll");
            continue;
        }
        // 如果infds == 0 表示超时
        if(infds == 0)
        {
            printf("timeout\n");
            continue;
        }
        // 如果infds > 0 表示有socket发生了事件
        for (int eventfd = 0; eventfd < maxfd; eventfd++)
        {
            if(fds[eventfd].fd < 0)
            {
                continue;
            }
            if((fds[eventfd].revents & POLLIN) == 0)
            {
                continue;
            }

            // 若发生事件的是listensock 则表示有新的客户端连接
            if(eventfd == listensock)
            {
                struct sockaddr_in client;
                socklen_t clientlen = sizeof(client);
                int clientsock = accept(listensock, (struct sockaddr*)&client, &clientlen);
                if(clientsock < 0)
                {
                    perror("accept");
                    continue;
                }

                printf("clientsock = %d\n", clientsock);

                // 修改fds数组中clientsock位置的元素
                fds[clientsock].fd = clientsock;
                fds[clientsock].events = POLLIN;

                if(maxfd < clientsock)
                {
                    maxfd = clientsock;
                }
            }
            else
            {
                // 如果是连上的客户端有事件，有报文发送过来或者连接已断开
                char buf[1024];
                int ret = recv(eventfd, buf, sizeof(buf), 0);
                if(ret <= 0)
                {
                    // 客户端连接已断开
                    printf("client %d disconnect\n", eventfd);
                    close(eventfd);
                    fds[eventfd].fd = -1;

                    // 重新计算maxfd的值，只有当maxfd对应的socket被关闭时，才需要重新计算maxfd的值
                    if (eventfd == maxfd)
                    {
                        for(int ii = maxfd; ii >= 0; ii--)
                        {
                            if(fds[ii].fd == eventfd)
                            {
                                maxfd = ii - 1;
                                break;
                            }
                        }
                    }
                } else if(ret > 0)
                {
                    printf("recv %d bytes from %d\n", ret, eventfd);
                    buf[ret] = 0;
                    printf("buf = %s\n", buf);
                    send(eventfd, buf, ret, 0);
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