/*
 * Socket.cpp
 *
 *  Created on: Jun 22, 2010
 *      Author: John
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>

#include "Socket.h"
#include "Util.h"


struct sockaddr NetAddress::GetAddr()
{
    struct sockaddr addr = {AF_INET};

    // The data consists of port and ip address, 
    //   stored in "big endian". 
    addr.sa_data[0] = Port>>8;
    addr.sa_data[1] = Port;
    addr.sa_data[2] = Dest>>24;
    addr.sa_data[3] = Dest>>16;
    addr.sa_data[4] = Dest>>8;
    addr.sa_data[5] = Dest;

    return addr;
}




Socket::Socket(Handle_t h) 
: Connection(h) 
{
    debug("Socket::Socket(%d)\n", handle);
    NonBlocking(handle);
}



bool Socket::Read(byte* buf, size_t size, ssize_t& actual)
{
    // Validity check.
    if (handle == BAD_HANDLE)
        return Error("Network read: bad file descriptor (%d)\n", handle);

    // Do a non-blocking read
    int fd = (int)handle;
    actual = ::read(fd, buf, size);

    // Check the return conditions for:  error, no data,  EOF
    // CASE: no data
    if (actual == -1 && errno == EAGAIN)
        actual = 0;
    // CASE: error
    else if (actual == -1)
        return SysError("Network Read failed on fd=%d\n", fd);
    // CASE: end of file
    else if (actual == 0)
        actual = -1;

    debug("Read fd=%d size=%d  actual=%d buf=", fd, size, actual);
    for (int i=0; i<actual; i++)
        debug("%c", buf[i]);
    debug("\n");

    // Note: actual = -1 designates EOF
    return OK;
}


bool Socket::Write(const byte* buf, size_t size, ssize_t& actual)
{
    // Validity check.
    if (handle == BAD_HANDLE)
        return Error("Network write: bad file descriptor (%d)]n", handle);

    // Do a non-blocking write. (no EPIPE signals!)
    int fd = (int)handle;
    actual = ::send(fd, buf, size, MSG_NOSIGNAL);
    if (actual == -1 && errno != EAGAIN)
        return SysError("Network Write faild on fd=%d\n", fd);
    else if (actual == -1)
         actual = 0;

    debug("Write fd=%d  size=%d actual=%d \n", fd, size, actual);
    return OK;
}




bool Socket::Connect(NetAddress& remote)
{
    return Error("Network Connect isn't implemented yet\n");
}


bool Socket::Listen(int port)
{
    NetAddress local(INADDR_ANY, port);

    // Bind to the address
    struct sockaddr addr = local.GetAddr();
    if (::bind(handle, &addr, sizeof(addr)) == -1)
        return SysError("Unable to bind listener to port %d\n",local.Port);

    // Set the listen queue
    if (::listen(handle, SOMAXCONN) != OK)
        return Error("Unable to set listen options for socket\n");

    // Put in nonblocking mode
    if (NonBlocking(handle) != OK)
       return Error();

    return OK;
}


bool Socket::Accept( Socket *(&returnSocket) )
{
    struct sockaddr remote;
    socklen_t addrlen = sizeof(remote);
    returnSocket = NULL;

    Handle_t newfd = ::accept(handle, &remote, &addrlen);
    if (newfd == -1 && errno != EAGAIN)
        return SysError("Network is unable to accept a connection\n");

    // Create a new socket instance
    bool err = OK;
    Socket_ptr newsock(new Socket(newfd));
    if (newsock.get() == NULL)
    	return Error("Accept - Unable to allocate Socket instance\n");

    // Everything is fine. Return a conventional pointer to the new socket
    returnSocket = newsock.release();
    return OK;
}


bool Socket::Accept( Socket_ptr &sock )
{
    Socket *ptr;
    bool status = Accept(ptr);
    sock.reset(ptr);
    return status;
}

// Some helper functions
Handle_t Socket::NewSocket()
{
    int fd = ::socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1) return BAD_HANDLE;
    return (Handle_t) fd;
}
    
bool Socket::NonBlocking(Handle_t fd)
{

  // Put the file descriptor into non-blocking mode
    int flags = ::fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return SysError("Unable to get socket state\n");
    if (::fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        return SysError("Unable to set socket nonblocking mode\n");

    return OK;
}

