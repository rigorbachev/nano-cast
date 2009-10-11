#include "Parse.h"
#include "string.h"


Parse::Parse(const byte* buf, int len)
    : Buf(buf), Len(len), Delimiter(-1)
{
}

Parse::Parse(const char* str)
: Buf((byte*)str), Len(strlen(str)), Delimiter(-1)
{
}

Parse::~Parse()
{
}

bool Parse::operator!=(const char* str)
{
    bool equal = (*this == str);
    debug("Parse::operator!=  equal=%d\n", equal);
    return !equal;
}


bool Parse::operator==(const char* str)
{
    debug("Parse == (%s)\n", str);
    int i;
    for (i=TokenBegin; i<=TokenEnd; i++)
        if (Buf[i] != *str++)
            return false;
    return *str == '\0';
}

byte Parse::GetDelimiter()
{
    if (Delimiter < 0 || Delimiter >= Len)
        return 0;

    return Buf[Delimiter];
}



bool Parse::GetToken(char* token, int size)
{
    debug("GetToken: begin=%d  end=%d  size=%d\n", TokenBegin, TokenEnd, size);
    if (TokenEnd-TokenBegin > size) return !OK;

    for (int i=TokenBegin; i<=TokenEnd; i++)
        token[i-TokenBegin] = Buf[i];
    token[TokenEnd - TokenBegin + 1] = '\0';

    debug("Parse::GetToken  %s\n", token);

    return OK;
}



bool Parse::Next(const char* delimiters)
{
    // If we are at the end of the buffer, then done
    if (Delimiter >= Len)  return !OK;

    // The previous delimiter is not part of the token
    TokenBegin = Delimiter + 1;

    // Scan until we encounter the next delimiter
    for (Delimiter++; Delimiter < Len; Delimiter++)
        if (strchr(delimiters, Buf[Delimiter]) != NULL)
            break; 
    
    // The trailing delimiter is not part of the token
    TokenEnd = Delimiter - 1;

    // If the delimiter is a blank, then skip additional blanks
    if (Buf[Delimiter] == ' ')
        while (Delimiter+1 < Len && Buf[Delimiter+1] == ' ')
            Delimiter++;

    // If the delimiter is CR/LF, then skip to newline
    if (Delimiter+1 < Len && Buf[Delimiter] == '\r' 
        && Buf[Delimiter+1] == '\n' && strchr(delimiters, '\n') != NULL)
        Delimiter++;


    // Strip trailing blanks from token
    for (; TokenEnd >= TokenBegin; TokenEnd--)
        if (Buf[TokenEnd] != ' ')
            break;

    // Strip leading blanks from token
    for (; TokenEnd >= TokenBegin; TokenBegin++)
        if (Buf[TokenBegin] != ' ')
            break;

    debug("Parse::Next  Delimiter=%d(%c)  Token=", Delimiter, Buf[Delimiter]);
    for (int i=TokenBegin; i<=TokenEnd; i++)
        debug("%c", Buf[i]);
    debug("\n");

    return OK;
}
    

    
    
