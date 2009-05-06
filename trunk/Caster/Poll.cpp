#include "Poll.h"
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


Poll::Poll(int maxfds)
{
    MaxFds = maxfds;

    // Open the epoll device
    pollfd = epoll_create(MaxFds);
}


Poll::~Poll()
{
    close(pollfd);
    MaxFds = 0;
}




bool Poll::Register(int fd)
{
    // Sanity check
    if (fd >= MaxFds || fd < 0) return !OK;

    // Enable edge triggered reads/writes and errors
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLOUT | PollErrors | EPOLLET;
    if (epoll_ctl(pollfd, EPOLL_CTL_ADD, fd, &event) == -1 && errno != EEXIST)
        return !OK;

    return OK;
}


bool Poll::Deregister(int fd)
{
    // Sanity Check
    if (fd >= MaxFds || fd < 0) return !OK;

    // Disable event polling
    if (epoll_ctl(pollfd, EPOLL_CTL_DEL, fd, NULL) == -1)
        return !OK;
 
    return OK;
}


bool Poll::Reap(PollEvent* events, int max, int& actual, int msec)
{
    // Get a batch of events from epoll
    actual = epoll_wait(pollfd, events, max, msec);
    if (actual < 0) 
        return SysError("Poll::Reap  Unable to get events\n");

    debug("Reap:: max=%d  msec=%d  actual=%d\n", max, msec, actual);
    for (int i=0; i<actual; i++)
        debug("   fd=%d  readable=%d  writable=%d\n",
                Fd(events[i]), ReadReady(events[i]), WriteReady(events[i]));
    return OK;

}


