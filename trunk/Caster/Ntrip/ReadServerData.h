#ifndef ReadServerDataIncluded
#define ReadServerDataIncluded

//////////////////////////////////////////////////////////////////////////////////
//
// ReadServerData is a code fragment which communicates with an NTRIP server.
//   It is event driven, with "resume()" being called whenever a relevent event
//   occurs.
//   
////////////////////////////////////////////////////////////////////////////////////

#include "CallBack.h"
#include "MountTable.h"
#include "Connection.h"

class ReadServerData : public CallBack {
public:

    ReadServerData(Connection_ptr& c, MountPoint* mnt, int timeout=10000);
    ~ReadServerData();

    bool Call(bool status);

protected:
   Connection_ptr conn;
   MountPoint* mnt;
   DataBuffer& buf;
   int timeout;
   static const size_t TooFarBehind = DataBuffer::BufSize / 2;

   void WakeupWaiters(bool status=OK);

};


#endif // ReadServerDataIncluded

