#include "Login.h"
#include "SendSourceTable.h"

#include "string.h"
#include "unistd.h"


Login::Login(Connection_ptr& c, MountTable& mnt)
    : conn(c), actual(0), mounts(mnt)
//////////////////////////////////////////////////////////////////
// Creates a login code fragment which reads a login request.
//   It chains to either a client or server fragment depending
//   on the type of login
/////////////////////////////////////////////////////////////////
{
    buf[0] = '\0';
}


Login::~Login()
{
}



bool Login::Call(bool status)
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
    if (conn->Read(buf+actual, BufSize-actual, more) != OK)
        return Error("Lost connection during login\n");

    // Update the characters read so far
    actual += more;
    buf[actual] = 0;

    // if we don't have a complete buffer frame, try again
    if (!FrameComplete() && actual < BufSize)
        return WaitForRead(conn.get(), 10000);

    // if buffer is full,
    if (actual >= BufSize)    
        return Error("Login buffer is full, frame not complete\n");

    // Start parsing the buffer, starting with first token
    Parse token(buf, actual);
    token.Next(" \r\n");

    // Case: frame is a server request, process it
    if (token == "SOURCE")
        return ServerLogin(token);

    // Case: frame is a client request, process it
    else if (token == "GET")
        return ClientLogin(token);

    // Otherwise: error
    else
    	return Error("Unrecognized ntrip frame\n");
}        



bool Login::FrameComplete() 
{
    return  (strstr((char *)buf, "\r\n\r\n") != NULL);
}


bool Login::ServerLogin(Parse& token) 
{    
    // Get the password and mount point
    char name[MaxName+1];  char password[MaxPassword+1];
    if (GetHeader(token, name, password) != OK)
        return Shutdown("ERROR - Mount Point Invalid\r\n\r\n");
    
    // Locate the mount point
    MountPoint* mnt = mounts.Lookup(name);
    if (mnt == NULL) 
        return Shutdown("ERROR - Mount Point Invalid\r\n\r\n");

    // Check the password
    if (!mnt->ValidPassword(password)) 
        return Shutdown("ERROR - Bad Password\r\n\r\n");

    // Make sure it is isn't already mounted
    if (mnt->IsMounted()) 
        return Shutdown("ERROR - Mount Point Taken\r\n\r\n");

    // Mount it
    if (mnt->Mount() != OK)
        return Error("Unable to Mount %s\n");

    // Send the OK message
    WriteShort("ICY 200 OK\r\n\r\n");

    // Switch to task for reading server data
    return Switch(new ReadServerData(conn, mnt));
}


bool Login::WriteShort(const char *str)
{
    ssize_t actual;
    size_t len = strlen(str);
    
    if (conn->Write((byte*)str, len, actual) != OK || len != actual)
        return Error("WriteShort wrote %d of % bytes\n", actual, len);

    return OK;
}

    
bool Login::GetHeader(Parse& token, char name[MaxName+1], char password[MaxPassword+1])
{
    token.Next("/\r\n");
    if (token.GetDelimiter() != '/')
        return !OK;
    if (token.GetToken(password, MaxPassword+1) != OK)
        return !OK;

    token.Next(" \r\n");
    if (token.GetToken(name, MaxName+1) != OK)
        return !OK;

    return OK;
}

bool Login::Shutdown(const char* str)
{
    WriteShort(str);
    return !OK;
}
    
    
bool Login::ClientLogin(Parse& token) 
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
    WriteShort("ICY 200 OK\r\n\r\n");

    // Switch control to the client fragment. We are done.
    return Switch(new SendClientData(conn, mnt));
}

bool Login::SendTable()
/////////////////////////////////////////////////////////////////////
// Send the table of valid mount points to the client
/////////////////////////////////////////////////////////////////////
{
    // Send a message the table is coming
    WriteShort("SOURCETABLE 200 OK\r\n");

    // Start sending the list of mount points
    return Switch(new SendSourceTable(conn, mounts));
}
