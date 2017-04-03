#ifndef PolyMesh_h__
#define PolyMesh_h__
#include "Maths.h"
#include <vector>

struct BSPNode
{
  std::vector<int> triangles;
  BSPNode *children[8] = {nullptr};
};

struct PolyMesh
{
  void AddTriangle(const Triangle &triangle);

  void CompileBSP();

  vec3 SlideSweepSphere(Sphere &sphere, vec3 velocity) const;

  float OnSweepSphere(const Sphere &sphere, const vec3 &velocity, uint32_t *firstHitID = nullptr) const;

  float OnRay(const Ray &ray, uint32_t *firstHitID = nullptr) const;

private:

  std::vector<Triangle> polys;
  BSPNode rootNode;
  void SplitBSPNode(const BSPNode &node, int maxPolys);
};

#endif // PolyMesh_h__
