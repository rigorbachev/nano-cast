#ifndef SendSourceTableIncluded
#define SendSourceTableIncluded

#include "Fragment.h"
#include "MountTable.h"

class SendSourceTable : public Fragment {
public:

     SendSourceTable(int fd, MountTable& mounts);
     ~SendSourceTable();

     bool Resume();
     bool Abort(const char* str = NULL);
     bool Close();

protected:
    MountTable& Mounts;
    int Fd;

    const byte* Buf;
    int Len;
    int Actual;

    int Next;

    char Header[200];  // Buffer to build header in. 
};
  

#endif // SendSourceTableIncluded
