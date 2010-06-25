#ifndef MountPointIncluded
#define MountPointIncluded

////////////////////////////////////////////////////////////////////////////////////
//
// The Mount Table is a set of mount points, each of which represents a stream of ntrip data
//
// The table is initialzed from a file, typically "Caster.mounts"
//   which contains the mount point, server password, and STR data
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include "DataBuffer.h"

static const int MaxName = 32;
static const int MaxPassword = 32;
static const int MaxSTR = 256;
static const int MOUNTED = 1;
static const int UNMOUNTED = 0;

class MountPoint {
public:
    MountPoint();
    ~MountPoint();

    bool Mount();
    bool Unmount();

    bool Init(const char* name);
    bool SetPassword(const char* password);
    bool SetSTR(const char* srt);

    bool IsMounted();
    bool ValidPassword(const char* password);

public:
    char Name[MaxName+1];
    char Password[MaxPassword+1];
    char STR[MaxSTR+1];

    int State;
    DataBuffer Buf;
};


class MountTable {
public:
    MountTable();
    ~MountTable();

    MountPoint* CreateMount(const char* name);
    bool DestroyMount(const char* name);

    MountPoint* Lookup(const char* name);

    bool ReadFromFile(const char* path);

    inline MountPoint& operator[](int i) {return Mounts[i];}


    const static int MaxMounts = 32;
    MountPoint Mounts[MaxMounts];
};

#endif // MountPointIncluded
