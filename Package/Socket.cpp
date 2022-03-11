#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>


Socket::Socket():fd(-1)
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "sock create error");
    int opt =1;
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
}
Socket::Socket(int _fd):fd(_fd)
{
    errif(fd == -1, "sock create error");
    int opt =1;
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
}
Socket::~Socket()
{
    if(fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress* addr)
{
    errif(::bind(fd, (sockaddr*)&addr->addr, addr->addr_len)==-1, "socket bind error");
}

void Socket::listen()              
{
    errif(::listen(fd,SOMAXCONN)==-1, "socket listen error");
}

void Socket::setnonblocking()
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void Socket::setnonblocking2(int fds)
{
    int old_option = fcntl(fds, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fds, F_SETFL, new_option);
}

int Socket::accept(InetAddress* addr)   
{
    int client = ::accept(fd,(sockaddr*)&(addr->addr),&(addr->addr_len));
    errif(client ==-1, "socket accept error");
    return client;
}

int Socket::getFd()
{
    return fd;
}

void Socket::setlinger()
{
    struct linger tmp = {1,1};
    setsockopt(fd,SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
}
