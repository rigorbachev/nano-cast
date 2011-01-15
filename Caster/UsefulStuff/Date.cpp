/*
 * Date.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: John
 */

#include "Date.h"
#include <time.h>

void getDateString(char *str)
{
	// Get the time from the Linux operating system
	//  (we assume system clock is GMT)
	time_t clock = time(NULL);

	// RFC 1123:  Sun, 06 Nov 1994 08:49:37 GMT
	struct tm brokenDown; gmtime_r(&clock, &brokenDown)
	srftime(str, 30, "%a, %d %b %Y %T GMT", &brokenDown);
}
