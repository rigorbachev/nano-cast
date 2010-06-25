#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <time.h>

#include "Util.h"
#include "Poll.h"
#include "Dispatcher.h"

Pollable::Pollable(Handle_t h)
: handle(h), readCallBack(NULL), writeCallBack(NULL), next(NULL)
{
    debug("Pollable::Pollable(%d)\n", handle);
    Poll::Register(this);
}

 
Pollable::~Pollable()
{
    debug("Pollable::~Pollable() - handle=%d\n", handle);
    Poll::Deregister(this);
    ::close((int)handle);
}

bool Pollable::WaitForRead(CallBack* c, int timeout)
{
    readCallBack = c;
    if (timeout == -1)   readExpiration = 0;
    else                 readExpiration = Poll::clock + timeout;
    return OK;
}

bool Pollable::WaitForWrite(CallBack* c, int timeout)
{
    writeCallBack = c;
    if (timeout == -1)   writeExpiration = 0;
    else                 writeExpiration = Poll::clock + timeout;
    return OK;
}






List<Pollable> Poll::pollList;
int Poll::pollfd;
time_t Poll::clock;

bool Poll::Init()
{
    // Open the epoll device
    pollfd = ::epoll_create(MAXEVENTS);
    if (pollfd == -1)
    	return SysError("Unable to initialize epoll\n");
    clock = ::time(NULL) - 1;
    return OK;
}


bool Poll::Cleanup()
{
    ::close(pollfd);
    pollList.Delete();
    return OK;
}




bool Poll::Register(Pollable *conn)
{
    debug("Poll::Register - fd=%d\n", conn->handle);
    // Sanity check
    if (!conn->handle == BAD_HANDLE) return !OK;
    if (conn->next != NULL)   return !OK;

    // Add the connection to our list
    pollList.Push(conn);

    // Enable edge triggered reads/writes and errors
    int fd = (int)conn->handle;
    struct epoll_event event;
    event.data.ptr = conn;
    event.events = EPOLLIN | EPOLLOUT | PollErrors | EPOLLET;
    if (::epoll_ctl(pollfd, EPOLL_CTL_ADD, fd, &event) == -1 && errno != EEXIST)
        return !OK;

    return OK;
}


bool Poll::Deregister(Pollable* conn)
{
    debug("Poll::Deregister - fd=%d\n", conn->handle);
    // Sanity Check
    if (conn->handle == BAD_HANDLE)
    	return Error("Poll::Deregister - not connected\n");

    // Disable event polling
    int fd = (int)conn->handle;
    if (::epoll_ctl(pollfd, EPOLL_CTL_DEL, fd, NULL) == -1)
        return SysError("Unable to disable polling fd=%d\n", fd);
 
    // Remove the connection from the list
    pollList.Remove(conn);

    return OK;
}


bool Poll::Reap(int msec)
{
        debug("Poll::Reap(%d)\n", msec);
	// Repeat until we get a partial read from epoll
	int timeout = msec;
	int actual;
	do {

		// Get a batch of events from /dev/epoll
		static PollEvent events[MAXEVENTS];
		actual = ::epoll_wait(pollfd, events, MAXEVENTS, msec);
		if (actual < 0)
			return SysError("Poll::Reap  Unable to get events\n");

		// Only sleep for the first batch
		timeout = 0;

		// For each of the events
		for (int i=0; i<actual; i++) {
			Pollable* p = (Pollable*) events[i].data.ptr;
                        debug("   fd=%d  readable=%d  writable=\n",
                              p->handle, ReadReady(events[i]), 
                              WriteReady(events[i]));
     
			// Schedule the handler if waiting on read
			if (p->readCallBack != NULL && ReadReady(events[i])) {
				Dispatcher::Call(p->readCallBack, OK);
				p->readCallBack = NULL;
			}

			// Schedule the handler if waiting on write
			if (p->writeCallBack != NULL && WriteReady(events[i])) {
				 Dispatcher::Call(p->writeCallBack, OK);
				 p->writeCallBack = NULL;
			 }
		}
	} while (actual == MAXEVENTS);

    return OK;
}



bool Poll::CheckForTimeouts()
{
    debug("Poll::CheckForTimeouts() - clock=%ld\n", clock);
    // Do nothing if the clock hasn't ticked
    long current = ::time(NULL);  // need a wrapper around system call
    if (current == clock) return OK;

    if (current > clock+1)
        debug("Clock advanced by %ld seconds\n", current-clock);

    // Update the clock
    clock = current;

    // For each I/O channel
    for (Pollable* p = pollList.first; p != NULL; p=p->next) {

        // Abort any reader or writer which has timed out
        if (p->readCallBack!=NULL && p->readExpiration<=clock && p->readExpiration!=0) {
            Dispatcher::Call(p->readCallBack, !OK);
            p->readCallBack = NULL;
        }
        if (p->writeCallBack!=NULL && p->writeExpiration<=clock && p->writeExpiration!=0) {
            Dispatcher::Call(p->writeCallBack, !OK);
            p->writeCallBack = NULL;
        }
    }

    return OK;
}

