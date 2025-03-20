#pragma once

#include <sys/epoll.h>
#include "Epoll.h"

class Epoll;

class Channel {
private:
    int fd_ = -1;           // Channel拥有的fd_ 是 1:1 的关系
    Epoll* ep_ = nullptr;   // Channel对应的红黑树 Channel与Epoll是多对一的关系 一个Channel对应一个Epoll
    bool inepoll_ = false;  // Channel是否已添加到 epoll 树上 如果未添加 调用epoll_ctl()时调用EPOLL_CTL_ADD 否则调用EPOLL_CTL_MOD
    uint32_t events_ = 0;   // fd_需要监视的事件 listenfd_需要监视EPOLLIN clientfd_需要监视EPOLLOUT
    uint32_t revents_ = 0;  // fd_实际发生的事件
public:
    Channel(Epoll* ep, int fd);
    ~Channel();

    int fd();                       // 获取fd_
    void useet();                   // 采用边缘触发
    void enablereading();           // 让epoll_wait()监视fd_的EPOLLIN事件
    void setinepoll();              // 将inepoll_置为true
    void setrevents(uint32_t ev);   // 设置revents_成员的值为参数ev
    bool inpoll();                  // 判断inepoll_是否为true
    uint32_t events();              // 获取events_
    uint32_t revents();             // 获取revents_
};