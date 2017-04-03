#include "RayTracerTypes.h"

vec3 ReflectRay(vec3 dir, vec3 normal)
{
  return (dir - (normal * 2 * dir.DotProduct(normal))).Normalized();
}

Job::Job(int a_jobNumber, int a_startX, int a_startY, int a_width, int a_height)
{
  jobNumber = a_jobNumber;
  startX = a_startX;
  startY = a_startY;
  width = a_width;
  height = a_height;
}

ColoredSphere::ColoredSphere(vec3 a_pos, float a_radius, vec3 a_color /*= vec3(1, 1, 1)*/) : m_pos(a_pos)
, m_radius(a_radius)
{
  uint8_t r, g, b;
  b = a_color.x * 255;
  g = a_color.y * 255;
  r = a_color.z * 255;
  m_color = r | (g << 8) | (b << 16);
}

ColoredSphere::ColoredSphere(vec3 a_pos, float a_radius, uint32_t a_color /*= 0xFFFFFFFF*/) : m_pos(a_pos)
, m_radius(a_radius)
, m_color(a_color)
{
  m_color = a_color;
}

bool ColoredSphere::RayOnSphere(vec3 rayPos, vec3 rayDir, float *t) const
{
  vec3 rayToSphere = rayPos - m_pos;
  float rayCo = rayToSphere.DotProduct(rayDir);
  float c = rayToSphere.DotProduct(rayToSphere) - m_radius * m_radius;

  // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
  if (c > 0.0f && rayCo > 0.0f)
    return false;
  float discr = rayCo*rayCo - c;

  // A negative discriminant corresponds to ray missing sphere 
  if (discr < 0.0f)
    return false;

  // Ray now found to intersect sphere, compute smallest t value of intersection
  *t = -rayCo - sqrt(discr);

  // If t is negative, ray started inside sphere so clamp t to zero 
  if (*t < 0.0f) *t = 0.0f;

  return true;
}
