#ifndef SendClientDataIncluded
#define SendClientDataIncluded

#include "CallBack.h"
#include "MountTable.h"
#include "Connection.h"

class SendClientData : public CallBack {
public:

    SendClientData(Connection_ptr& c, MountPoint* mnt);
    ~SendClientData();

    bool Call(bool status);

protected:
   Connection_ptr conn;
   MountPoint* Mnt;
   DataBuffer& buf;
   uint64 count;  // total bytes sent on this connection.
   static const size_t TooFarBehind = DataBuffer::BufSize / 2;

private:
   bool WaitForBuf();

};


#endif // SendClientDataIncluded

