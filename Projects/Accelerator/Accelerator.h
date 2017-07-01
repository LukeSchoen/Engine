#ifndef Accelerator_h__
#define Accelerator_h__

#include "stdint.h"

class Accelerator
{
public:
  Accelerator();

  int OptimalThreadCount();

  // must provide vector count in multiple of 1024
  void MatVec(const float *input, float *output, const float *matrix, int64_t num);

private:
  int block_size;
  int multiProcessors;
  int maxThreadsPerMultiProcessor;
};

#endif // Accelerator_h__
