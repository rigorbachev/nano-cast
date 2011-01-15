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
#include <boost/shared_ptr.hpp>


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
//typedef std::auto_ptr<Socket> Socket_ptr;
typedef boost::shared_ptr<Socket> Socket_ptr;


class Socket : public Connection {
public:
	Socket(Handle_t handle = NewSocket());

	Status Connect(NetAddress& addr);
	Status Listen(int port);

        Status Accept(Socket*& s);
        template <typename T>
            Status Accept( boost::shared_ptr<T>& sp )
            {
                Socket* ptr;  
                if (Accept(ptr)!= OK) return !OK;  
                sp.reset((T*)ptr); 
                return OK;
            }

	Status Read(byte* buf, size_t len, ssize_t &actual);
	Status Write(const byte* buf, size_t len, ssize_t &actual);
     
        static Handle_t NewSocket();
        static Status NonBlocking(Handle_t fd);

};

class Listener;
//typedef std::auto_ptr<Listener> Listener_ptr;
typedef boost::shared_ptr<Listener> Listener_ptr;

class Listener : public Socket {
public:
    Listener(int port) {Listen(port);}
};



#endif /* SOCKET_H_ */
