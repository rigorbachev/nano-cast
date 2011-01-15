/******************************************************************************
 *                                                                             *
 *        Code from Item 29 ("Reference Counting") of MORE EFFECTIVE C++       *
 *                                                                             *
 *                               Scott Meyers                                  *   
 *                                                                             *
 *            Copyright 1996 (c) Addison-Wesley Publishing Company             *
 *       You are free to use this code for non-commercial purposes only.       *
 *                                                                             *
 * This page contains the code for the classes and class templates making up   *
 * the Reference Counting Item of More Effective C++.  To use this code,       *
 * either copy this page and paste it into a C++ source file or save the       *
 * entire page as text into a C++ source file.  Don't save the HTML source     *
 * and expect that to compile :-)                                              *
 *                                                                             *
 * Each class or template in this file follows a block comment that shows the  *
 * corresponding pages in the book.  This page also contains a main function   *
 * that performs a VERY limited test of the code in this file.  You can        *
 * compile the code in this file as a stand-alone program, and you should get  *
 * this output:                                                                *
 *                                                                             *
 *     String with no changes.                                                 *
 *     String with    changes.                                                 *
 *     10                                                                      *
 *     -1                                                                      *
 *                                                                             *
 * The code here reflects all changes made to date in response to bug reports  *
 * from readers of the book.  (To see a complete list of known bugs in More    *
 * Effective C++, as well as whether they've been fixed yet, visit the         *
 * More Effective C++ Errata List.)  If you find any additional bugs, please   *
 * send them to me.                                                            *
 ******************************************************************************/
 
 /******************************************************************************
 *                       Class RCObject (from pp. 204-205)                     *
 ******************************************************************************/
 class RCObject {                       // base class for reference-
 public:                                // counted objects
   void addReference();
   void removeReference();
   void markUnshareable();
   bool isShareable() const;
   bool isShared() const;
 
 protected:
   RCObject();
   RCObject(const RCObject& rhs);
   RCObject& operator=(const RCObject& rhs);
   virtual ~RCObject() = 0;
 
 private:
   int refCount;
   bool shareable;
 };
 
 RCObject::RCObject()
 : refCount(0), shareable(true) {}
 
 RCObject::RCObject(const RCObject&)
 : refCount(0), shareable(true) {}
 
 RCObject& RCObject::operator=(const RCObject&)
 {
   return *this;
 }  
 
 RCObject::~RCObject() {}
 
 void RCObject::addReference() 
 {
   ++refCount;
 }
 
 void RCObject::removeReference()
 {
   if (--refCount == 0) delete this;
 }
 
 void RCObject::markUnshareable()
 {
   shareable = false;
 }
 
 bool RCObject::isShareable() const
 {
   return shareable;
 }
 
 bool RCObject::isShared() const
 {
   return refCount > 1;
 }  
 
 
 /******************************************************************************
 *                 Template Class RCPtr (from pp. 203, 206)                    *
 ******************************************************************************/
 template<class T>                      // template class for smart
 class RCPtr {                          // pointers-to-T objects; T
 public:                                // must support the RCObject interface
   RCPtr(T* realPtr = 0);
   RCPtr(const RCPtr& rhs);
   ~RCPtr();
   RCPtr& operator=(const RCPtr& rhs);                     
   T* operator->() const;
   T& operator*() const;
 
 private:
   T *pointee;
   void init();
 };
 
 template<class T>
 void RCPtr<T>::init()
 {
   if (pointee == 0) return;
   
   if (pointee->isShareable() == false) {
     pointee = new T(*pointee);
   }
   
   pointee->addReference();
 }
 
 template<class T>
 RCPtr<T>::RCPtr(T* realPtr)
 : pointee(realPtr)
 {
   init();
 }
 
 template<class T>
 RCPtr<T>::RCPtr(const RCPtr& rhs)
 : pointee(rhs.pointee)
 { 
   init();
 }
 
 template<class T>
 RCPtr<T>::~RCPtr()
 {
   if (pointee) pointee->removeReference();
 }
 
 template<class T>
 RCPtr<T>& RCPtr<T>::operator=(const RCPtr& rhs)
 {
   if (pointee != rhs.pointee) {                   // this code was modified
     T *oldPointee = pointee;                      // for the book's 10th
                                                   // printing
     pointee = rhs.pointee;
     init(); 
 
     if (oldPointee) oldPointee->removeReference();                
   }
   
   return *this;
 }
 
 template<class T>
 T* RCPtr<T>::operator->() const 
 {
   return pointee;
 }
 
 template<class T>
 T& RCPtr<T>::operator*() const
 {
   return *pointee;
 }
 
#endif
