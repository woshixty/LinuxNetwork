#pragma once

#include "Epoll.h"
#include "Connection.h"

#include <functional>
#include <memory>
#include <unistd.h>
#include <queue>
#include <mutex>
#include <map>
#include <sys/eventfd.h>
#include <sys/syscall.h>

class Channel;
class Epoll;
class Connection;

using spConnection=std::shared_ptr<Connection>;

// 事件循环类。
class EventLoop
{
private:
    std::unique_ptr<Epoll> ep_;                                   // 每个事件循环只有一个Epoll。
    std::function<void(EventLoop*)> epolltimeoutcallback_;         // epoll_wait()超时的回调函数。
    pid_t threadid_;                                                       // 事件循环所在线程的id。
    std::queue<std::function<void()>> taskqueue_;    // 事件循环线程被eventfd唤醒后执行的任务队列。
    std::mutex mutex_;                                                  // 任务队列同步的互斥锁。
    int wakeupfd_;                                                         // 用于唤醒事件循环线程的eventfd。
    std::unique_ptr<Channel> wakechannel_;              // eventfd的Channel。

    int timerfd_;
    std::unique_ptr<Channel> timerchannel_;

    bool mainloop_;
    std::mutex mmutex_;

    std::map<int, spConnection> conns_;

    std::function<void(int)> timercallback_;
    int timetvl_;
    int timeout_;

    // 1、在事件循环中增加 map<int spConnection> 存放Connection对象
    // 2、闹钟时间到了 便利conns 从判断是否超时
    // 3、如果超时 删除conns里面的Connection对象
    // 4、从TcpServer.conns里面删除Connection对象

public:
    EventLoop(bool mainloop, int timetvl=30, int timeout=20);                   // 在构造函数中创建Epoll对象ep_。
    ~EventLoop();                // 在析构函数中销毁ep_。

    void run();                      // 运行事件循环。

    void updatechannel(Channel *ch);                        // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
    void removechannel(Channel *ch);                       // 从黑树上删除channel。
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);  // 设置epoll_wait()超时的回调函数。

    bool isinloopthread();   // 判断当前线程是否为事件循环线程。

    void queueinloop(std::function<void()> fn);          // 把任务添加到队列中。
    void wakeup();                                                        // 用eventfd唤醒事件循环线程。
    void handlewakeup();                                             // 事件循环线程被eventfd唤醒后执行的函数。
    
    void handletimer();

    void newconnections(spConnection conn);

    void settimercallback(std::function<void(int)> fn);
};