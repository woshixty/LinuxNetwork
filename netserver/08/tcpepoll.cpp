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
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, &servsock));
    servchannel->enablereading();

    while(true)
    {
        std::vector<Channel*> channels = ep.loop();
        for(auto& ch : channels)
        {
            ch->handleevent();
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