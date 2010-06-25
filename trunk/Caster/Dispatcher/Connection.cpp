/*
 * Connection.cpp
 *
 *  Created on: Jun 21, 2010
 *      Author: John
 */

#include <unistd.h>
#include "Connection.h"

Connection::Connection(Handle_t h)
: Pollable(h)
{}
