/*
 * Connection.h
 *
 *  Created on: Jun 21, 2010
 *      Author: John
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "Util.h"
#include "Poll.h"
#include <boost/shared_ptr.hpp>

class Connection;
//typedef std::auto_ptr<Connection> Connection_ptr;
typedef boost::shared_ptr<Connection> Connection_ptr;


class Connection : public Pollable {

public:
    Connection(Handle_t h);

	bool isConnected() {return handle != BAD_HANDLE;}

	virtual bool Read(byte* buf, size_t count, ssize_t &actual) = 0;
	virtual bool Write(const byte* buf, size_t count, ssize_t &actual) = 0;
	bool WriteShort(const byte* buf, size_t count);
	bool WriteShort(const char* str)
	    {return WriteShort((byte*)str, strlen(str));}
	bool Print(const char* fmt, ...);

public:
	// give each connection a buffer
	const static int MaxBuf = 512;
	byte buffer[MaxBuf];
};




#endif /* CONNECTION_H_ */
