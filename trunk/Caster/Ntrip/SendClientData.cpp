#include "SendClientData.h"
#include <algorithm>
#include <unistd.h>




SendClientData::SendClientData(Connection_ptr& c, MountPoint* mnt)
    : conn(c), Mnt(mnt), buf(mnt->Buf)
{
    count = buf.count;
}



SendClientData::~SendClientData()
{
}



bool SendClientData::Call(bool status)
/////////////////////////////////////////////////////////////////////////////
// Call sending data to the client after an event occurs
/////////////////////////////////////////////////////////////////////////////
{
    debug("SendClientData: Resume  count=%d buf.count=%ld\n", count, buf.count);

    // If no longer mounted, then exit
    if (Mnt->State != MOUNTED) 
        return Error("Mountpoint disappeared\n");
    
    // Keep writing until we are caught up or we can't write any more
    ssize_t size, actual;
    while (count < buf.count) {

         // If we are too far behind, then quit
         if (count+TooFarBehind < buf.count)
             return Error("Client is falling behind\n");

        // Write as much data as we can, up to end of buffer
        size_t start = count % buf.BufSize;
        size = min<size_t>(buf.count-count, buf.BufSize-start);
        if (conn->Write(buf.Buffer+start, size, actual) != OK)
            return Error("Problems writing to client\n");

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
        return Error("Client has closed connection\n");

    // CASE: client is busy. Send more when we can.
    else 
        return WaitForWrite(conn.get(), 10000);
}



    


bool SendClientData::WaitForBuf()
{
    buf.waiters.Wait(this);
    return OK;
}

        
