#include "PolyMesh.h"


void PolyMesh::AddTriangle(const Triangle &triangle)
{
  polys.push_back(triangle);
  rootNode.triangles.push_back(polys.size()-1);
}

void PolyMesh::CompileBSP()
{
  SplitBSPNode(rootNode, 1000000);
}

vec3 PolyMesh::SlideSweepSphere(Sphere &_sphere, vec3 _velocity) const
{
  Sphere sphere = _sphere;
  vec3 velocity = _velocity;
  const int MaxSteps = 5;
  int step = 0;
  float dist = velocity.Length();
  while (dist > 0.001f)
  {
    vec3 velocityDir = velocity.Normalized();
    uint32_t polyID;
    float t = OnSweepSphere(sphere, velocityDir * dist, &polyID);
    sphere.position = sphere.position + velocity * t * 0.9;
    dist -= dist * t;
    if (t < 1.0f)
    { // Collision Response
      Triangle triangle = polys[polyID];
      Plane plane = Plane(triangle.p1, triangle.p2, triangle.p3);

      vec3 target = plane.MovePointToPlane(sphere.position + velocity);
      vec3 source = plane.MovePointToPlane(sphere.position);
      if ((target - source).Length() < 0.01) break; // too oblique

      velocity = (target - source).Normalized() * dist;

      // keep slightly away from poly for floating point accuracy problems
      sphere.position = sphere.position + plane.normal * dist * 0.1;
    }

    if (step++ > MaxSteps) break;
  }
  return sphere.position;
}

float PolyMesh::OnSweepSphere(const Sphere &sphere, const vec3 &velocity, uint32_t *firstHitID /*= nullptr*/) const
{
  float ClosestT = 1.0f;
  uint32_t polyID;
  for (uint32_t i = 0; i < polys.size(); i++)
  {
    Triangle triangle = polys[i];

    // Basic distance check
    //float longSide = Max(Max((triangle.p1 - triangle.p2).LengthSquared(), (triangle.p2 - triangle.p3).LengthSquared()), (triangle.p3 - triangle.p1).LengthSquared());
    //float closest = Max(Max((triangle.p1 - sphere.position).LengthSquared(), (triangle.p2 - sphere.position).LengthSquared()), (triangle.p3 - sphere.position).LengthSquared());
    //if (closest > longSide * 2) continue;

    float T = triangle.OnSweepSphere(sphere, velocity);
    if (T < ClosestT)
    {
      ClosestT = T;
      polyID = i;
    }
  }
  if (ClosestT < 1.0f && firstHitID) *firstHitID = polyID;
  return ClosestT;
}

float PolyMesh::OnRay(const Ray &ray, uint32_t *firstHitID) const
{
  float ClosestT = 1.0f;
  uint32_t polyID;
  for (uint32_t i = 0; i < polys.size(); i++)
  {
    const Triangle &triangle = polys[i];
    float T = triangle.OnRay(ray);
    if (T < ClosestT)
    {
      ClosestT = T;
      polyID = i;
    }
  }
  if (ClosestT < 1.0f && firstHitID) *firstHitID = polyID;
  return ClosestT;
}

void PolyMesh::SplitBSPNode(const BSPNode &node /*= rootNode*/, int maxPolys /*= 1000*/)
{
  while (node.triangles.size() > maxPolys)
  {
    int triID = node.triangles[node.triangles.size() - 1];
    Triangle &tri = polys[triID];
    tri.centrePos;
  }
}
