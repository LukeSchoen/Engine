#include "Sphere.h"
#include "Maths.h"

Sphere::Sphere()
{
  position = vec3(0, 0, 0);
  radius = 1.0;
}


Sphere::Sphere(const vec3 &_position, const float &_radius /*= 1.0f*/)
{
  position = _position;
  radius = _radius;
}


bool Sphere::HitsPoint(const vec3 &pointPosition) const
{
  return (position - pointPosition).LengthSquared() < (radius * radius);
}


bool Sphere::HitsRay(const Ray &ray) const
{
  // Ray starts in sphere so definitely hits
  if (HitsPoint(ray.position)) return true;
  // Ray moves away from sphere and so doesn't hit
  vec3 lineStartToSphereCentre = position - ray.position;
  float time = ray.direction.DotProduct(lineStartToSphereCentre);
  if (time < 0) return false;  // collision is before start of ray
  vec3 closestPointAlongLine = ray.position + ray.direction * time;
  return HitsPoint(closestPointAlongLine);
}


bool Sphere::HitsSphere(const Sphere &sphere) const
{
  float sumRad = radius + sphere.radius;
  return (position - sphere.position).LengthSquared() < (sumRad * sumRad);
}


float Sphere::OnRay(const Ray &ray) const
{
  vec3 rayToSphere = position - ray.position;
  float distToCentre = rayToSphere.Length();
  //Project input ray against ray to sphere center
  float inRayOnRayToCentre = rayToSphere.DotProduct(ray.direction);
  float d = radius*radius - (distToCentre*distToCentre - inRayOnRayToCentre*inRayOnRayToCentre);
  // No intersection occurred ?, return -1
  if (d < 0.0) return (-1.0f);
  // Otherwise return distance to intersection
  return (inRayOnRayToCentre - sqrtf(d));
}
