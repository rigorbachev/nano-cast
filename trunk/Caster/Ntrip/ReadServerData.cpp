#include "ReadServerData.h"
#include <algorithm>
#include <unistd.h>



ReadServerData::ReadServerData(Connection_ptr& c, MountPoint* m, int to)
    : conn(c), mnt(m), timeout(to), buf(mnt->Buf)
{
    debug("ReadServerData()  fd=%d  mnt=%s\n", conn->handle, mnt->Name);
}



ReadServerData::~ReadServerData()
{
    debug("~ReadServerData()  fd=%d  mnt=%s\n", conn->handle, mnt->Name);
    WakeupWaiters(!OK);
    mnt->Unmount();
}




bool ReadServerData::Call(bool status)
{
    debug("ReadServerData::Call(%d)\n", status);
    // If timed out, then done.
    if (status != OK)
    	return Error("Timed out waiting for server data\n");

    // Keep reading while there is more data
    ssize_t actual, size;

    // Read data, but not so much the consumers get left behind
    int start = buf.count % buf.BufSize;
    size = min(TooFarBehind/2, buf.BufSize-start);
    if (conn->Read(buf.Buffer+start, size, actual) !=  OK)
        return Error("Problems reading input data stream\n");
    debug("ReadServerData: start=%zd size=%zd actual=%zd buf.count=%lld\n",
                           start,    size,    actual,    buf.count);
    if (actual == -1)
        return Error("Server has closed connection\n");

        // Update the number of bytes transferred
    buf.count += actual;

    // if we actually transferred data, wake up the waiters
    if (actual > 0)
        WakeupWaiters(OK);

    // If we read all the data, then wait for more
    if (actual <= size)
        return WaitForRead(conn.get(), timeout);

    // otherwise, yield so the other events can be processed
    else
        return Yield();
}



void ReadServerData::WakeupWaiters(bool status)
{
        buf.waiters.WakeupWaiters(status);
}

