#include "Login.h"
#include "SendSourceTable.h"
#include "Caster.h"
#include "Date.h"

#include <string.h>
#include <unistd.h>


Login::Login(Connection_ptr& c, MountTable& mnt)
    : conn(c), actual(0), mounts(mnt)
//////////////////////////////////////////////////////////////////
// Creates a login code fragment which reads a login request.
//   It chains to either a client or server fragment depending
//   on the type of login
/////////////////////////////////////////////////////////////////
{
    conn->buffer[0] = '\0';
    Call=Login::GetHeader;
}


Login::~Login()
{
}



Status Login::GetHeader(Status status)
///////////////////////////////////////////////////////////////////////////////////////
// Call this login fragment when an event occurs
//////////////////////////////////////////////////////////////////////////////////
{
    debug("Login::Call  status=%d\n", status);
    // if timed out, then done
    if (status != OK)
    	return Error("Login sequence timed out\n");

    // Read data into the buffer until we have a complete frame 
    ssize_t more;
    if (conn->Read(conn->buffer+actual, conn->MaxBuf-actual-1, more) != OK)
        return Error("Lost connection during login\n");

    // Update the characters read so far
    actual += more;
    buf[actual] = 0;

    // if we don't have a complete buffer frame, try again
    if (!FrameComplete() && actual < MaxBuf-1)
        return WaitForRead(conn.get(), 10000);

    // if buffer is full,
    if (actual >= MaxBuf-1)
        return Error("Login buffer is full, frame not complete\n");

    // Start parsing the buffer, starting with first token
    Parse token(buf, actual);
    token.Next(" \r\n");

    // Process according to the type of request
    if      (token == "POST")    return ServerLogin(token);
    else if (token == "GET")     return ClientLogin(token);
    else                         return Error("Unrecognized ntrip frame\n");
}        



bool Login::FrameComplete() 
{
    return  (strstr((char *)buf, "\r\n\r\n") != NULL);
}


Status Login::ServerLogin(Parse& token) 
{    
    // Get the mount point
    token.Next(" \r\n");
    MountPoint* mnt = mounts.Lookup(token);
    if (mnt == NULL)
        return Shutdown("ERROR - Mount Point Invalid\r\n\r\n");

    // Make sure it is isn't already mounted
    if (mnt->IsMounted()) 
        return Shutdown("ERROR - Mount Point Taken\r\n\r\n");

    // Verify the protocol is correct
    token.Next(" \r\n");
    if (token != "HTTP/1.1")
        return Shutdown("ERROR - Invalid HTTP protocol\r\n\r\n");
        
    // Keep track of which fields we've seen
    bool host, version, agent, authorization;
    host = version = agent = authorization = false;

    // Repeat for each line of the header
    while (token.Next(":" != "")) {

        // Process According to the field
        if (token == "Host") {
            token.Next(" \r\n");
            host = true;
        } 
        else if (token == "Ntrip-Version") {
            if (token.Next(" \r\n") != "Ntrip/2.0")
                return Shutdown("ERROR - Only Ntrip/2.0 is supported\r\n\r\n");
            version = true;
        }
        else if (token == "User-Agent") {
            if (token.Next(" \r\n") != "NTRIP")
              return Shutdown("ERROR - Must have NTRIP as User-Agent\r\n\r\n");
            agent = true;
        }
        else if (token == "Authorization") {
            if (token.Next(" \r\n") != "Basic")
                return Shutdown("ERROR - Only supports Basic Authorization\r\n\r\n");       
            if (!mnt->ValidPassword(token.Next(" \r\n")))
                return Shutdown("ERROR - Bad Password\r\n\r\n");
            authorization = true;
        }
    }

    // Verify all necessary fields are present
    if (!version || !agent || !authorization)
    	return Shutdown("ERROR -- Incomplete Ntrip request\r\n\r\n");

    // Mount it
    if (mnt->Mount() != OK)
        return Error("Unable to Mount %s\n");

    // Send the OK message.
    //  TODO: create subtask in case can't do it all at once
    c.Print("HTTP/1.1 200 OK\r\n"
               "Ntrip-Version: Ntrip/2.0\r\n"
               "Server: NTRIP NanoCast %s\r\n"
               "Date: %s\r\n"
               "Content-Type: gnss/data\r\n"
               "\r\n", NanoCastVersion, getDateStr());

    // Switch to task for reading server data
    return Switch(ReadServerData);
}



Status Login::Shutdown(const char* str)
{
    c.WriteShort(str);
    return Error();
}
    
    
Status Login::ClientLogin(Parse& token) 
//////////////////////////////////////////////////////////////////////////////
// ClientLogin handles the login of a client. Passwords are not checked.
///////////////////////////////////////////////////////////////////////////
{    
    // Get the mount point and password. (password is ignored for clients)
    // If it doesn't parse, then send the list of mount points
    char password[MaxPassword+1]; char name[MaxName+1];
    if (GetHeader(token, name, password) != OK || name[0] == '\0')
        return SendTable();
    
    // Locate the mount point, sending the list of mount points if it doesn't exist
    MountPoint* mnt = mounts.Lookup(name);
    if (mnt == NULL || mnt->State != MOUNTED)
        return SendTable();

    // Send a message saying "all is fine"
    // Send the OK message.
    //  TODO: create subtask in case can't do it all at once
    c.Print("HTTP/1.1 200 OK\r\n"
               "Ntrip-Version: Ntrip/2.0\r\n"
               "Server: NTRIP NanoCast %s\r\n"
               "Date: %s\r\n"
               "Content-Type: gnss/data\r\n"
               "\r\n", NanoCastVersion, getDateStr());


    // Switch control to the client fragment. We are done.
    return Switch(SendClientData);
}

bool Login::SendTable()
/////////////////////////////////////////////////////////////////////
// Send the table of valid mount points to the client
/////////////////////////////////////////////////////////////////////
{
    // Send a message the table is coming
    // Send the OK message.
    //  TODO: create subtask in case can't do it all at once
    c.Print("HTTP/1.1 200 OK\r\n"
               "Ntrip-Version: Ntrip/2.0\r\n"
               "Server: NTRIP NanoCast %s\r\n"
               "Date: %s\r\n"
               "Content-Type: gnss/sourcetable\r\n"
               "\r\n", NanoCastVersion, getDateStr());


    // Start sending the list of mount points
    return Switch(new SendSourceTable(conn, mounts));
}
