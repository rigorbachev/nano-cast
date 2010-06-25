#ifndef SendSourceTableIncluded
#define SendSourceTableIncluded

#include "CallBack.h"
#include "MountTable.h"
#include "Connection.h"

class SendSourceTable : public CallBack {
public:

     SendSourceTable(Connection_ptr& c, MountTable& m);
     ~SendSourceTable();

     bool Call(bool status);

protected:
    MountTable& mounts;
    Connection_ptr conn;

    const byte* Buf;
    size_t Len;
    size_t Actual;

    size_t Next;

    char Header[200];  // Buffer to build header in. 
};
  

#endif // SendSourceTableIncluded
