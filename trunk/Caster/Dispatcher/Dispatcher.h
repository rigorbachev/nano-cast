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
#include "List.h"
#include "Util.h"
class CallBack;

class Dispatcher {
public:
    static Status Init();
    static Status Cleanup();
    
    static Status Run();
    static Status Stop();

    static void Call(CallBack *h, Status status);

    // Protected
    //   Note: we use a FifoList to prevent starvation
    static FifoList<CallBack>   runList;
    static bool     done;          // flag telling us when to stop
};
    
#endif // DispatcherIncluded

