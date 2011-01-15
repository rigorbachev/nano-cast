#ifndef LoginIncluded
#define LoginIncluded

#include "CallBack.h"
#include "MountTable.h"
#include "Connection.h"
#include "ReadServerData.h"
#include "SendClientData.h"
#include "Parse.h"

class Login : public CallBack {
public:
    Login(Connection_ptr& c, MountTable& m);
    ~Login();

protected:
    Connection_ptr conn;
    MountTable& mounts;
    static const int BufSize = 1024;
    byte buf[BufSize+1];
    int actual;

private:
    bool FrameComplete();
    bool ClientLogin(Parse& token);
    bool ServerLogin(Parse& token);
    bool WriteShort(const char *str);
    bool GetHeader(Parse& token, 
                char name[MaxName+1], char passwd[MaxPassword+1]);
    bool Shutdown(const char* str);
    bool SendTable();
};

#endif // LoginIncluded

