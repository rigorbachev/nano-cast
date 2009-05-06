#ifndef PollIncluded
#define PollIncluded
///////////////////////////////////////////////////////////////////////////////////
// Poll is a wrapper class for implementing fast poll.
//   It abstracts basic poll functionality which could be implemented
//   on a variety of operating systems.
//
// Each fd must be "registered".
//
// Read() and Write() are non-blocking, and will return 0 if no data can 
//     be transferred.
//
// Reap() returns a list of events which have occurred since the last Reap().
//     event.ReadReady() and event.WriteReady() indicate it's time to try
//     another transfer. 
//
// Note that events are created only if a partial buffer was
//     transferred by a Read() or Write() call. If a complete transfer took place,
//     then no events will be created until the application issues an additional 
//     request.
//
//////////////////////////////////////////////////////////////////////////////
#include "Util.h"

#include <sys/epoll.h>
#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0
#endif 

typedef struct epoll_event PollEvent;
static const int PollErrors 
    = EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP;
inline bool ReadReady(PollEvent& ev) 
    {return (ev.events & (EPOLLIN|PollErrors)) != 0;}
inline int Fd(PollEvent& ev) {return ev.data.fd;}
inline bool WriteReady(PollEvent& ev) 
    {return (ev.events & (EPOLLOUT|PollErrors)) != 0;}



class Poll {
public:
    Poll(int maxfds);
    ~Poll();

    bool Register(int fd);
    bool Deregister(int fd);
    bool Reap(PollEvent*, int max, int& actual, int msec=-1);
   
protected:
    int MaxFds;
    int pollfd;
};

#endif


