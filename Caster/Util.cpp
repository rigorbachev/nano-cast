#include "Util.h"
#include <errno.h>
#include <stdio.h>

bool Error() {return !OK;}
bool Error(const char* fmt, ...)
    {printf("Error: fmt=%s\n", fmt); return Error();}  // TODO: use varargs to format message properly

bool SysError(const char* fmt, ...)
    {printf("SysError: fmt=%s errno=%d\n", fmt, errno); return Error();}
