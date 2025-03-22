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
#include "EventLoop.h"

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

    EventLoop loop;
    Channel* servchannel = new Channel(loop.ep(), servsock.fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, &servsock));
    servchannel->enablereading();
    loop.run();

    return 0;
}