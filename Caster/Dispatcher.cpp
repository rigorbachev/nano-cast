#include "Dispatcher.h"
#include <time.h>


int Dispatcher::MaxFds;
Network* Dispatcher::net;
FragList Dispatcher::dead;
BaseFragment** Dispatcher::Reader;
BaseFragment** Dispatcher::Writer;
int* Dispatcher::ReadExpiration;
int* Dispatcher::WriteExpiration;
int Dispatcher::clock;
bool Dispatcher::done;

//Dispatcher::Dispatcher(int maxfds)
//    : poll(maxfds), MaxFds(maxfds)

bool Dispatcher::Configure(int port, int maxfds)
{
    MaxFds = maxfds;
    net = new Network(MaxFds);
    Reader = new BaseFragment*[MaxFds];
    ReadExpiration = new int[MaxFds];
    Writer = new BaseFragment*[MaxFds];
    WriteExpiration = new int[MaxFds];
    clock = ::time(NULL);
}


//Dispatcher::~Dispatcher()
bool Dispatcher::Cleanup()
{
    // Free up BaseFragments contained in datastructures
    for (int i=0; i<MaxFds; i++)
        if (Reader[i] != NULL) 
             delete Reader[i];
    for (int i=0; i<MaxFds; i++)
        if (Reader[i] != NULL)
             delete Reader[i];

    // Free up the datastructures
    delete Reader;
    delete Writer;
    delete ReadExpiration;
    delete WriteExpiration;

    delete net;
}


bool Dispatcher::Start(BaseFragment *root)
{
    // We are starting anew
    done = false;

    // Start the first fragment
    root->Resume();

    // Repeat until done
    while (!done) {

        // Check for timeouts
        if (CheckForTimeouts() != OK)
            return Error("Dispatcher encountered problems with timeout mechanism\n");

        // Free the dead fragments
        while (dead.Head() != NULL)
            delete dead.Pop();

        // Wait for I/O events
        if (PollForIO() != OK)
            return Error("Dispatcher encountered problems polling\n");

    }
 
    return OK;
}


bool Dispatcher::Stop()
{
    done = true;
    return OK;
}



bool Dispatcher::PollForIO()
{
    // repeat until we get less than a full batch of events
    int timeout = 1000;  // one second timeout first time through
    int actual;
    do {

        // Get a batch of events
        PollEvent event[100];
        if (net->Reap(event, 100, actual, 1000) != OK)
            return Error("Dispatcher was unable to poll for I/O\n");

        // Do for each event in batch
        for (int i=0; i<actual; i++) {
            int fd = Fd(event[i]);
            
            // If read will complete, reset and call the reader
            BaseFragment *r = Reader[fd];
            if (ReadReady(event[i]) && Reader[fd] != NULL) {
                Reader[fd] = NULL;
                r->Resume();
            }                          
                
            // if write will complete, reset and call the writer
            BaseFragment *w = Writer[fd];
            if (WriteReady(event[i]) && w != NULL) {
                Writer[fd] = NULL;
                w->Resume();
            }
        }

        // Next time, don't block when getting a batch events
        timeout = 0;

     } while (actual == 100);
    

    return OK;
}



// Note: Don't like to scan across all outstanding I/Os,
//   but this is only done once per time interval. It could
//   be redone using a linked list of active requests,
//   or even a heap. Leave it for now.

bool Dispatcher::CheckForTimeouts()
{
    // Do nothing if the clock hasn't ticked
    int current = ::time(NULL);  // need a wrapper around system call
    if (current == clock) return OK;

    if (current > clock+1) 
        debug("Clock advanced by %d seconds\n", current-clock);

    // Update the clock
    clock = current;
     
    // For each I/O channel 
    for (int i=0; i<MaxFds; i++) {

        // Abort any reader or writer which has timed out
        if (Reader[i]!=NULL && ReadExpiration[i]<=clock && ReadExpiration[i]!=0) {
            BaseFragment* r = Reader[i];
            Reader[i] = NULL;
            r->Abort("Read timeout\n");
        }
        if (Writer[i]!=NULL && WriteExpiration[i]<=clock && WriteExpiration[i]!=0) {
            BaseFragment* w = Writer[i];
            Writer[i] = NULL;
            w->Abort("Write timeout\n");
        }
    }

    return OK;
}

           


bool Dispatcher::WaitForRead(BaseFragment *f, int fd, int timeout)
{
    debug("WaitForRead: f=%p  fd=%d  timeout=%d\n", f, fd, timeout);
    // Check for errors.
    if (fd < 0 || fd >= MaxFds)  
        return Error("WaitForRead: fd %d out of range (0..%d)\n", fd, MaxFds-1);
    if (Reader[fd] != NULL)
        return Error("WaitForRead: fd %d is already active\n", fd);

    // Add to the tables
    Reader[fd] = f;
    if (timeout == -1) ReadExpiration[fd] = 0;
    else               ReadExpiration[fd] = clock + (timeout+999)/1000;

    return OK;
}

bool Dispatcher::WaitForWrite(BaseFragment *f, int fd, int timeout)
{
    debug("WaitForWrite: f=%p  fd=%d  timeout=%d\n", f, fd, timeout);
    // Check for errors.
    if (fd < 0 || fd >= MaxFds)  
        return Error("WaitForWrite: fd %d out of range [0..%d]\n", fd, MaxFds-1);
    if (Writer[fd] != NULL)
        return Error("WaitForWrite: fd %d is already active\n", fd);

    // Add to the tables
    Writer[fd] = f;
    if (timeout == -1) WriteExpiration[fd] = 0;
    else               WriteExpiration[fd] = clock + (timeout+999)/1000;

    return OK;
}



bool Dispatcher::Abort(BaseFragment *f, const char *msg)
{
    dead.Add(f);

    if (msg != NULL)
        Error("Fragment Aborted: %s\n", msg);

    return !OK;
}


   


