#include "Login.h"
#include "SendSourceTable.h"

#include "string.h"
#include "unistd.h"


Login::Login(int fd, MountTable& mnt)
    : Fd(fd), Actual(0), Mounts(mnt) 
//////////////////////////////////////////////////////////////////
// Creates a login code fragment which reads a login request.
//   It chains to either a client or server fragment depending
//   on the type of login
/////////////////////////////////////////////////////////////////
{
    Buf[0] = '\0';
}


Login::~Login()
{
}


bool Login::Abort(const char* msg)
//////////////////////////////////////////////////////////
// Abort causes the code fragment to quit with an error message
//////////////////////////////////////////////////////////////
{ 
    // For a normal exit (no message) keep the Fd open
    if (msg != NULL)  Close();
    Fragment::Abort(msg);
}

bool Login::Resume() 
///////////////////////////////////////////////////////////////////////////////////////
// Resume this login fragment when an event occurs
//////////////////////////////////////////////////////////////////////////////////
{
    debug("Login::Resume  Fd=%d  Actual=%d\n", Fd, Actual);
    // Read data into the buffer until we have a complete frame 
    int actual;      
    if (Read(Fd, Buf+Actual, BufSize-Actual, actual) != OK) return Error();
    if (actual == -1) return Abort("Lost connection during login\n");

    // Update the characters read so far
    Actual += actual;
    Buf[Actual] = 0;

    // if we don't have a complete buffer frame, try again
    if (!FrameComplete()) 
        return WaitForRead(Fd, 10000);

    // if buffer is full,
    if (Actual >= BufSize)    
        return Abort("Login buffer is full, frame not complete\n");

    // Start parsing the buffer, starting with first token
    Parse token(Buf, Actual);
    token.Next(" \r\n");

    // Case: frame is a server request, process it
    if (token == "SOURCE")
        return ServerLogin(token);

    // Case: fram is a client request, process it
    else if (token == "GET")
        return ClientLogin(token);

    // Otherwise: error
    else return Abort("Unrecognized ntrip frame\n");
        

}        



bool Login::FrameComplete() 
{
    return  (strstr((char *)Buf, "\r\n\r\n") != NULL);
}


bool Login::ServerLogin(Parse& token) 
{    
    // Get the password and mount point
    char name[MaxName+1];  char password[MaxPassword+1];
    if (GetHeader(token, name, password) != OK)
        return Shutdown("ERROR - Mount Point Invalid\r\n");
    
    // Locate the mount point
    MountPoint* mnt = Mounts.Lookup(name);
    if (mnt == NULL) 
        return Shutdown("ERROR - Mount Point Invalid\r\n");

    // Check the password
    if (!mnt->ValidPassword(password)) 
        return Shutdown("ERROR - Bad Password\r\n");

    // Make sure it is isn't already mounted
    if (mnt->IsMounted()) 
        return Shutdown("ERROR - Mount Point Taken\r\n");

    // Mount it
    if (mnt->Mount() != OK)
        return Abort("Unable to Mount %s\n");

    // Create a fragment to read the data
    ReadServerData* server = new ReadServerData(Fd, mnt);
    if (server == NULL) 
        return Abort("Can't create task to read server data\n");

    // Send the OK message
    WriteShort("ICY 200 OK\r\n");

    // Switch to task for reading server data
    return Switch(server);
}


bool Login::WriteShort(const char *str)
{
    int actual;
    int len = strlen(str);
    
    if (Write(Fd, (byte*)str, len, actual) != OK || len != actual)
        return Error("WriteShort wrote %d of % bytes\n", actual, len);

    return OK;
}

bool Login::Close()
{
    ::close(Fd);
    Fd = -1;
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
    Close();
    Abort();
    return OK;
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
    MountPoint* mnt = Mounts.Lookup(name);
    if (mnt == NULL || mnt->State != MOUNTED)
        return SendTable();

    // Start sending data to client
    SendClientData* client = new SendClientData(Fd, mnt);
    if (client == NULL) 
        return Abort("Can't create task to read server data\n");

    // Send a message saying "all is fine"
    WriteShort("ICY 200 OK\r\n");

    // Switch control to the client fragment. We are done.
    return Switch(client);
}

bool Login::SendTable()
/////////////////////////////////////////////////////////////////////
// Send the table of valid mount points to the client
/////////////////////////////////////////////////////////////////////
{

    // Create a fragment to send the source table
    SendSourceTable* sender = new SendSourceTable(Fd, Mounts);
    if (sender == NULL)
        return Abort("Can't crate task to send source table\n");

    // Send a message the table is coming
    WriteShort("SOURCETABLE 200 OK\r\n");

    // Switch control to task
    return Switch(sender);
}
