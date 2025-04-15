#include "EventLoop.h"

#include "unistd.h"   // syscall(SYS_gettid)需要包含此头文件。
#include "sys/syscall.h"   // syscall(SYS_gettid)需要包含此头文件。

// 在构造函数中创建Epoll对象ep_。
EventLoop::EventLoop():ep_(new Epoll)
{}

// 在析构函数中销毁ep_。
EventLoop::~EventLoop()
{}

// 运行事件循环。
void EventLoop::run()                      
{
    // 打印线程ID
    // printf("EventLoop thread is %ld.\n",syscall(SYS_gettid));
    while (true)
    {
        // 等待监视的fd有事件发生
        std::vector<Channel *> channels=ep_->loop(100 * 1000);
        if(channels.size() == 0)
        {
            epolltimeoutcallback_(this);
        }
        else
        {
            for (auto &ch:channels)
            {
                // 处理epoll_wait()返回的事件
                ch->handleevent();
            }
        }
    }
}

// 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
void EventLoop::updatechannel(Channel *ch)                        
{
    ep_->updatechannel(ch);
}

void EventLoop::removechannel(Channel *ch)
{
    ep_->removechannel(ch);
}

void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn)
{
    epolltimeoutcallback_ = fn;
}