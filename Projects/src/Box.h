#ifndef Box_h__
#define Box_h__
#include "Maths.h"

struct Box
{
  vec3 minPos;
  vec3 maxPos;

  Box();

  Box(const vec3 &lowVert, const vec3 &highVert);

  bool OnSphere(const Sphere &sphere);
};

#endif // Box_h__
