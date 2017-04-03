#include "Maths.h"

float IntersectRaySphere(vec3 rayStart, vec3 rayDir, vec3 sphereCentre, float radius)
{
  vec3 rayToSphere = sphereCentre - rayStart;
  double distToCentre = rayToSphere.Length();
  //Project input ray against ray to sphere center
  vec3 q = (rayToSphere * rayDir);
  q.Length();
  float inRayOnRayToCentre = (rayToSphere * rayDir).Length();
  float d = radius * radius - ((float)(distToCentre*distToCentre) - inRayOnRayToCentre*inRayOnRayToCentre);
  // No intersection occurred ?, return -1 
  if (d < 0.0)
    return -1.0f;
  // Otherwise return distance to the intersecting
  return inRayOnRayToCentre - sqrt(d);
}

bool RayWillHitTriangle(const vec3 V1, const vec3 V2, const vec3 V3, const vec3 RayPos, const vec3 RayDir, float *pCollisionT)
{
#define EPSILON 0.000001
  vec3 e1, e2;
  vec3 P, Q, T;
  float det, inv_det, u, v;
  float t;

  //Find vectors for two edges sharing V1
  e1 = V2 - V1;
  e2 = V3 - V1;

  //Begin calculating determinant - also used to calculate u parameter
  P = RayDir.CrossProduct(e2);

  //if determinant is near zero, ray lies in plane of triangle
  det = (e1 * P).Length();

  //NOT CULLING
  if (det > -EPSILON && det < EPSILON)
    return false;
  inv_det = 1.f / det;

  //calculate distance from V1 to ray origin
  T = RayPos - V1;

  //Calculate u parameter and test bound
  u = (T * P).Length() * inv_det;

  //The intersection lies outside of the triangle
  if (u < 0.f || u > 1.f)
    return false;

  //Prepare to test v parameter
  Q = T.CrossProduct(e1);

  //Calculate V parameter and test bound
  v = (RayDir * Q).Length() * inv_det;

  //The intersection lies outside of the triangle
  if (v < 0.f || u + v  > 1.f)
    return false;

  //Calculate Time To Intersection
  t = (e2 * Q).Length() * inv_det;

  if (t > EPSILON) //ray intersection
  {
    if (pCollisionT != nullptr)
      *pCollisionT = t;
    return true;
  }

  // No Cigar
  return false;
}

bool RayWillHitQuad(const vec3 V1, const vec3 V2, const vec3 V3, const vec3 V4, const vec3 RayPos, const vec3 RayDir, float *pCollisionT)
{
  float t = 0.0;
  bool hit = RayWillHitTriangle(V1, V2, V3, RayPos, RayDir, &t);
  if (!hit)
    hit = RayWillHitTriangle(V1, V3, V4, RayPos, RayDir, &t);
  if (hit)
  {
    if (pCollisionT != nullptr)
      *pCollisionT = t;
    return true;
  }
  return false;
}
