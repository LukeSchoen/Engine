#ifndef Line_h__
#define Line_h__
#include "Maths.h"

struct Line
{
  Line(const vec3 &_start, const vec3 &_end)
  {
    start = _start;
    end = _end;
  }

  vec3 OnPoint(const vec3& point)
  {
    vec3 line = end - start;
    float len = line.Length();
    line = line / len;
    double t = line.DotProduct(point - start);
    if (t < 0.0f) return (start); // T is before line start
    if (t > len) return (end); // T is after line end
    return (start + line * t); // T is within the line segment
  }

  vec3 start, end;
};

#endif // Line_h__
