/*
 * 程序名：echoserver.cpp，此程序用于演示采用epoll模型实现网络通讯的服务端。
*/
#include "EchoServer.h"

int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./echoserver ip port\n"); 
        printf("example: ./echoserver 192.168.150.128 5085\n\n"); 
        return -1; 
    }

    EchoServer ecgoServer(argv[1],atoi(argv[2]), 3, 0);
    ecgoServer.start();

    return 0;
}