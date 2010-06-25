/////////////////////////////////////////////////////////////////////////////////
//
// Caster is the main program for the NTRIP caster.
//   The mount points are defined in the "Caster.mounts" file
//   where each entry has the form:
//        mountpoint; password; STR data
//
//   For each mount point, an NTRIP server can log in and start
//   providing data.
//
//   Once an NTRIP server attaches to a mount point, 
//   multiple clients can attach and receive the data.
//
//   There is no interpretation of the data. The Caster
//   simply passes on the bytes from the server to the clients.
//
///////////////////////////////////////////////////////////////////////
#include "Dispatcher.h"
#include "Poll.h"
#include "DataBuffer.h"
#include "AcceptConnection.h"
#include <stdio.h>

bool Configure(int argc, char** argv);

// Caster configuration
int Port = 2101;
int NrStreams = 1024;
const char *MountFile = "Caster.mounts";
int DebugLevel = 0;


int main(int argc, char** argv)
{
    setlinebuf(stdout);
    Configure(argc, argv);

    // Configure the dispatcher to handle a bunch of connections
    if (Dispatcher::Init() != OK || Poll::Init() !=OK)
        return SysError("Can't configure Dispatcher\n");

    // Read in the mount points from the "Caster.mounts" configuration file
    MountTable mounts;
    if (mounts.ReadFromFile(MountFile) != OK)
        return Error("Can't read mount points from %s\n", "Caster.mounts");

    // Start the dispatcher with a handler to accept incoming connections
    new AcceptConnection(Port, mounts);
    Dispatcher::Run();
    
    return OK;
}


bool Configure(int argc, char** argv)
{


    return OK;
}
