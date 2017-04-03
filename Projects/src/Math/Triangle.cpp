#include "Triangle.h"
#include "Maths.h"

Triangle::Triangle(const vec3 &v1, const vec3 &v2, const vec3 &v3)
{
  p1 = v1;
  p2 = v2;
  p3 = v3;
  normal = ((v2 - v1).CrossProduct(v3 - v1)).Normalized();

  // Bounding Sphere
  centrePos = (p1 + p2 + p3) * 0.333333;
  float d1 = (centrePos - p1).Length();
  float d2 = (centrePos - p2).Length();
  float d3 = (centrePos - p3).Length();
  sqrBoundSize = Max(Max(d1, d2), d3);
  sqrBoundSize *= sqrBoundSize;
}

const vec3 *Triangle::getVert() const
{
  return &p1;
}

bool Triangle::PointInTriangle(vec3 point) const
{
  double total_angles = 0.0f;
  vec3 v1 = (point - p1).Normalized();
  vec3 v2 = (point - p2).Normalized();
  vec3 v3 = (point - p3).Normalized();
  total_angles += acos(v1.DotProduct(v2));
  total_angles += acos(v2.DotProduct(v3));
  total_angles += acos(v3.DotProduct(v1));
  if (fabs(total_angles - 2 * PI) <= 0.005) // add to 180 degrees?
    return true;
  return false;
}

float Triangle::OnSweepSphere(const Sphere &sphere, const vec3 &velocity) const
{
  // broad phase check (dodgy)
  //if ((sphere.position - centrePos).LengthSquared() > (sqrBoundSize + velocity.Length())) return 2;

  // Front face check
  if (velocity.DotProduct(normal) > 0) return 2;

  // Sphere Hits Plane (Test 1) //
  // Move sphere center velocity Ray
  // towards Triangle by one and
  // Project it onto H-plane to check
  // if it falls withing the Triangle.
  vec3 newSpherePos = sphere.position - (normal * sphere.radius);
  float t = Ray(newSpherePos, velocity).OnPlane(Plane(p1, normal));
  vec3 planeHitSpot = newSpherePos + velocity * t;
  if (t >= 0 && t < 1.0) if (PointInTriangle(planeHitSpot)) return t;
  if (t > 1.0) return 2; // too far to collide at all

  // Sphere Hits Edges (Test 2) //
  // Project sphere center onto
  // H-plane then move to lines
  // and project ray back along
  // velocity and check against
  // the sphere at start pos
  float closestEdge = 1000000000;
  for (int edge = 0; edge < 3; edge++)
  {
    float edgeDist = sphere.OnRay(Ray(Line(getVert()[edge], getVert()[(edge + 1) % 3]).OnPoint(planeHitSpot), vec3() - velocity.Normalized()));
    if (edgeDist > 0 && edgeDist < closestEdge) closestEdge = edgeDist;
  }
  if (closestEdge > 0 && closestEdge < 1000000000)
  {
    t = closestEdge / velocity.Length();
    if (t >= 0 && t < 1.0)
      return t;
  }
  return 2;
}

float Triangle::OnRay(const Ray &ray) const
{
  // Front face check
  //if (velocity.DotProduct(normal) > 0) return 2;
  float t = ray.OnPlane(Plane(p1, normal));
  vec3 planeHitSpot = ray.position + ray.direction * t;
  if (t >= 0 && t < 1.0) if (PointInTriangle(planeHitSpot)) return t;
  return 2; // too far to collide at all
}

float Triangle::Area()
{
  float ab = (p2 - p1).Length();
  float bc = (p3 - p2).Length();
  float ac = (p3 - p1).Length();
  float s = (ab + bc + ac) * 0.5f;
  s = Max(0.f, s * (s - ab) * (s - bc) * (s - ac));
  return sqrt(s);
}
