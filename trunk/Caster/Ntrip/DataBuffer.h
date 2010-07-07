#ifndef DataBufferIncluded
#define DataBufferIncluded

#include "Util.h"
#include "CallBack.h"


class DataBuffer {
public:
    DataBuffer() {Init();}
    ~DataBuffer() {}

    void Init() {count = 0;}

    static const size_t BufSize = 8192;  // power of two desired
    byte Buffer[BufSize];

    uint64 count;

    WaitList waiters;
};

#endif // DataBufferIncluded
