#include <time.h>
#include "Dispatcher.h"
#include "Poll.h"



FifoList<CallBack>   Dispatcher::runList;
bool     Dispatcher::done;          // flag telling us when to stop



bool Dispatcher::Init() {
    return Poll::Init();
}

bool Dispatcher::Cleanup() {
    Poll::Cleanup();
    runList.Delete();
    return OK;
}




bool Dispatcher::Run()
{
    // We are starting anew
    done = false;

    // Repeat until done
    while (!done) {

    	// Invoke the runnable handlers  (careful, could loop!)
    	while (runList.Head() != NULL) {
    		CallBack *h = runList.Pop();
                debug("Dispatcher::Run() - calling %p\n", h);
    		if (h->Call(h->status) != OK)
    			delete h;
    	}

        // Wait for I/O events
        if (Poll::Reap(1000) != OK)
            return Error("Dispatcher encountered problems polling\n");

        // Check for timeouts
        if (Poll::CheckForTimeouts() != OK)
            return Error("Dispatcher encountered problems with timeout mechanism\n");
    }
 
    return OK;
}




bool Dispatcher::Stop()
{
    done = true;
    return OK;
}


void Dispatcher::Call(CallBack *h, bool status)
{
    debug("Dispatcher::Call(%p, %d)\n", h, status);
	h->status = status;
	runList.Add(h);
}


