// 封装socket通讯的客户端
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("usage: ./client <ip> <port>\n");
        return -1;
    }
    int sockfd;
    struct sockaddr_in server_addr;
    char buf[1024];
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create socket error\n");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect error");
        return -1;
    }

    printf("connect success\n");

    for(int ii = 0; ii < 200000; ++ii)
    {
        // 从命令行输入内容
        printf("input: ");
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf), stdin);
        if (send(sockfd, buf, strlen(buf), 0) < 0)
        {
            printf("send error\n");
            close(sockfd);
            return -1;
        }
        memset(buf, 0, sizeof(buf));
        if (recv(sockfd, buf, sizeof(buf), 0) < 0)
        {
            printf("recv error\n");
            close(sockfd);
            return -1;
        }
        printf("recv: %s\n", buf);
    }
    return 0;
}