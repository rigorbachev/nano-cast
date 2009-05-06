#ifndef FragListIncluded
#define FragListIncluded

// A list of code fragments, usually for waiting on some event


#include "BaseFragment.h"


class FragList {
protected:
    BaseFragment* head;

public:
    inline FragList() {head = NULL;}
    inline ~FragList()
    {
        while (Head() != NULL) 
            delete Pop();
    }

    inline void Add(BaseFragment* f) 
    {
        f->link = head;  
        head = f;
    }

    inline BaseFragment* Pop()   
    {
        BaseFragment* f=head; 
        if (f!=NULL) 
            head=f->link; 
        return f;
    }

    inline BaseFragment* Head()  
       {return head;}
};
    

#endif //FragListIncluded

