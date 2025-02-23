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

using namespace std;

// TCP通讯的客户端类
class ctcpclient
{
public:
    int m_clientfd;         // 客户端的socket -1 表示未连接或者连接已经断开 >=0 表示有效的socket
    string m_ip;            // 服务端的IP/域名
    unsigned short m_port;  // 通讯端口

    ctcpclient(): m_clientfd(-1) {}

    // 向服务端发送连接请求，成功返回true 失败返回false
    bool connect(const string& in_ip, const unsigned short in_port)
    {
        // 判断socket是否已连接，已连接直接返回失败
        if(m_clientfd != -1) 
        {
            return false;
        }

        m_ip = in_ip;
        m_port = in_port;

        m_clientfd = socket(AF_INET, SOCK_STREAM, 0);
        if(m_clientfd == -1)
        {
            perror("socket");
            return false;
        }

        struct sockaddr_in servaddr;                // 用于存放协议、端口、IP地址的结构体
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;    // 1 协议族 固定填AF_INET
        servaddr.sin_port = htons(m_port);          // 2 指定服务端的通信端口

        struct hostent* h;  // 用于存放服务端IP地址（大端序）结构体指针
        if((h = gethostbyname(m_ip.c_str())) == nullptr) // 把 域名/主机名/字符串格式的IP 转换为结构体
        {
            cout << "gethostbyname failed" << endl;
            ::close(m_clientfd);
            return false;
        }

        memcpy(&servaddr.sin_addr, h->h_addr, h->h_length); // 3 指定服务端的IP（大端序）
        // 向服务端发起连接请求
        if(::connect(m_clientfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
        {
            perror("connect");
            ::close(m_clientfd);
            return false;
        }
        return true;
    }

    // 向服务端发送报文请求
    // buffer 不要用 const char* ，因为 const char* 只支持c风格的字符串，不支持string
    bool send(const string& buffer) 
    {
        if(m_clientfd == -1)
        {
            return false;
        }
        if((::send(m_clientfd, buffer.c_str(), buffer.size(), 0)) <= 0)
        {
            perror("send failed");
            return false;
        }
        return true;
    }

    bool recv(string& buffer, size_t maxlen)
    {
        if(m_clientfd == -1)
        {
            return false;
        }
        buffer.clear();         // 清空容器
        buffer.resize(maxlen);  // 设置容器的大小为maxlen
        int readn = ::recv(m_clientfd, &buffer[0], buffer.size(), 0);   // 直接操作buffer的内存
        if(readn <= 0)
        {
            return false;
        }
        buffer.resize(readn);   // 重置buffer的实际大小
        return true;
    }

    // 断开与服务端的连接
    bool close()
    {
        if(m_clientfd == -1)
        {
            return false;
        }
        ::close(m_clientfd);
        m_clientfd = -1;
        return true;
    }

    ~ctcpclient() 
    {
        close();
    }
};

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        cout << "Using: ./demo1 服务端IP 服务端端口\n";
        return -1;
    }

    ctcpclient tcpclient;
    bool bRet = tcpclient.connect(argv[1], atoi(argv[2]));
    if(!bRet)
    {
        perror("connect()");
        return -1;
    }

    // Step3 与服务器通信
    string buffer;
    for (int ii = 0; ii < 3; ii++)
    {
        buffer = "这是第" + std::to_string(ii+1) + "个超级女声，编号" + std::to_string(ii+1);
        // 向服务端发送报文请求
        bRet = tcpclient.send(buffer);
        if(!bRet)
        {  
            perror("send failed");
            break;
        }
        cout << "发送：" << buffer << endl;

        // 接受回应报文
        bRet = tcpclient.recv(buffer, 1024);
        if(!bRet)
        {
            cout << "recv failed" << endl;
            break;
        }
        cout << "接收：" << buffer << endl;
        sleep(1);
    }
    
    // Step4 关闭socket 释放资源
    close(tcpclient.m_clientfd);
    return 0;
}