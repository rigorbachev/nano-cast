/*
 * Writer.cpp
 *
 *  Created on: Jul 12, 2010
 *      Author: John
 */

#include "Writer.h"
Writer::Writer(Connection_ptr& c, CallBack* cb, const char* format, ...)
: conn(c), buf(c->buffer), size(strlen((char*)buf)), callback(cb), timeout(10000)
{
    status = Error("Writer:Writer constructor not implemented\n");
}


Writer::Writer(Connection_ptr& c, CallBack* cb, const byte* b, size_t s, int t)
: conn(c), buf(b), size(s), callback(cb), timeout(t)
{
}

Writer::~Writer()
{
}

bool Writer::Call(bool status)
{
	// Problems encountered, probably timed out
	if (status != OK)
		return Switch(callback, Error("Writer: timed out\n"));

	// If we got bad write, let the callback know.
	ssize_t actual;
	if (conn->Write(buf, size, actual) != OK || actual == -1)
		return Switch(callback, Error("Writer: can't write\n"));

	// Make note of the new data
	buf += actual; size -= actual;

	// Write more data if need be
	if (size > 0)
		return WaitForWrite(conn.get(), timeout);

	// We are done. Invoke the callback
	return Switch(callback, OK);
}
