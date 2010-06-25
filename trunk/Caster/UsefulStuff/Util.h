#ifndef UtilIncluded
#define UtilIncluded

#define debug printf
#define vdebug vprintf

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

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


bool Event(const char *fmt, ...);
bool Error(const char *fmt, ...);
bool SysError(const char *fmt, ...);
bool Verror(const char* fmt, va_list args);
inline bool Error() {return true;}
void ClearError();
int ShowErrors();
static const bool OK = false;


// Cygwin only
template <class T> const T& min (const T& a, const T&b) {return (a<b)?a:b;}
template <class T> const T& max (const T& a, const T&b) {return (a>b)?a:b;}

#endif // UtilIncluded
