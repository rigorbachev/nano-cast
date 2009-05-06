#ifndef NetworkIncluded
#define NetworkIncluded

//////////////////////////////////////////////////////////////////////////////////
//
// A Network object manages a set of network connections
//   It implements an asynchronous network model where
//   a singlethread application manages many connections.
//
// For efficiency on Linux, it uses /dev/epoll
//
// Currently it is based on Linux file descriptors.
//   TODO: create a socket object which hides the OS dependencies.
//
//////////////////////////////////////////////////////////////////////////

#include "Util.h"
#include "Poll.h"

class NetAddress {
public:
    NetAddress(uint32 dest, int port) : Dest(dest), Port(port) {}
    NetAddress(const char* name, const char* service);

    struct sockaddr GetAddr();

public:
    uint32 Dest;
    int Port;
};


class Network {
public:
    Network(int maxfds);
    ~Network();
    
    bool Connect(NetAddress& addr, int& newfd);
    bool Listen(NetAddress& addr, int& newfd);
    bool Accept(int fd, int& newfd);

    bool Write(int fd, const byte* buf, int size, int& actual);
    bool Read(int fd, byte* buf, int size, int& actual);

    bool Close(int fd);
    bool IsConnected(int fd);

    bool Reap(PollEvent* events, int max, int& actual, int timeout);

protected:
    Poll poll;
    int MaxFds;
 
private:
    bool NonBlocking(int fd);

};


#endif // SocketIncluded
