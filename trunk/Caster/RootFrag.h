#ifndef RootFragIncluded
#define RootFragIncluded


#include "ListenerFrag.h"
#include "ClientFrag.h"

class RootFrag : public Fragment {
public:
    RootFrag(int f, Databuffer b)
        : serverlisten(f, b), clientlisten(f, b) {}

    bool Resume() {serverlisten.Resume(); clientlisten.Resume(); return OK;}

protected:
    ListenerFrag serverlisten;
    ClientListener clientlisten;
}
    
