#ifndef SendClientDataIncluded
#define SendClientDataIncluded

#include "Fragment.h"
#include "MountTable.h"

class SendClientData : public Fragment {
public:

    SendClientData(int fd, MountPoint* mnt);
    ~SendClientData();

    bool Resume();
    bool Abort(const char* msg);

    using Fragment::Write;
    bool Write(int size, int& actual);

protected:
   int fd;
   MountPoint* Mnt;
   DataBuffer& buf;
   int count;
   static const int TooFarBehind = DataBuffer::BufSize / 2;

private:
    bool WaitForBuf();

};


#endif // SendClientDataIncluded

