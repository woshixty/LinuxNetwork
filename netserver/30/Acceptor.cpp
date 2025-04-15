#include "Acceptor.h"

#include "Connection.h"

Acceptor::Acceptor(const std::unique_ptr<EventLoop>& loop,const std::string &ip,const uint16_t port)
    : loop_(loop), 
        servsock_(createnonblocking()), 
        acceptchannel_(loop_,servsock_.fd())
{
    // 服务端的地址和协议。
    InetAddress servaddr(ip,port);
    servsock_.setreuseaddr(true);
    servsock_.settcpnodelay(true);
    servsock_.setreuseport(true);
    servsock_.setkeepalive(true);
    servsock_.bind(servaddr);
    servsock_.listen();
       
    acceptchannel_.setreadcallback(std::bind(&Acceptor::newconnection, this));
    // 让epoll_wait()监视servchannel的读事件。
    acceptchannel_.enablereading();
}

Acceptor::~Acceptor()
{}

// 处理新客户端连接请求。
void Acceptor::newconnection()    
{
    printf("Acceptor::newconnection()\n");
    // 客户端的地址和协议。
    InetAddress clientaddr;
    std::unique_ptr<Socket> clientsock(new Socket(servsock_.accept(clientaddr)));
    clientsock->setipport(clientaddr.ip(), clientaddr.port());
    
    newconnectioncb_(std::move(clientsock));
} 

// 设置处理新客户端连接请求的回调函数。
void Acceptor::setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)> fn)
{
    newconnectioncb_=fn;
}
