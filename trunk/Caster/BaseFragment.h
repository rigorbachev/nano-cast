#ifndef BaseFragmentIncluded
#define BaseFragmentIncluded

#include "Util.h"  // for NULL


class BaseFragment {
public:
    BaseFragment* link;   // Make available to FragList only

public:
    BaseFragment() {}
    virtual ~BaseFragment(){}
    virtual bool Resume() = 0;
    virtual bool Abort(const char* msg) = 0;
};

#endif // BaseFragmentIncluded

