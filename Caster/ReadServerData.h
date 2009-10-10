#ifndef ReadServerDataIncluded
#define ReadServerDataIncluded

//////////////////////////////////////////////////////////////////////////////////
//
// ReadServerData is a code fragment which communicates with an NTRIP server.
//   It is event driven, with "resume()" being called whenever a relevent event
//   occurs.
//   
////////////////////////////////////////////////////////////////////////////////////

#include "Fragment.h"
#include "MountTable.h"

class ReadServerData : public Fragment {
public:

    ReadServerData(int fd, MountPoint* mnt, int timeout=10000);
    ~ReadServerData();

    bool Resume();
    bool Abort(const char* msg);

protected:
   int fd;
   MountPoint* Mnt;
   DataBuffer& buf;
   int Timeout;
   static const int TooFarBehind = DataBuffer::BufSize / 2;

   void WakeupWaiters();

};


#endif // ReadServerDataIncluded

