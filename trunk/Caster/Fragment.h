#ifndef FragmentIncluded
#define FragmentIncluded
////////////////////////////////////////////////////////////////////
//
// Dispatchable is a Fragment which interacts with the Dispatcher
//   In other words, they know how to do polling I/O
//   Most applications will use base their Fragments on Dispatchable
//   There may be some name changes
//       Fragment -->  BaseFragment 
//       Dispatchable --> Fragment
///////////////////////////////////////////////////////////////////

#include "Dispatcher.h"
#include "BaseFragment.h"


class Fragment : public BaseFragment {

public:
    Fragment() {}
    virtual ~Fragment(){}

    inline bool Switch(Fragment *frag)
        {Abort(); return frag->Resume();}

    virtual bool Abort(const char *msg = NULL)
        {return Dispatcher::Abort(this, msg);}
    inline bool Read(int fd, byte* buf, int size, int& actual) 
        {return Dispatcher::Read(fd, buf, size, actual);}
    inline bool Write(int fd, const byte* buf, int size, int& actual) 
        {return Dispatcher::Write(fd, buf, size, actual);}
    inline bool WaitForRead(int fd, int timeout) 
        {return Dispatcher::WaitForRead(this, fd, timeout);}
    inline bool WaitForWrite(int fd, int timeout) 
        {return Dispatcher::WaitForWrite(this, fd, timeout);}
    inline bool Accept(int fd, int& newfd)
        {return Dispatcher::Accept(fd, newfd);}
    inline bool Connect(NetAddress& addr, int& newfd)
        {return Dispatcher::Connect(addr, newfd);}
    inline bool Listen(NetAddress& addr, int& newfd)
        {return Dispatcher::Listen(addr, newfd);}

};


#endif // FragmentIncluded
