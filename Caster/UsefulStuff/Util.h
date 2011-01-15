#ifndef UtilIncluded
#define UtilIncluded

#define debug printf
#define vdebug vprintf

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define CASTER_SW "NtripCaster_1.0Beta(precision-gps.org)"

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned long intp;
typedef uint8 byte;


typedef bool Status;
static const Status OK = false;
Status Event(const char *fmt, ...);
Status Error(const char *fmt, ...);
Status SysError(const char *fmt, ...);
Status Verror(const char* fmt, va_list args);
inline bool Error() {return !OK;}
void ClearError();
int ShowErrors();

// Cygwin only
template <class T> const T& min (const T& a, const T&b) {return (a<b)?a:b;}
template <class T> const T& max (const T& a, const T&b) {return (a>b)?a:b;}

#endif // UtilIncluded
