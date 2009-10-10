#include "ReadServerData.h"
#include <algorithm>
#include <unistd.h>



ReadServerData::ReadServerData(int pfd, MountPoint* mnt, int timeout) 
    : fd(pfd), Mnt(mnt), Timeout(timeout), buf(mnt->Buf)
{
}



ReadServerData::~ReadServerData()
{
}


bool ReadServerData::Abort(const char* msg)
{
    // Tell listeners to shutdown
    WakeupWaiters();

    // Unmount
    Mnt->Unmount();

    // Close the socket to the server
    ::close(fd);
    fd = -1;

    // Now really exit
    Fragment::Abort(msg);
    Error("Lost connection to ntrip server. Shutting down.\n");
 
    return OK;
}





bool ReadServerData::Resume()
{
    debug("ReadServerData: Resume  fd=%d\n", fd);
    // Keep reading while there is more data
    int actual, size;
    do {

        // Read data, but not so much the consumers get left behind
        int start = buf.count % buf.BufSize;
        size = min(TooFarBehind/2, buf.BufSize-start);
        if (Read(fd, buf.Buffer+start, size, actual) !=  OK)
            return Abort("Problems reading input data stream\n");
        if (actual == -1)
            return Abort("Server has closed connection\n");

        // Update the number of bytes transferred
        buf.count += actual;

        // if we received data, wakeup waiters
        if (actual > 0)
            WakeupWaiters();

    } while (actual == size);

    // Sleep, waiting for more input data
    return WaitForRead(fd, Timeout);
}


void ReadServerData::WakeupWaiters()
{
        // Copy the waiters list to a safe place  (rewrite)
        //   otherwise, a waiter can add themself back to the list and we loop
        FragList temp;
        while (buf.Waiters.Head() != NULL)
            temp.Add(buf.Waiters.Pop());

        // Wake up each of the waiters
        while (temp.Head() != NULL)
            temp.Pop()->Resume();
}

