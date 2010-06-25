#include "MountTable.h"
#include "Parse.h"
#include <string.h>

#include <stdio.h>
static bool ReadLine(FILE* f, byte* buf, int size);

MountPoint::MountPoint()
{
    Init("");
}

MountPoint::~MountPoint()
{
}

bool MountPoint::ValidPassword(const char *password)
////////////////////////////////////////////////////////////////////////////
// ValidPassword is true if the password is valid for this mountpoint
////////////////////////////////////////////////////////////////////////////
{
    return (Password[0] == '\0' || strcmp(Password, password) == 0);
}


bool MountPoint::IsMounted()
///////////////////////////////////////////////////////////////////////////////
// IsMounted is true if the mountpoint has a server attached.
///////////////////////////////////////////////////////////////////////////////
{
    return State != UNMOUNTED;
}

bool MountPoint::Mount() 
{
    if (IsMounted())
        return Error("Mountpoint %s is already mounted\n", Name);

    Buf.Init();
    State = MOUNTED;
    return OK;
}




bool MountPoint::Unmount()
{
    State = UNMOUNTED;
    Buf.waiters.WakeupWaiters();

    return OK;
}


bool MountPoint::Init(const char* name)
{
    if (strlen(name) > MaxName)
        return Error("MountPoint: name too long\n");

    strcpy(Name, name);
    strcpy(Password, "");
    strcpy(STR, "");

    State = UNMOUNTED;
    return OK;
}


bool MountPoint::SetPassword(const char* password)
{
    if (strlen(password) > MaxPassword)
        return Error("MountPoint: password too long\n");
    strcpy(Password, password);
    return OK;
}



bool MountPoint::SetSTR(const char* str)
{
    if (strlen(str) > MaxSTR)
        return Error("MountPoint: STR entry too long\n");
    strcpy(STR, str);
    return OK;
}



MountTable::MountTable() {}
MountTable::~MountTable(){}

MountPoint* MountTable::CreateMount(const char* name)
////////////////////////////////////////////////////////////////////////////////////
// CreateMount creates a new mount point in the mount table.
//   typically done during initialization
////////////////////////////////////////////////////////////////////////////////////
{
   // Make sure the mountpoint name is valid
   if (name[0] == '\0')
        {Error("Create: Can't create null mount point\n"); return NULL;}
   if (Lookup(name) != NULL)
        {Error("Create: Mount Point %s lready Defined\n", name); return NULL;}

    // Find an empty slot in the mount table
    MountPoint* mnt = Lookup("");
    if (mnt == NULL)
        {Error("Create: Too many mountpoints (%s)\n", name); return NULL;}

    // Use the slot
    if (mnt->Init(name) != OK)
        {Error("CreateMount: invalid name %s\n", name); return NULL;}

    return mnt;
}



bool MountTable::DestroyMount(const char* name)
{
    MountPoint* mnt = Lookup(name);
    if (mnt == NULL)
        return Error("Can't destroy - mount point %s doesn't exist\n", name);

    mnt->Init("");
    return OK;
}             
    



MountPoint* MountTable::Lookup(const char* name)
///////////////////////////////////////////////////////////////////////////
// Lookup finds the mountpoint in the mount table
//////////////////////////////////////////////////////////////////////
{
    for (int i=0; i<MaxMounts; i++)
        if (strcmp(Mounts[i].Name, name) == 0)
            return &Mounts[i];
    return NULL;
}



bool MountTable::ReadFromFile(const char* path)
////////////////////////////////////////////////////////////////////////
// ReadFromFile reads the mount points from the configuration file
//   and fills in the mount table
/////////////////////////////////////////////////////////////////////////
{
    // Open the file
    //   TODO: If there are errors, we will abort, although we really should close the file
    FILE *f = fopen(path, "r");
    if (f == NULL) return SysError("Can't open source table %s\n", path);

    // Read it a line at a time.
    // Each line is of the form:   mountpoint; password; STR data
    while (!feof(f)) {
        byte line[MaxSTR+MaxName+MaxPassword+25]; // TODO: get it out of stack
        if (ReadLine(f, line, sizeof(line)) != OK)
            return SysError("Trouble reading from source table file %s\n", path);

       // Get the mount point name. 
       Parse token(line, strlen((char*)line));
       char name[MaxName+1];
       token.Next(";");
       if (token == "") return Error("Empty mount point not valid\n");
       token.GetToken(name, sizeof(name));

       // Get the password
       char password[MaxPassword+1];
       token.Next(";");
       token.GetToken(password, sizeof(password));

       // Get the Stream Record entry (STR) - all entries up to end of line.
       char str[MaxSTR+1];
       token.Next("\r\n");
       token.GetToken(str, sizeof(str));

       // Create a new entry with the password and STR data
       MountPoint* mnt = CreateMount(name);
       if (mnt == NULL || mnt->SetPassword(password) != OK | mnt->SetSTR(str) != OK)
           return Error("Problem creating new mountpoint %s\n", name);
    }

    fclose(f);
    return OK;
}



static bool ReadLine(FILE *f, byte* buf, int size)
{
     int i;
     for (i=0; i+2< size; i++) {

        int c; 
        buf[i] = c = getc(f);
        if (c == EOF || buf[i] == '\n')  break;

        // Ignore "CR" since we don't know if file was edited by windows or unix
        if (buf[i] == '\r') i--;
    }
    if (i+2 >= size) return Error("ReadLine: line longer than %d characters\n", size-3);

    // Line is terminated with /r /n /0
    buf[i] = '\r';
    buf[i+1] = '\n';
    buf[i+2] = '\0';

    return OK;
}
      

    
   
