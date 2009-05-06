#include "SendSourceTable.h"

#include <unistd.h>
#include <string.h>




SendSourceTable::SendSourceTable(int fd, MountTable& mounts)
    : Fd(fd), Mounts(mounts), Next(-1)
{
}


SendSourceTable::~SendSourceTable()
{
    Close();
}

bool SendSourceTable::Close()
{
    ::close(Fd);
    Fd = -1;
    return OK;
}


bool SendSourceTable::Resume()
///////////////////////////////////////////////////////////////////////////////
// Resume continues sending the SRC table to the client when an event occurs
///////////////////////////////////////////////////////////////////////////////
{

    // Repeat for each mount point [0..n-1], with an added header [-1] and trailer [n] and shutdown [n+1]
    for (;;) {

        // Finish sending the previous buffer if any
        while (Actual < Len) {
            int actual;
            if (Write(Fd, Buf+Actual, Len-Actual, actual) != OK || actual < 0)
                return Abort("Write problems sending the Source Table\n");

            Actual += actual;
            if (Actual < Len)
                return WaitForRead(Fd, 10000);
        }

        // if no more records to send, then done
        if (Next > Mounts.MaxMounts+1)  break;

        // Process according to which record we are at.
        // Case: -1 -- Header record  
        if (Next == -1) {
           
            // Figure out how much characters in message
            int total = 0;
            for (int i=0; i<Mounts.MaxMounts; i++)
                if (Mounts[i].State == MOUNTED)
                    total += strlen(Mounts.Mounts[i].STR);   // Add each mounted STR
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
        else if (Next < Mounts.MaxMounts) {

            // if Mounted, then send STR record
            if (Mounts[Next].State == MOUNTED) {
                Buf = (byte *)Mounts[Next].STR; Len = strlen(Mounts[Next].STR); Actual = 0;
            }
        }

        // Case: (otherwise) send the trailing "\r\n"
        else if (Next == Mounts.MaxMounts) {
            Buf = (const byte *)"\r\n"; Len = 2; Actual = 0;
        }

        // Case: We are finished. 
        else {
            Close();
            Abort();
        }

        Next++;
    }

    return OK;
}

bool SendSourceTable::Abort(const char* str)
{
    if (str != NULL)
        {::close(Fd); Fd=-1;}

    return Fragment::Abort(str);
}
    
        
        
        
