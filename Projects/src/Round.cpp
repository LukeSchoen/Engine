#include "Round.h"
#include "stdint.h"
#include <math.h>

double FractionalPart(double a_val)
{
  int64_t realPart = (int64_t)floor(a_val);
  return a_val - realPart;
}
