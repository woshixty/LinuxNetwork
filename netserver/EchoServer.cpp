#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip,const uint16_t port,int subthreadnum,int workthreadnum)
                   :tcpserver_(ip,port,subthreadnum),threadpool_(workthreadnum,"WORKS")
{
    // 以下代码不是必须的，业务关心什么事件，就指定相应的回调函数。
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    // tcpserver_.settimeoutcb(std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{

}

// 启动服务。
void EchoServer::Start()                
{
    tcpserver_.start();
}

// 处理新客户端连接请求，在TcpServer类中回调此函数。
void EchoServer::HandleNewConnection(spConnection conn)    
{
    std::cout << "New Connection Come in." << std::endl;
    // printf("EchoServer::HandleNewConnection() thread is %d.\n",syscall(SYS_gettid));

    // 根据业务的需求，在这里可以增加其它的代码。
}

// 关闭客户端的连接，在TcpServer类中回调此函数。 
void EchoServer::HandleClose(spConnection conn)  
{
    std::cout << "EchoServer conn closed." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}

// 客户端的连接错误，在TcpServer类中回调此函数。
void EchoServer::HandleError(spConnection conn)  
{
    std::cout << "EchoServer conn error." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}

// 处理客户端的请求报文，在TcpServer类中回调此函数。
void EchoServer::HandleMessage(spConnection conn,const std::string& message)     
{
    // printf("EchoServer::HandleMessage() thread is %d.\n",syscall(SYS_gettid)); 

    if (threadpool_.size()==0)
    {
        // 如果没有工作线程，表示在IO线程中计算。
        OnMessage(conn,message);       
    }
    else
    {
        // 把业务添加到线程池的任务队列中，交给工作线程去处理业务。
        threadpool_.addtask(std::bind(&EchoServer::OnMessage,this,conn,message));
    }
}

 // 处理客户端的请求报文，用于添加给线程池。
 void EchoServer::OnMessage(spConnection conn,const std::string& message)     
 {
    // 在这里，将经过若干步骤的运算。
    std::string messageT="reply:" + message;          // 回显业务。
    printf("EchoServer::OnMessage() message is %s.\n",message.c_str());
    printf("EchoServer::OnMessage() messageT is %s.\n",messageT.c_str());
    conn->send(messageT.data(),messageT.size());   // 把数据发送出去。 
 }

// 数据发送完成后，在TcpServer类中回调此函数。
void EchoServer::HandleSendComplete(spConnection conn)     
{
    std::cout << "Message send complete." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}

/*
// epoll_wait()超时，在TcpServer类中回调此函数。
void EchoServer::HandleTimeOut(EventLoop *loop)         
{
    std::cout << "EchoServer timeout." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}
*/