#include "Network.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>


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

Network::Network(int maxfds)
    : MaxFds(maxfds), poll(maxfds)
{
}

Network::~Network()
{
}


bool Network::Reap(PollEvent* events, int max, int& actual, int timeout)
{
    return poll.Reap(events, max, actual, timeout);
}


bool Network::Connect(NetAddress& remote, int& newfd)
{
    newfd = -1;
    return Error("Network Connect isn't implemented yet\n");
}


bool Network::Listen(NetAddress& local, int& newfd)
{
    newfd = socket(PF_INET, SOCK_STREAM, 0);
    if (newfd < 0) return SysError("Unable to Create a new socket\n");

    // Bind to the address
    struct sockaddr addr = local.GetAddr();
    if (bind(newfd, &addr, sizeof(addr)) == -1)
    {
        ::close(newfd); 
        return SysError("Unable to bind listener to port %d\n",local.Port);
    }

    // Set the listen queue
    if (listen(newfd, SOMAXCONN) != OK) 
        {close(newfd); return Error("Unable to set options for socket\n");}

    // Put in nonblocking mode
    if (NonBlocking(newfd) != OK) 
       {close(newfd); return Error();}

    // Register for polling
    if (poll.Register(newfd) != OK) 
        {close(newfd); return Error();}

    return OK;  
}


bool Network::Accept(int fd, int& newfd)
{
    struct sockaddr remote;
    socklen_t addrlen = sizeof(remote);
    
    newfd = ::accept(fd, &remote, &addrlen);
    if (newfd == -1 && errno != EAGAIN)
        return SysError("Network is unable to accept a connection\n");

    // put the new socket into non-blocking mode
    if (newfd != -1 && NonBlocking(newfd) != OK)
        return Error("Unable to put accepted connection into non-blocking mode\n");

    // Register the new fd so we can tell when something happens
    if (newfd != -1 && poll.Register(newfd) != OK)
        {close(newfd); return Error("Can't register new connection fd=%d\n", fd);}

    // No errors.  We return newfd=-1 when the data isn't ready yet
    debug("Accept: fd=%d  newfd=%d\n", fd, newfd);
    return OK;
}


bool Network::Read(int fd, byte* buf, int size, int& actual)
{
    // Validity check.
    if (fd < 0 || fd >= MaxFds)
        return Error("Network read: bad file descriptor (%d)\n", fd);

    // Do a non-blocking read
    actual = ::read(fd, buf, size);

    // Check the return conditions for:  error, no data,  EOF
    if (actual == -1 && errno != EAGAIN)
        return SysError("Network Read failed on fd=%d\n", fd);
    else if (actual == -1)
        actual = 0;
    else if (actual == 0)
        actual = -1;

    debug("Read fd=%d  actual=%d buf=", fd, actual);
    for (int i=0; i<actual; i++)
        debug("%c", buf[i]);
    debug("\n");

    return OK;

    // Note: actual = -1 designates EOF
    return OK;
}


bool Network::Write(int fd, const byte* buf, int size, int& actual)
{
    // Validity check.
    if (fd < 0 || fd >= MaxFds)
        return Error("Network write: bad file descriptor (%d)]n", fd);

    // Do a non-blocking write. (no EPIPE signals!)
    actual = ::send(fd, buf, size, MSG_NOSIGNAL);
    if (actual == -1 && errno != EAGAIN)
        return SysError("Network Write faild on fd=%d\n", fd);
    else if (actual == -1)
         actual = 0;

    debug("Write fd=%d  size=%d actual=%d \n", fd, actual);
    return OK;
}

bool Network::NonBlocking(int fd)
{

  // Put the file descriptor into non-blocking mode
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)  
        return SysError("Unable to get socket state\n");
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        return SysError("Unable to set socket nonblocking mode\n");

    return OK;
}


