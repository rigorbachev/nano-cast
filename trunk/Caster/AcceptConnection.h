#ifndef AcceptConnectionIncluded
#define AcceptConnectionIncluded

#include "Fragment.h"
#include "MountTable.h"



class AcceptConnection : public Fragment {
public:
    AcceptConnection(int port, MountTable& mnt);
    ~AcceptConnection();

    bool Resume();
    bool Abort(const char* msg);

protected:
    int fd;
    MountTable& Mnt;
    int Port;
};




#endif // AcceptConnectionIncluded
