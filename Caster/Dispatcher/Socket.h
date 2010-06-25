/*
 * Socket.h
 *
 *  Created on: Jun 22, 2010
 *      Author: John
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <memory>

#include "Util.h"
#include "Connection.h"


class NetAddress {
public:
    NetAddress(uint32 dest, int port) : Dest(dest), Port(port) {}
    NetAddress(const char* name, const char* service);

    struct sockaddr GetAddr();

public:
    uint32 Dest;
    int Port;
};



// An automatic pointer for Sockets
class Socket;
typedef std::auto_ptr<Socket> Socket_ptr;


class Socket : public Connection {
public:
	Socket(Handle_t handle = NewSocket());

	bool Connect(NetAddress& addr);
	bool Listen(int port);
	bool Accept(Socket *(&newsock));
        bool Accept(Socket_ptr &newsock);

	bool Read(byte* buf, size_t len, ssize_t &actual);
	bool Write(const byte* buf, size_t len, ssize_t &actual);
     
        static Handle_t NewSocket();
        static bool NonBlocking(Handle_t fd);
};

class Listener;
typedef std::auto_ptr<Listener> Listener_ptr;

class Listener : public Socket {
public:
    Listener(int port) {Listen(port);}
};



#endif /* SOCKET_H_ */
