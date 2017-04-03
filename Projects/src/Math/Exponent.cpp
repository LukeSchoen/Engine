#include "Exponent.h"
#include "Maths.h"

float Delinerize(float value, float exponent)
{
  float mag = fabs(value);
  if (mag == 0.0f) return 0;
  float sign = 1.0f;
  if (value < 0) sign = -1;
  return pow(mag, exponent) * sign;
}

vec2 Delinerize(vec2 value, float exponent)
{
  if (value.Length() == 0) return vec2();
  return value / value.Length() * Delinerize(value.Length(), exponent);
}
