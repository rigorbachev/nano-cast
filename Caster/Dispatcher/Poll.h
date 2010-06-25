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
#include "List.h"

#include <time.h>
#include <sys/epoll.h>
#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0
#endif 

#include "CallBack.h"

typedef int Handle_t;
static const Handle_t BAD_HANDLE = -1;

class Pollable {
public:
	Pollable*    next;
	const Handle_t     handle;
	CallBack     *readCallBack;
	CallBack     *writeCallBack;
	time_t         readExpiration;
	time_t         writeExpiration;

public:
	Pollable(Handle_t h);
        virtual ~Pollable();

        bool WaitForRead(CallBack* c, int timeout);
        bool WaitForWrite(CallBack* c, int timeout);
};


typedef struct epoll_event PollEvent;
static const int PollErrors 
    = EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP;
inline bool ReadReady(PollEvent& ev) 
    {return (ev.events & (EPOLLIN|PollErrors)) != 0;}
inline bool WriteReady(PollEvent& ev) 
    {return (ev.events & (EPOLLOUT|PollErrors)) != 0;}
inline int Handle(PollEvent& ev)
    {return ((Pollable*)ev.data.ptr)->handle;}



class Poll {
public:
    static bool Init();
    static bool Cleanup();

    static bool Register(Pollable *connection);
    static bool Deregister(Pollable *connection);
    static bool Reap(int msec=-1);
    static bool CheckForTimeouts();
   
    static List<Pollable> pollList;
    static int pollfd;
    static const int MAXEVENTS = 128;

    static time_t clock;
};

#endif


