// 封装socket的服务端
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

class ctcpserver
{
public:
    int     m_listenfd;             // 监听的socket -1表示未初始化
    int     m_clientfd;             // 客户端连接上来的socket -1表示客户端未连接
    string  m_clientip;             // 客户端的IP
    unsigned short m_port;          // 服务端用于通讯的端口

public:
    ctcpserver() : m_listenfd(-1), m_clientfd(-1) {}
    ~ctcpserver() 
    { 
        closelisten(); 
        closeclient();
    }

    bool initserver(unsigned short in_port)
    {
        m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if(m_listenfd==-1)
        {
            perror("socket");
            return false;
        }
        m_port = in_port;
        
        // 将服务端通信的IP和端口绑定值socket
        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family=AF_INET;
        servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
        servaddr.sin_port=htons(m_port);
        // 绑定服务端ip和端口
        if(::bind(m_listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
        {
            perror("bind");
            return false;
        }
        
        if(::listen(m_listenfd, 5) != 0)
        {
            perror("listen");
            return false;
        }
        
        return true;
    }

    bool accept()
    {
        // 客户端的地址信息
        struct sockaddr_in caddr;
        socklen_t addrlen = sizeof(caddr);

        m_clientfd=::accept(m_listenfd, (struct sockaddr*)&caddr, &addrlen);
        if(m_clientfd == -1)
        {
            perror("accept");
            return false;
        }
        m_clientip = inet_ntoa(caddr.sin_addr); // 将客户局端的地址从大端序转成字符串
        return true;
    }

    const std::string& clientip() const
    {
        return m_clientip;
    }

    bool send(std::string& buffer)
    {
        if(m_clientfd == -1)
        {
            return false;
        }
        if(::send(m_clientfd, buffer.c_str(), buffer.size(), 0) <= 0)
        {
            return false;
        }
        return true;
    }

    // 接受服务端回应报文
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

    bool closelisten()
    {
        if(m_listenfd == -1)
        {
            return false;
        }
        ::close(m_listenfd);
        m_listenfd = -1;
        return true;
    }

    bool closeclient()
    {
        if(m_clientfd == -1)
        {
            return false;
        }
        ::close(m_clientfd);
        m_clientfd = -1;
        return true;
    }
};

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        cout << "Using: ./demo2 通讯端口" << endl;
        return -1;
    }
    ctcpserver tcpserver;
    bool bRet = tcpserver.initserver(atoi(argv[1]));
    if(!bRet)
    {
        std::cout << "initserver failed\n";
        return -1;
    }

    bRet = tcpserver.accept();
    if(!bRet)
    {
        std::cout << "accept failed\n";
        return -1;
    }

    cout << "客户端已连接" << endl;

    // Step5 与客户端通信
    string buffer;
    while (true)
    {
        bRet = tcpserver.recv(buffer, 1024);
        if(!bRet)
        {
            cout << "recv failed" << endl;
            break;
        }
        cout << "接收到：" << buffer << endl;
        
        // 回应客户端
        buffer = "ok";
        bRet = tcpserver.send(buffer);
        if(!bRet)
        {
            cout << "send failed" << endl;
            break;
        }
        cout << "发送到：" << buffer << endl;
    }
}