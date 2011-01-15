// TODO: Restrict the number of connected sessions.
//   Don't want to run out of resources from having too many connections.

#include "AcceptConnection.h"
#include "Login.h"
#include "Socket.h"
#include "Dispatcher.h"


AcceptConnection::AcceptConnection(int port, MountTable& mnt)
     : Mnt(mnt), sock(new Listener(port))
{
    // Wait for an incoming connection
    WaitForRead(sock.get());
    
}

AcceptConnection::~AcceptConnection()
{
}

bool AcceptConnection::Call(bool status)
{
   debug("AcceptConnection::Call(%d)\n", status);

   // Accept a new connection;
   Socket* s;
   if (sock->Accept(s) != OK || s == NULL)
        return Error("Failed to accept new connection\n");
   Connection_ptr newsock(s);

   // We got a connection, start a login sequence
   Dispatcher::Call(new Login(newsock, Mnt), OK);

   return WaitForRead(sock.get());
}
    

