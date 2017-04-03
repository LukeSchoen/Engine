#include "Vector.h"
#include "maths.h"
#include <string.h>

Vector::Vector(int ItemSize)
{
  itemSize = ItemSize;
}

void Vector::Add(void *item)
{
  if (count == capacity)
  {
    capacity = Max(capacity * 2, 1);
    listData.Grow(capacity * itemSize);
  }
  memcpy(listData.pData + itemSize * count, item, itemSize);
  count++;
}

void * Vector::Get(int index)
{
  if ((index < 0) | (index >= count))
    return nullptr;
  return listData.pData + itemSize * index;
}

void Vector::Del(int index)
{
  if ((index < 0) | (index >= count))
    return;
  memcpy(listData.pData + itemSize * index, listData.pData + itemSize * (index+1), itemSize * ((count-1) - index));
  count--;
}

void Vector::Clear()
{
  count = 0;
  capacity = 0;
  listData.Clear();
}

void * Vector::Data()
{
  return listData.pData;
}
