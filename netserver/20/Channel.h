#pragma once
#include <sys/epoll.h>
#include <functional>
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
 
class EventLoop;

class Channel
{
private:
    int fd_=-1;                 // Channel拥有的fd，Channel和fd是一对一的关系。
    EventLoop *loop_=nullptr;   // Channel对应的事件循环，Channel与EventLoop是多对一的关系，一个Channel只对应一个EventLoop。
    bool inepoll_=false;        // Channel是否已添加到epoll树上，如果未添加，调用epoll_ctl()的时候用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD。
    uint32_t events_=0;         // fd_需要监视的事件。listenfd和clientfd需要监视EPOLLIN，clientfd还可能需要监视EPOLLOUT。
    uint32_t revents_=0;        // fd_已发生的事件。 
    std::function<void()> readcallback_;         // fd_读事件的回调函数，如果是acceptchannel，将回调Acceptor::newconnection()，如果是clientchannel，将回调Channel::onmessage()。
    std::function<void()> closecallback_;        // 关闭fd_的回调函数，将回调Connection::closecallback()。
    std::function<void()> errorcallback_;        // fd_发生了错误的回调函数，将回调Connection::errorcallback()。

public:
    Channel(EventLoop* loop,int fd);
    ~Channel();

    int fd();
    void useet();
    void enablereading();
    void setinepoll();
    void setrevents(uint32_t ev);
    bool inpoll();
    uint32_t events();
    uint32_t revents();

    void handleevent();

    void setreadcallback(std::function<void()> fn);    // 设置fd_读事件的回调函数。
    void setclosecallback(std::function<void()> fn);   // 设置关闭fd_的回调函数。
    void seterrorcallback(std::function<void()> fn);   // 设置fd_发生了错误的回调函数。
};