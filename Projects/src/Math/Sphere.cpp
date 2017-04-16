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
  vec3 rayToSphere = ray.position - position;
  float rayCo = rayToSphere.DotProduct(ray.direction);
  float c = rayToSphere.DotProduct(rayToSphere) - radius * radius;

  // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
  if (c > 0.0f && rayCo > 0.0f)
    return -1;
  float discr = rayCo*rayCo - c;

  // A negative discriminant corresponds to ray missing sphere 
  if (discr < 0.0f)
    return -1;

  // Ray now found to intersect sphere, compute smallest t value of intersection
  float ret = -rayCo - sqrt(discr);

  // If t is negative, ray started inside sphere so clamp t to zero 
  if (ret < 0.0f) ret = 0.0f;

  return ret;
}
