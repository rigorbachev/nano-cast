#ifndef DispatcherIncluded
#define DispatcherIncluded

/////////////////////////////////////////////////////////////////////////////////////
//
// A Dispatcher manages network and timer events.
//   It invokes a code "fragment" when an event occurs
//   and cleans up fragements which have exited.
//
// By being event driven and using finite-state "fragments" of code,
//   the Dispatcher avoids having to create a thread for each connection.
//   The total memory requirements are minimal, and there is no need
//   to deal with contention.
//
////////////////////////////////////////////////////////////////////////////////////////////////

// Fragment and Dispatcher must go together to support 
//    inlining of some of the Fragment functions.
#include "BaseFragment.h"
#include "Network.h"
#include "FragList.h"


class Dispatcher {
public:


    // These are the functions a fragment calls to wait on an event or timeout trying
    static bool WaitForRead(BaseFragment *f, int fd, int timeout);
    static bool WaitForWrite(BaseFragment *f, int fd, int timeout);
    static bool Abort(BaseFragment *f, const char *msg = NULL);

    static bool Start(BaseFragment *f);
    static bool Stop();

    // These are for setting up the dispatcher itself
    Dispatcher(int MaxFds = 0);
    ~Dispatcher();
    static bool Configure(int MaxFds = 0);
    static bool Cleanup();

    // Wrappers around net. 
    // These are a consequence of using fd's instead of a socket class.
    // We need access to the "net" class in order to do network I/O,s 
    //   but we don't want to make it obvious to our callers.
    //   These wrappers hide our design mistake.
    inline static bool Read(int fd, byte* buf, int size, int& actual) 
        {return net->Read(fd, buf, size, actual);}
    inline static bool Write(int fd, const byte* buf, int size, int& actual) 
        {return net->Write(fd, buf, size, actual);}
    inline static bool Connect(NetAddress& remote, int& newfd)
        {return net->Connect(remote, newfd);}
    inline static bool Listen(NetAddress& local, int& newfd)
        {return net->Listen(local, newfd);}
    inline static bool Accept(int fd, int& newfd)
        {return net->Accept(fd, newfd);}

    

protected:
    static int MaxFds;             // the maximum number of file descriptors
    static Network* net;              // Network stuff
    static FragList dead;          // A list of dead fragments which need to be cleaned up
    static BaseFragment **Reader;       // Fragment waiting to read
    static int      *ReadExpiration;   // When the read expires
    static BaseFragment **Writer;       // Fragment waiting to write
    static int      *WriteExpiration;  // When the write expires
    static int      clock;         // current clock time, or close enough
    static bool     done;          // flag telling us when to stop

private:
    static bool PollForIO();
    static bool CheckForTimeouts();
};
    
#endif // DispatcherIncluded

