#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <map>
#include <memory>

// TCP网络服务类。
class TcpServer
{
private:
    std::unique_ptr<EventLoop> mainloop_;                                 // 主事件循环。 祼指针 普通指针 原始指针 std::unique_ptr
    std::vector<std::unique_ptr<EventLoop>> subloops_;            // 存放从事件循环的容器。
    Acceptor acceptor_;                                         // 一个TcpServer只有一个Acceptor对象。
    int threadnum_;                                               // 线程池的大小，即从事件循环的个数。
    ThreadPool threadpool_;                                 // 线程池。
    std::map<int,spConnection>  conns_;            // 一个TcpServer有多个Connection对象，存放在map容器中。
    std::function<void(spConnection)> newconnectioncb_;          // 回调EchoServer::HandleNewConnection()。
    std::function<void(spConnection)> closeconnectioncb_;        // 回调EchoServer::HandleClose()。
    std::function<void(spConnection)> errorconnectioncb_;         // 回调EchoServer::HandleError()。
    std::function<void(spConnection,const std::string &message)> onmessagecb_;        // 回调EchoServer::HandleMessage()。
    std::function<void(spConnection)> sendcompletecb_;            // 回调EchoServer::HandleSendComplete()。
    std::function<void(EventLoop*)>  timeoutcb_;                       // 回调EchoServer::HandleTimeOut()。
public:
    TcpServer(const std::string &ip,const uint16_t port,int threadnum=3);
    ~TcpServer();

    void start();          // 运行事件循环。 

    void newconnection(std::unique_ptr<Socket> clientsock);    // 处理新客户端连接请求，在Acceptor类中回调此函数。
    void closeconnection(spConnection conn);  // 关闭客户端的连接，在Connection类中回调此函数。 
    void errorconnection(spConnection conn);  // 客户端的连接错误，在Connection类中回调此函数。
    void onmessage(spConnection conn,const std::string& message);     // 处理客户端的请求报文，在Connection类中回调此函数。
    void sendcomplete(spConnection conn);     // 数据发送完成后，在Connection类中回调此函数。
    void epolltimeout(EventLoop *loop);         // epoll_wait()超时，在EventLoop类中回调此函数。

    void setnewconnectioncb(std::function<void(spConnection)> fn);
    void setcloseconnectioncb(std::function<void(spConnection)> fn);
    void seterrorconnectioncb(std::function<void(spConnection)> fn);
    void setonmessagecb(std::function<void(spConnection,const std::string &message)> fn);
    void setsendcompletecb(std::function<void(spConnection)> fn);
    void settimeoutcb(std::function<void(EventLoop*)> fn);
};