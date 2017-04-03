#include "Pool.h"

Pool::Pool(int poolSize)
{
  maxItems = poolSize;
  freeList = new int[maxItems];
  for (int i = 0; i < maxItems; i++) freeList[i] = i;
  freeHead = 0;
  UseCount = 0;
}

Pool::~Pool()
{
  delete[] freeList;
}

int Pool::New()
{
  UseCount++;
  return freeList[freeHead++];
}

void Pool::Old(int freeItem)
{
  UseCount--;
  freeList[--freeHead] = freeItem;
}
