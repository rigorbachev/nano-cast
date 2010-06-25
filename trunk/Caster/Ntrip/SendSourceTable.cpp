#include <unistd.h>
#include <string.h>

#include "SendSourceTable.h"


SendSourceTable::SendSourceTable(Connection_ptr& c, MountTable& m)
    : conn(c), mounts(m), Next(-1)
{
}


SendSourceTable::~SendSourceTable()
{
}


bool SendSourceTable::Call(bool status)
///////////////////////////////////////////////////////////////////////////////
// Call continues sending the SRC table to the client when an event occurs
///////////////////////////////////////////////////////////////////////////////
{
    // If problems sending table, then quit
    if (status != OK)
        return Error("Was unable to send the source table\n");

    // Repeat for each mount point [0..n-1], 
    //     with an added header [-1] and trailer [n] and shutdown [n+1]
    for (;Next <= mounts.MaxMounts; Next++) {

        // Finish sending the previous buffer if any
        while (Actual < Len) {
            ssize_t actual;
            if (conn->Write(Buf+Actual, Len-Actual, actual) != OK || actual < 0)
                return Error("Write problems sending the Source Table\n");

            Actual += actual;
            if (Actual < Len)
                return WaitForRead(conn.get(), 10000);
        }

        // Process according to which record we are at.
        // Case: -1 -- Header record  
        if (Next == -1) {
           
            // Figure out how much characters in message
            size_t total = 0;
            for (int i=0; i<mounts.MaxMounts; i++)
                if (mounts[i].State == MOUNTED)
                    total += strlen(mounts.Mounts[i].STR);   // Add each mounted STR
            total += 2; // Add final /r/n

            // NOTE: if streams are mounted or dismounted while sending the table, 
            //       then the count will be invalid.
            //       We will assume it doesn't matter for now.

            // Build a buffer containing the message header
            snprintf(Header, sizeof(Header), "SOURCETABLE 200 OK\r\n"
                                             "Server: "CASTER_SW"/1.0\r\n"
                                             "Content-Type: text/plain\r\n"
                                             "Content-Length: %d\r\n"
                                             "\r\n", total);

            // Start Sending the header
            Buf = (byte *)Header; Len = strlen(Header); Actual = 0;
        }

        // Case: 0..n-1.  Sending an STR record
        else if (Next < mounts.MaxMounts) {

            // if Mounted, then send STR record
            if (mounts[Next].State == MOUNTED) {
                Buf = (byte *)mounts[Next].STR; Len = strlen(mounts[Next].STR); Actual = 0;
            }
        }

        // Case: n.  Send the trailing "\r\n"
        else if (Next == mounts.MaxMounts) {
            Buf = (const byte *)"\r\n"; Len = 2; Actual = 0;
        }
    }

    // We are done. Tell Dispatcher we can be deleted.
    return !OK;
}

