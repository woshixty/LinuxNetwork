#pragma once

#include <sys/epoll.h>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <iostream>
#include <cassert>
#include <cstring>
#include <unistd.h> 
      
class Epoll
{
private:
    const static int MaxEvents = 100;   // epoll_wait() 返回事件数组的大小
    int epollfd_ = -1;                  // epoll文件描述符
    epoll_event events_[MaxEvents];     // epoll_wait() 返回的事件数组

public:
    Epoll();
    ~Epoll();

    void addfd(int fd, uint32_t op);    // 将fd和需要监视的事件添加到红黑树上
    std::vector<epoll_event> loop(int timeout = -1);    // 运行epoll_wait()，等待事件的发生
};