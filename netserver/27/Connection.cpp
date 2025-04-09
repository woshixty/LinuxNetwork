#include "Connection.h"


Connection::Connection(EventLoop *loop,Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    // 为新客户端连接准备读事件，并添加到epoll中。
    clientchannel_=new Channel(loop_,clientsock_->fd());   
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage,this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));
    clientchannel_->useet();                 // 客户端连上来的fd采用边缘触发。
    clientchannel_->enablereading();   // 让epoll_wait()监视clientchannel的读事件
}

Connection::~Connection()
{
    delete clientsock_;
    delete clientchannel_;
}

int Connection::fd() const                              // 返回客户端的fd。
{
    return clientsock_->fd();
}

std::string Connection::ip() const                   // 返回客户端的ip。
{
    return clientsock_->ip();
}

uint16_t Connection::port() const                  // 返回客户端的port。
{
    return clientsock_->port();
}

void Connection::closecallback()
{
    closecallback_(this);    // 回调TcpServer::closeconnection()。
}

void Connection::errorcallback()
{
    errorcallback_(this);     // 回调TcpServer::errorconnection()。
}

void Connection::writecallback()
{
    int writen=::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
    if(writen>0)
    {
        outputbuffer_.erase(0, writen);
    }
    if(outputbuffer_.size() == 0)
    {
        clientchannel_->disablewriting();
        sendcompletecallback_(this);
    }
}

// 设置关闭fd_的回调函数。
void Connection::setclosecallback(std::function<void(Connection*)> fn)    
{
    closecallback_=fn;     // 回调TcpServer::closeconnection()。
}

// 设置fd_发生了错误的回调函数。
void Connection::seterrorcallback(std::function<void(Connection*)> fn)    
{
    errorcallback_=fn;     // 回调TcpServer::errorconnection()。
}

// 处理对端发送过来的消息。
void Connection::onmessage()
{
    char buffer[1024];
    // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    while (true)
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd(), buffer, sizeof(buffer));
        if (nread > 0)      // 成功的读取到了数据。
        {
            inputbuffer_.append(buffer, nread);
        }
        // 读取数据的时候被信号中断，继续读取。
        else if (nread == -1 && errno == EINTR)
        {
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            while (true)
            {
                int len = 0;

                // 确保 inputbuffer_ 至少有 4 个字节用于读取消息长度
                if (inputbuffer_.size() < 4)
                {
                    break; // 数据不足，等待更多数据
                }

                // 读取消息长度
                memcpy(&len, inputbuffer_.data(), 4);

                // 检查消息长度是否合法
                if (len <= 0 || len > 1024) // 假设消息长度不能超过 1024 字节
                {
                    fprintf(stderr, "Invalid message length: %d\n", len);
                    inputbuffer_.erase(0, 4); // 丢弃长度字段，避免死循环
                    continue;
                }

                // 确保 inputbuffer_ 包含完整的消息数据
                if (inputbuffer_.size() < len + 4)
                {
                    break; // 数据不足，等待更多数据
                }

                // 提取消息内容
                std::string message(inputbuffer_.data() + 4, len);
                inputbuffer_.erase(0, len + 4);

                // 打印接收到的消息
                printf("recv(eventfd=%d):%s\n", fd(), message.c_str());
                onmessagecallback_(this, message);
            }
            break;
        }
        // 客户端连接已断开。
        else if (nread == 0)
        {
            closecallback();
            break;
        }
    }
}

void Connection::setonmessagecallback(std::function<void(Connection*, std::string&)> fn)
{
    onmessagecallback_ = fn;
}

void Connection::send(const char* data, size_t size)
{
    outputbuffer_.appendwithhead(data, size);
    // 注册写事件
    clientchannel_->enablewriting();
}

void Connection::setsendcompletecallback(std::function<void(Connection*)> fn)
{
    sendcompletecallback_ = fn;
}