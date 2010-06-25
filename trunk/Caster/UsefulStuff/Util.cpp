#include "Util.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

/////////////////////////////////////////////////////////////
// Error handling
//
// Error() adds the message to an internal buffer.
// ClearError() clears the error messages
// ShowError()  displays the error messages
//
////////////////////////////////////////////////////////////////


int ErrCount = 0;
static const int ErrMax = 15;
static const int ErrMaxStr = 256;
char   ErrSlot[ErrMax][ErrMaxStr];  // Make this thread specific later


const char * GetLastError()
{
    return strerror(errno);
}

bool SysError(const char* fmt, ...)
{
	Error("System error %s\n", GetLastError());

	va_list arglist;
	va_start(arglist, fmt);
	Verror(fmt, arglist);
	va_end(arglist);
    return true;
}




bool Error(const char *fmt, ...)
{
	if (fmt == NULL) return !OK;
	va_list arglist;
	va_start(arglist, fmt);
	Verror(fmt, arglist);
	va_end(arglist);
	return !OK;
}

bool Verror(const char *fmt, va_list arglist)
{
	debug("ERROR: "); vdebug(fmt, arglist);

	// Format into the slot
	vsnprintf(ErrSlot[ErrCount], ErrMaxStr-1, fmt, arglist);
	ErrSlot[ErrCount][ErrMaxStr-1] = '\0';

	// if we have more room in the error list, then allocate a slot
	if (ErrCount < ErrMax)
	    ErrCount++;

	return true;
}


void ClearError()
{
	debug("CLEAR errors\n");
	ErrCount = 0;
}


int ShowErrors()
{
	for (int i=0; i<ErrCount; i++)
		printf("%5d: %s", i, ErrSlot[i]);

/***********************************
	if (ErrCount > 0) {
		printf("\nHit ""enter"" to continue.\n");
		while (getchar() != '\n')
			;
	}
*******************************/

        if (ErrCount > 0) return 1;
        else              return 0;

}


