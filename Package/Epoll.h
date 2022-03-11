#ifndef EPOLL_H_INCLUDED
#define EPOLL_H_INCLUDED

#include <sys/epoll.h>
#include <vector>

class Epoll{
private:
    int epfd;
    struct epoll_event *events;

public:
    Epoll();
    ~Epoll();

    void addFd(int, uint32_t);
    void delFd(int);
    static bool modfd(int fd, int epfd, uint32_t events);
    static bool delfd(int fds, int epfds);
    int getepfd();
    std::vector<epoll_event> poll(int timeout = -1);
};



#endif // EPOLL_H_INCLUDED
