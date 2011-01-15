/*
 * Connection.cpp
 *
 *  Created on: Jun 21, 2010
 *      Author: John
 */

#include <unistd.h>
#include <stdio.h>
#include "Connection.h"

Connection::Connection(Handle_t h)
: Pollable(h)
{}


bool Connection::WriteShort(const byte* buf, size_t size)
{
	ssize_t actual;
	if (Write(buf, size, actual) != OK)
		return !OK;
	if (actual != size)
		return Error("WriteShort wrote partial buffer\n");
	return OK;
}

bool Connection::Print(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int ret = vsnprintf((char *)buffer, MaxBuf, fmt, ap);
	va_end(ap);
	if (ret < 0 || ret >= MaxBuf)
		return Error("Connection::Print - bad format or too large\n");

	return WriteShort(buffer, ret);
}

