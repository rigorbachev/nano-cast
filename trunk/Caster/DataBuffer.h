#ifndef DataBufferIncluded
#define DataBufferIncluded

#include "Util.h"
#include "FragList.h"

class DataBuffer {
public:
    DataBuffer() {Init();}
    ~DataBuffer() {}

    void Init() {count = 0;}

    static const int BufSize = 8192;  // power of two desired
    byte Buffer[BufSize];

    int64 count;
    FragList Waiters;

};

#endif // DataBufferIncluded
