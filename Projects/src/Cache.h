#ifndef Cache_h__
#define Cache_h__
#include "Vector.h"

struct Cache
{
  void AddItem(const char *key, void *data);
  void *GetItem(const char *key);
  void DelItem(const char *key);
  Vector *cacheData = nullptr;
};

#endif // Cache_h__
