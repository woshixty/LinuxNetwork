#include "Socket.h"

int creatnonblocking() {
    // 创建服务端用于监听的listenfd
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if(listenfd < 0)
    {
        perror("socket() failed");
        printf("%s:%s:%d listen socket create failed, error: %d", __FILE__, __FUNCTION__, __LINE__, errno);
        exit(-1);
    }
    return listenfd;
}

Socket::Socket(int fd) : fd_(fd) {}

Socket::~Socket() {
    ::close(fd_);
}

int Socket::fd() const {
    return fd_;
}

void Socket::settcpnodelay(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setreuseaddr(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setreuseport(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::setkeepalive(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

void Socket::bind(const InetAddress& servaddr) {
    if (::bind(fd_, servaddr.addr(), sizeof(sockaddr_in)) == -1) {
        perror("bind error");
        close(fd_);
        exit(-1);
    }
}

void Socket::listen(int nn) {
    if(::listen(fd_, nn) < 0)
    {
        perror("listen() failed");
        close(fd_);
        exit(-1);
    }
}

int Socket::accept(InetAddress& clientaddr) {
    struct sockaddr_in peeraddr;
    socklen_t clientlen = sizeof(peeraddr);
    int clientsock = accept4(fd_, (struct sockaddr*)&peeraddr, &clientlen, SOCK_NONBLOCK);
    
    clientaddr.setaddr(peeraddr);
    return clientsock;
}

