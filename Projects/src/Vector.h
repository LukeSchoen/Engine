#ifndef Vector_h__
#define Vector_h__
#include "Buffer.h"

struct Vector
{
  Vector(int ItemSize);
  void Add(void *item);
  void *Get(int index);
  void Del(int index);
  void Clear();
  void *Data();
  int count = 0;
private:
  Buffer listData;
  int itemSize = 0;
  int capacity = 0;
};

#endif // Vector_h__
