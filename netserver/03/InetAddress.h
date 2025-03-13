#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

/** 
sockaddr 的使用场景：

1、在需要处理多种协议族的情况下，通常使用 sockaddr 作为通用接口。
    例如，在调用 bind、connect、accept 等函数时，这些函数的参数类型为 struct sockaddr*，
    实际传递时需要将具体类型的地址结构（如 sockaddr_in）强制转换为 sockaddr。
sockaddr_in 的使用场景：

2、当明确使用 IPv4 协议时，直接使用 sockaddr_in。
    例如，在初始化服务器或客户端的地址信息时，通常会填充 sockaddr_in 结构体。
*/

// socket的地址协议族
class InetAddress
{
private:
    sockaddr_in addr_;    

public:
    InetAddress(const std::string& ip, uint16_t port); // 如果是监听的fd 用这个构造函数
    InetAddress(const sockaddr_in addr);
    ~InetAddress();

    // 返回字符串表示的地址 例如：192.168.50.106
    const char* ip() const;
    // 返回整数表示的端口 例如 80 8080
    uint16_t port() const;
    // 返回addr_成员的的地址，转换成了sockaddr
    const sockaddr* addr() const;

};