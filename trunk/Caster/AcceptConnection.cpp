#include "AcceptConnection.h"
#include "Login.h"
#include "unistd.h"

// Make this a template which spawns the desired fragment type
//   once a connection is established.

 AcceptConnection::AcceptConnection(int port, MountTable& mnt) : Port(port), Mnt(mnt) {fd = -1;}

 AcceptConnection::~AcceptConnection(){::close(fd);}

 bool AcceptConnection::Resume() 
 {
    debug("AcceptConnection::Resume  fd=%d\n", fd);

    // Create a listening socket if not already done
    //NetAddress addr((127<<24)+1, 9998);
    NetAddress addr(0, Port);
    if (fd < 0 &&  Listen(addr, fd) != OK)
            return Abort("AcceptConnection can't create listen socket\n");
      
    // Accept a new connection;
    int newfd;
    if (Accept(fd, newfd) != OK) 
         return Abort("Failed to accept new connection\n");

    // If we got a connection, start a login sequence
    if (newfd >= 0) {
        Fragment *f = new Login(newfd, Mnt);
        if (f != NULL)
            f->Resume();
    }

    return WaitForRead(fd, -1);
}
    
bool AcceptConnection::Abort(const char* msg)
{
    Dispatcher::Stop();
    return Fragment::Abort(msg);
}

