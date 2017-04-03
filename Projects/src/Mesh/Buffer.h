#ifndef Buffer_h__
#define Buffer_h__
#include "stdint.h"

struct Buffer
{
  void Grow(int size);
  void Clear();
  uint8_t *pData = nullptr;
  Buffer();
  ~Buffer();
protected:
  int capacity = 0;
};

#endif // Buffer_h__
