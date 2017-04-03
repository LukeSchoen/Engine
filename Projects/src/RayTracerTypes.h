#ifndef RayTracerTypes_h__
#define RayTracerTypes_h__

#include "Window.h"
#include "maths.h"

vec3 ReflectRay(vec3 dir, vec3 normal);

struct Job
{
  Job(int a_jobNumber, int a_startX, int a_startY, int a_width, int a_height);
  int jobNumber;
  int startX, startY;
  int width, height;
};

struct ColoredSphere
{
  ColoredSphere(vec3 a_pos, float a_radius, uint32_t a_color = 0xFFFFFFFF);

  ColoredSphere(vec3 a_pos, float a_radius, vec3 a_color = vec3(1, 1, 1));

  bool RayOnSphere(vec3 rayPos, vec3 rayDir, float *t) const;

  vec3 m_pos;
  float m_radius;
  uint32_t m_color;
};

#endif // RayTracerTypes_h__