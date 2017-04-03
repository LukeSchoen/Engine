#ifndef Pool_h__
#define Pool_h__

struct Pool
{
  Pool(int poolSize = 65536);
  ~Pool();
  int New();
  void Old(int freeItem);
  int *freeList;
  int freeHead;
  int maxItems;
  int UseCount;
};


#endif // Pool_h__
