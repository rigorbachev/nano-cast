#ifndef LoginIncluded
#define LoginIncluded

#include "Fragment.h"
#include "MountTable.h"
#include "ReadServerData.h"
#include "SendClientData.h"
#include "Parse.h"

class Login : public Fragment {
public:
    Login(int fd, MountTable& mnt);
    ~Login();

    bool Abort(const char* msg = NULL);
    bool Resume();
protected:
    int Fd;
    MountTable& Mounts;
    static const int BufSize = 1024;
    byte Buf[BufSize+1];
    int Actual;

private:
    bool FrameComplete();
    bool ClientLogin(Parse& token);
    bool ServerLogin(Parse& token);
    bool WriteShort(const char *str);
    bool Close();
    bool GetHeader(Parse& token, 
                char name[MaxName+1], char passwd[MaxPassword+1]);
    bool Shutdown(const char* str);
    bool SendTable();
};

#endif // LoginIncluded

