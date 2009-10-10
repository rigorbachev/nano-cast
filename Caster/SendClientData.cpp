#include "SendClientData.h"
#include <algorithm>
#include <unistd.h>




SendClientData::SendClientData(int pfd, MountPoint* mnt)
    : fd(pfd), Mnt(mnt), buf(mnt->Buf), Fragment()
{
    count = buf.count;
}



SendClientData::~SendClientData()
{
    ::close(fd);
}


bool SendClientData::Abort(const char* msg)
{
    // This is the place to try to recover
    //   Maybe attempt to reconnect
    //   Maybe tell listeners to shutdown
    //   Maybe exit everything and start over
    Error("Lost connection to ntrip client. Shutting down.\n");
    Fragment::Abort(msg);
 
    return OK;
}



bool SendClientData::Resume()
/////////////////////////////////////////////////////////////////////////////
// Resume sending data to the client after an event occurs
/////////////////////////////////////////////////////////////////////////////
{
    debug("SendClientData: Resume  fd=%d count=%d buf.count=%lld\n", fd, count, buf.count);

    // If no longer mounted, then exit
    if (Mnt->State != MOUNTED)  Abort("Mountpoint disappeared\n");
    
    // Keep writing until we are caught up or we can't write any more
    int size, actual;
    while (count < buf.count) {

         // If we are too far behind, then quit
         if (count+TooFarBehind < buf.count)
             return Abort("Aborting because client is falling behind\n");

        // Write as much data as we can, up to end of buffer
        int start = count % buf.BufSize;
        size = min<size_t>(buf.count-count, buf.BufSize-start);
        if (Write(fd, buf.Buffer+start, size, actual) != OK)
            return Abort("Problems writing to client\n");

        // update the number of bytes sent to client
        if (actual != -1)
            count += actual;

    // End "keep writing until ..."
        if (actual != size)
            break;
    }


    // CASE: we have sent all the data to client. Wait for more.
    if (count == buf.count)
        return WaitForBuf();
    
    // CASE: the client connection is closed.  Terminate the session.
    else if (actual == -1)
        return Abort("Client has closed connection\n");

    // CASE: client is busy. Send more when we can.
    else 
        return WaitForWrite(fd, 10000);
}



    


bool SendClientData::WaitForBuf()
{
    buf.Waiters.Add(this);
    return OK;
}

        
