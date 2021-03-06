#include "CallBack.h"
#include "Dispatcher.h"
#include "Poll.h"

/******
bool CallBack::Switch(CallBack* c, bool status) 
{
    Dispatcher::Call(c, status); 
    return !OK;
}

CallBack* CallBack::OnExit(CallBack* cb)
{
    exitCallBack = cb;
}

bool CallBack::Exit(bool status)
{
     Dispatcher::Call(exitCallBack, status);
     return !OK;
}


bool CallBack::Yield()
{
    Dispatcher::Call(this, OK);
    return OK;
}


********/

bool CallBack::WaitForRead(Pollable *c, int timeout)
{
    return c->WaitForRead(this, timeout);
}


bool CallBack::WaitForWrite(Pollable *c, int timeout)
{
    return c->WaitForWrite(this, timeout);
}

bool WaitList::Wait(CallBack *c)
{
    Push(c);
    return OK;
}

bool WaitList::WakeupWaiters(bool status)
{
    while (Head() != NULL)
        Dispatcher::Call(Pop(), status);
    return OK;
}

