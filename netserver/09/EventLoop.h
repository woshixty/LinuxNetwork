#pragma once

#include "Epoll.h"

class EventLoop
{
private:
    Epoll* ep_;
public:
    EventLoop();    // 构造函数中创建epoll对象
    ~EventLoop();   // 析构函数中销毁epoll对象

    void run();     // 事件循环
    Epoll* ep();
};