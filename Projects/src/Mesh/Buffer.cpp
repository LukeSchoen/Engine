#include "Buffer.h"
#include "Maths.h"
#include <stdlib.h>

void Buffer::Grow(int size)
{
  if (size > capacity)
  {
    while (size > capacity)
      capacity = Max(capacity, 1) * 2;
    pData = (uint8_t*)realloc(pData, capacity);
  }
}

void Buffer::Clear()
{
  if(capacity) free(pData);
  pData = nullptr;
  capacity = 0;
}

Buffer::Buffer()
{

}

Buffer::~Buffer()
{
  Clear();
}
