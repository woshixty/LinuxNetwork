// 此程序用于演示采用epoll模型实现网络通讯的服务端
#include "TcpServer.h"

int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.150.128 5085\n\n"); 
        return -1; 
    }
    TcpServer tcpserver(argv[1],atoi(argv[2]));
    tcpserver.start();      // 运行事件循环。

    return 0;
}