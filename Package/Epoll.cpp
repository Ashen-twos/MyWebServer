#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 2048

Epoll::Epoll(): epfd(-1),events(nullptr)
{
    epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(*events) * MAX_EVENTS);
}
Epoll::~Epoll()
{
    if(epfd != -1)
    {
        close(epfd);
        epfd = -1;
    }
    delete [] events;
}

int Epoll::getepfd()
{
    return epfd;
}

void Epoll::addFd(int fd, uint32_t op)
{
    struct epoll_event ev;
    bzero(&ev,sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
}

void Epoll::delFd(int fd)
{
    if(fd<0)
        return;
    epoll_event evs = {0};
    errif(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &evs) == -1, "epoll add error");
}

bool Epoll::delfd(int fds, int epfds)
{
    if(fds<0)
        return false;
    epoll_event evs = {0};
    errif(epoll_ctl(epfds, EPOLL_CTL_DEL, fds, &evs) == -1, "epoll del error");
    return true;
}

bool Epoll::modfd(int fd, int epfd, uint32_t events)
{
    if(fd < 0) 
        return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);

}

std::vector<epoll_event> Epoll::poll(int timeout)
{
    std::vector<epoll_event> act_events;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for(int i=0; i<nfds; ++i)
        act_events.push_back(events[i]);
    return act_events;
}
