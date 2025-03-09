#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <poll.h>

// 将socket设置为非阻塞
int setnonblocking(int sockfd)
{
    int old_option = fcntl(sockfd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(sockfd, F_SETFL, new_option);
    return old_option;
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("Usage: ./client <host> <port>\n");
        return -1;
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        if(errno == EINPROGRESS)
        {
            perror("socket");
            return -1;
        }
    }
    setnonblocking(sockfd);

    pollfd fds;
    fds.fd = sockfd;
    fds.events = POLLOUT;
    poll(&fds, 1, -1);
    if(fds.revents == POLLOUT)
    {
        printf("Connecting to server...\n");
    }
    else
    {
        printf("Connect to server failed.\n");
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        return -1;
    }

    printf("Connected to server.\n");

    for (int ii = 0; ii < 1000000; ii++)
    {
        // 从控制台输入内容
        memset(buf, 0, sizeof(buf));
        printf("Please input message: ");
        scanf("%s", buf);

        if(send(sockfd, buf, strlen(buf), 0) <= 0)
        {
            perror("send");
            return -1;
        }
        memset(buf, 0, sizeof(buf));
        if(recv(sockfd, buf, sizeof(buf), 0) <= 0)
        {
            perror("recv");
            return -1;
        }
        printf("Received from server: %s\n", buf);
    }
    return 0;
}