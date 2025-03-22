#include "EventLoop.h"

EventLoop::EventLoop()
{
    ep_ = new Epoll();
}

EventLoop::~EventLoop()
{
    delete ep_;
}

void EventLoop::run()
{
    while(true)
    {
        std::vector<Channel*> channels = ep_->loop();
        for(auto& ch : channels)
        {
            ch->handleevent();
        }
    }
}

Epoll* EventLoop::ep()
{
    return ep_;
}