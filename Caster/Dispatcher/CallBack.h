#ifndef HandlerIncluded
#define HandlerIncluded
/***   dummy autodoc comment */

#include "Util.h"  // for NULL
#include "List.h"

class Pollable;

/** A CallBack is an object invoked to process an event

    A CallBack can be placed in a queue to be processed when an
    event of interest happens, say when data is available
    on a socket or a new connection arrives. Also, one
    CallBack can schedule another, allowing callbacks to
    be daisy-chained together to form more complex functions.

    This is a base class which should be used for 
    all callback functions. Each derived class provides a 
    Call() method, a destructor and data fields.
*/

class CallBack {
public:
    CallBack* next;
    bool status;

public:
    virtual ~CallBack(){}

    /** Invoke the callback.
        @param status indicates a possible error for the callback to handle
        @returns !OK if the callback is complete and should be destroyed
    */     
    virtual bool Call(bool status = OK)
        {return Error("Handler base class resumed\n");}

    /// Destroy the current callback and invoke the next
    bool Switch(CallBack* c, bool status = OK) ;

    /// Schedule the current callback when device is readable
    bool WaitForRead(Pollable *p, int timeout = -1);

    /// Schedule the current callback when device is writable
    bool WaitForWrite(Pollable *p, int timeout = -1);

    /// Reschedule the current callback
    bool Yield();
};

/// A WaitList is a list of CallBacks which are waiting for something
class WaitList : public List<CallBack>
{
public:
    /// Add a CallBack to the waitlist
    bool Wait(CallBack *c);

    /// Invoke all the CallBacks on the waitlist
    bool WakeupWaiters(bool status=OK);
};


#endif // BaseFragmentIncluded

