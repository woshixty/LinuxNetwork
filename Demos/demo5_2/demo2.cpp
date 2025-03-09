// 演示socket的服务端
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
    if(argc != 2)
    {
        cout << "Using: ./demo2 通讯端口" << endl;
        return -9;
    }
    // Step1 创建服务端socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd==-1)
    {
        perror("socket");
        return -1;
    }
    
    // Step2 将服务端通信的IP和端口绑定值socket
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(atoi(argv[1]));
    // 绑定服务端ip和端口
    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        perror("bind");
        close(listenfd);
        return -2;
    }

    // Step3 将socket设置为可连接状态
    if(listen(listenfd, 5) != 0)
    {
        perror("listen");
        close(listenfd);
        return -3;
    }

    // Step4 受理请求
    int clientfd=accept(listenfd, 0, 0);
    if(clientfd==-1)
    {
        perror("accept");
        close(listenfd);
        return -4;
    }

    cout << "客户端已连接" << endl;

    // Step5 与客户端通信
    char buffer[1024];
    while (true)
    {
        int iret;
        memset(buffer, 0, sizeof(buffer));
        if((iret=recv(clientfd, buffer, sizeof(buffer), 0)) <= 0)
        {
            cout << "iret=" << iret << endl;
            break;
        }
        cout << "接收到：" << buffer << endl;
        
        // 回应客户端
        strcpy(buffer, "ok");
        if((iret=send(clientfd, buffer, strlen(buffer), 0)) <= 0)
        {
            perror("send");
            break;
        }
        cout << "发送到：" << buffer << endl;
    }

    // Step6 关闭socket 释放资源
    close(listenfd);
    close(clientfd);
}