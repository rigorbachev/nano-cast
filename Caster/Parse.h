#ifndef ParseIncluded
#define ParseIncluded

//////////////////////////////////////////////////////////////////////////
//
// Parse is a simple parser which extracts tokens from a line string
//
////////////////////////////////////////////////////////////////////////////
#include "Util.h"


class Parse {
public:
    Parse(const byte* buf, int len);
    ~Parse();

    bool Next(const char* delimiters);

    bool operator==(const char* str);
    bool GetToken(char *str, int len);
    byte GetDelimiter();

protected:
    const byte* Buf;
    int Len;
    int TokenBegin;
    int TokenEnd;
    int Delimiter;
};




#endif // ParseIncluded
