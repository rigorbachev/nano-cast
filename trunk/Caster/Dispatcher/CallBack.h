#ifndef HandlerIncluded
#define HandlerIncluded
/***   dummy autodoc comment */

#include "Util.h"  // for NULL
#include "List.h"


class Pollable;
class CallBack;

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

typedef Status (CallBack::*CallBackMethod)(Status status);

class CallBack {
public:
    CallBack* next;
    bool status;

    CallBackMethod Call;

public:
    virtual ~CallBack(){}

    /** Invoke the callback.
        @param status indicates a possible error for the callback to handle
        @returns !OK if the callback is complete and should be destroyed
    */     
    //virtual bool DefaultCall(bool status = OK)
        //{return Error("CallBack base class shouldn't be called\n");}

    /// Destroy the current callback and invoke the next
    //CallBack* OnExit(CallBack* c);
    //bool Exit(bool status = OK);

    //bool Switch(CallBack* c, bool status = OK) ;

    /// Schedule the current callback when data is available
    bool WaitForRead(Pollable *p, int timeout = -1);
    bool WaitForWrite(Pollable *p, int timeout = -1);

    /// Reschedule the current callback
    //bool Yield();
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

