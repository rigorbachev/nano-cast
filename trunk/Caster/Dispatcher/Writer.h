/*
 * Writer.h
 *
 *  Created on: Jul 12, 2010
 *      Author: John
 */

#ifndef WRITER_H_
#define WRITER_H_

#include "CallBack.h"
#include "Connection.h"

class Writer: public CallBack {
public:
     Writer(Connection_ptr& c, CallBack* cb, const char* format ...);
	Writer(Connection_ptr& c, CallBack* cb, const byte* b, size_t s, int timeout=10000);
	virtual ~Writer();
	virtual bool Call(bool status);

protected:
	Connection_ptr conn;
	CallBack* callback;
	const byte* buf;
	size_t size;
     int timeout;
};

#endif /* WRITER_H_ */
