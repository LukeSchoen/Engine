#include "Box.h"

Box::Box()
{
  minPos = vec3();
  maxPos = vec3();
}

Box::Box(const vec3 &lowVert, const vec3 &highVert)
{
  minPos = lowVert;
  maxPos = highVert;
}

bool Box::OnSphere(const Sphere &sphere)
{
  vec3 hit = vec3(Min(sphere.position.x, maxPos.x), Min(sphere.position.y, maxPos.y), Min(sphere.position.z, maxPos.z));
  hit = vec3(Max(hit.x, minPos.x), Max(hit.y, minPos.y), Max(hit.z, minPos.z));
  return ((hit - sphere.position).LengthSquared() < (sphere.radius*sphere.radius));
}
