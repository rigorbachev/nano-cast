/*
 * Connection.h
 *
 *  Created on: Jun 21, 2010
 *      Author: John
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <memory>
#include "Util.h"
#include "Poll.h"

class Connection;
typedef std::auto_ptr<Connection> Connection_ptr;


class Connection : public Pollable {

public:
        Connection(Handle_t h);

	bool isConnected() {return handle != BAD_HANDLE;}

	virtual bool Read(byte* buf, size_t count, ssize_t &actual) = 0;
	virtual bool Write(const byte* buf, size_t count, ssize_t &actual) = 0;
};




#endif /* CONNECTION_H_ */
