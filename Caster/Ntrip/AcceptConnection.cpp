
#include "AcceptConnection.h"
#include "Login.h"
#include "Socket.h"
#include "Dispatcher.h"


// Make this a template which spawns the desired fragment type
//   once a connection is established.

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
   if (sock->Accept(s) != OK)
        return Error("Failed to accept new connection\n");
   Connection_ptr newsock(s);

   // We got a connection, start a login sequence
   Dispatcher::Call(new Login(newsock, Mnt), OK);

   return WaitForRead(sock.get());
}
    

