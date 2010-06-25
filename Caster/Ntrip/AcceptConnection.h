#ifndef AcceptConnectionIncluded
#define AcceptConnectionIncluded

#include "CallBack.h"
#include "MountTable.h"
#include "Socket.h"


class AcceptConnection : public CallBack {
public:
    AcceptConnection(int port, MountTable& mnt);
    ~AcceptConnection();

    bool Call(bool status);

protected:
    Socket_ptr sock;
    MountTable& Mnt;
    int Port;
};




#endif // AcceptConnectionIncluded
