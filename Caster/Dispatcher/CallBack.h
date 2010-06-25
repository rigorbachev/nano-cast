#ifndef HandlerIncluded
#define HandlerIncluded

#include "Util.h"  // for NULL
#include "List.h"

class Pollable;

// A CallBack is invoked to process an event
//   - Call() is called when an event occurs
//       (automatically deleted if Call() returns Error)
//   - Puts itself on various wait queues

class CallBack {
public:
    CallBack* next;
    bool status;

public:
    virtual ~CallBack(){}
    virtual bool Call(bool status = OK)
        {return Error("Handler base class resumed\n");}

    // Some wrappers
    bool Switch(CallBack* c, bool status = OK) ;
    bool WaitForRead(Pollable *c, int timeout = -1);
    bool WaitForWrite(Pollable *c, int timeout = -1);
};


class WaitList : public List<CallBack>
{
public:
    bool Wait(CallBack *c);
    bool WakeupWaiters(bool status=OK);
};


#endif // BaseFragmentIncluded

