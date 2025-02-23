// 演示socket的客户端
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        cout << "Using: ./demo1 服务端IP 服务端端口\n";
        return -1;
    }
    // Step1 创建客户端socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd==-1) 
    {
        perror("socket failed");
    }
    // Step2 向服务器发送连接请求
    struct hostent* h;  // 用于存放服务器IP的数据结构
    if((h=gethostbyname(argv[1])) == 0)
    {
        cout << "gethostbyname failed\n";
        close(sockfd);
        return -1;
    }
    struct sockaddr_in serveaddr;
    memset(&serveaddr, 0, sizeof(serveaddr));
    serveaddr.sin_family = AF_INET;
    serveaddr.sin_port = htons(atoi(argv[2]));
    memcpy(&serveaddr.sin_addr, h->h_addr, h->h_length);
    // 向服务器发起连接请求
    if(connect(sockfd, (struct sockaddr*)&serveaddr, sizeof(serveaddr)) != 0)
    {
        perror("connect failed");
        close(sockfd);
        return -2;
    }

    // Step3 与服务器通信
    char buffer[1024];
    for (int ii = 0; ii < 3; ii++)
    {
        int iret;
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "这是第%d个超级女声，编号%03d", ii+1, ii+1);
        // 向服务端发送报文请求
        if((iret=send(sockfd, buffer, strlen(buffer), 0)) <= 0)
        {
            perror("send failed");
            break;
        }
        cout << "发送：" << buffer << endl;

        memset(buffer, 0, sizeof(buffer));
        // 接受回应报文
        if((iret=recv(sockfd, buffer, sizeof(buffer), 0)) <= 0)
        {
            cout << "iret=" << iret << endl;
        }
        cout << "接收：" << buffer << endl;
        sleep(1);
    }
    
    // Step4 关闭socket 释放资源
    close(sockfd);
    return 0;
}