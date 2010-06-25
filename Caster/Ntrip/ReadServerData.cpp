#include "ReadServerData.h"
#include <algorithm>
#include <unistd.h>



ReadServerData::ReadServerData(Connection_ptr& c, MountPoint* m, int to)
    : conn(c), mnt(m), timeout(to), buf(mnt->Buf)
{
}



ReadServerData::~ReadServerData()
{
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
    do {

        // Read data, but not so much the consumers get left behind
        int start = buf.count % buf.BufSize;
        size = min(TooFarBehind/2, buf.BufSize-start);
        if (conn->Read(buf.Buffer+start, size, actual) !=  OK)
            return Error("Problems reading input data stream\n");
        if (actual == -1)
            return Error("Server has closed connection\n");

        // Update the number of bytes transferred
        buf.count += actual;

        // if we received data, wakeup waiters
        if (actual > 0)
            WakeupWaiters(OK);

    } while (actual == size);

    // Sleep, waiting for more input data
    if (WaitForRead(conn.get(), timeout) != OK)
    	return Error();

    return OK;
}



void ReadServerData::WakeupWaiters(bool status)
{
        buf.waiters.WakeupWaiters(status);
}

