#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <map>
#include <memory>
#include <mutex>

// TCP网络服务类。
class TcpServer
{
private:
    std::unique_ptr<EventLoop> mainloop_;
    std::vector<std::unique_ptr<EventLoop>> subloops_;
    Acceptor acceptor_;
    int threadnum_;
    ThreadPool threadpool_;
    std::mutex mmutex_;
    std::map<int,spConnection> conns_;
    std::function<void(spConnection)> newconnectioncb_ = nullptr;
    std::function<void(spConnection)> closeconnectioncb_ = nullptr;
    std::function<void(spConnection)> errorconnectioncb_ = nullptr;
    std::function<void(spConnection,const std::string &message)> onmessagecb_ = nullptr;
    std::function<void(spConnection)> sendcompletecb_;
    std::function<void(EventLoop*)>  timeoutcb_ = nullptr;
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

    void removeconnection(int fd);
};