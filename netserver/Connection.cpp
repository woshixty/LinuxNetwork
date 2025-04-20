#include "Connection.h"

Connection::Connection(EventLoop* loop,std::unique_ptr<Socket> clientsock)
    : loop_(loop),
        clientsock_(std::move(clientsock)),
        disconnect_(false),
        clientchannel_(new Channel(loop_,clientsock_->fd())) 
{
    // 为新客户端连接准备读事件，并添加到epoll中。
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage,this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));
    clientchannel_->useet();                 // 客户端连上来的fd采用边缘触发。
    clientchannel_->enablereading();   // 让epoll_wait()监视clientchannel的读事件
}

Connection::~Connection()
{
    printf("[%s], 已经析构\n", __FUNCTION__);
}

// 返回客户端的fd。
int Connection::fd() const
{
    return clientsock_->fd();
}

// 返回客户端的ip。
std::string Connection::ip() const
{
    return clientsock_->ip();
}

// 返回客户端的port。
uint16_t Connection::port() const
{
    return clientsock_->port();
}

// TCP连接关闭（断开）的回调函数，供Channel回调。
void Connection::closecallback()
{
    disconnect_=true;
    clientchannel_->remove();
    closecallback_(shared_from_this());
}

void Connection::errorcallback()
{
    disconnect_=true;
    // 从事件循环中删除Channel。
    clientchannel_->remove();
    // 回调TcpServer::errorconnection()。
    errorcallback_(shared_from_this());
}

// 设置关闭fd_的回调函数。
void Connection::setclosecallback(std::function<void(spConnection)> fn)    
{
    // 回调TcpServer::closeconnection()。
    closecallback_=fn;
}

// 设置fd_发生了错误的回调函数。
void Connection::seterrorcallback(std::function<void(spConnection)> fn)    
{
    // 回调TcpServer::errorconnection()。
    errorcallback_=fn;
}

// 设置处理报文的回调函数。
void Connection::setonmessagecallback(std::function<void(spConnection,const std::string&)> fn)    
{
    // 回调TcpServer::onmessage()。
    onmessagecallback_=fn;
}

// 发送数据完成后的回调函数。
void Connection::setsendcompletecallback(std::function<void(spConnection)> fn)    
{
    sendcompletecallback_=fn;
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
        // 成功的读取到了数据。
        if (nread > 0)
        {
            // 把读取的数据追加到接收缓冲区中。
            inputbuffer_.append(buffer,nread);
        } 
        // 读取数据的时候被信号中断，继续读取。
        else if (nread == -1 && errno == EINTR)
        {  
            continue;
        } 
        // 全部的数据已读取完毕。
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            // 从接收缓冲区中拆分出客户端的请求消息。
            while (true)
            {
                //////////////////////////////////////////////////////////////
                // 可以把以下代码封装在Buffer类中，还可以支持固定长度、指定报文长度和分隔符等多种格式。
                int len;
                memcpy(&len,inputbuffer_.data(),4);     // 从inputbuffer中获取报文头部。
                // 如果inputbuffer中的数据量小于报文头部，说明inputbuffer中的报文内容不完整。
                if (inputbuffer_.size()<len+4) break;

                std::string message(inputbuffer_.data()+4,len);   // 从inputbuffer中获取一个报文。
                inputbuffer_.erase(0,len+4);                                 // 从inputbuffer中删除刚才已获取的报文。
                //////////////////////////////////////////////////////////////

                printf("message (fd=%d):%s\n",fd(),message.c_str());
                lastatime_ = Timestamp::now();
                printf("lastatime_ is %s\n", lastatime_.toString().c_str());

                // 回调TcpServer::onmessage()处理客户端的请求消息。
                onmessagecallback_(shared_from_this(),message);
            }
            break;
        } 
        else if (nread == 0)  // 客户端连接已断开。
        {  
            // 回调TcpServer::closecallback()。
            closecallback();
            break;
        }
    }
}

// 发送数据，不管在任何线程中，都是调用此函数发送数据。
void Connection::send(const char *data, size_t size)
{
    // 打印发送的数据
    printf("send() data is %.*s\n", static_cast<int>(size), data);

    if (disconnect_ == true) {
        printf("客户端连接已断开了，send()直接返回。\n");
        return;
    }
    std::string copied_data(data, size);
    if (loop_->isinloopthread()) {
        // 如果当前线程是IO线程，直接调用sendinloop()发送数据。
        printf("send() 在事件循环的线程中。\n");
        sendinloop(copied_data);
    } else {
        // 如果当前线程不是IO线程，深拷贝数据并传递给事件循环线程。
        printf("send() 不在事件循环的线程中。\n");
        loop_->queueinloop([this, copied_data]() {
            sendinloop(copied_data);
        });
    }
}

// 发送数据，如果当前线程是IO线程，直接调用此函数，如果是工作线程，将把此函数传给IO线程去执行。
void Connection::sendinloop(const std::string& data)
{
    // 打印发送的数据
    printf("sendinloop() data is %.*s\n", static_cast<int>(data.size()), data.data());

    outputbuffer_.appendwithhead(data.data(), data.size());
    clientchannel_->enablewriting();
}

// 处理写事件的回调函数，供Channel回调。
void Connection::writecallback()                   
{
    int writen=::send(fd(),outputbuffer_.data(),outputbuffer_.size(),0);    // 尝试把outputbuffer_中的数据全部发送出去。
    if (writen>0) outputbuffer_.erase(0,writen);                                        // 从outputbuffer_中删除已成功发送的字节数。

    // 如果发送缓冲区中没有数据了，表示数据已发送完成，不再关注写事件。
    if (outputbuffer_.size()==0) 
    {
        clientchannel_->disablewriting();        
        sendcompletecallback_(shared_from_this());
    }
}

bool Connection::timeout(time_t now, int val)
{
    printf("[%s], now %ld\n", __FUNCTION__, now);
    printf("[%s], lastatime_ %ld\n", __FUNCTION__, lastatime_.toInt());
    return now - lastatime_.toInt() > val;
}