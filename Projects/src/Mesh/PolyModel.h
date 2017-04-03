#ifndef PolyModel_h__
#define PolyModel_h__

#include "RenderObject.h"

struct PolyModel
{
public:
  PolyModel::PolyModel();
  bool LoadModel(const char *fileName, bool forceReload = false);
  void GetModelExtents(vec3 *pMinimum, vec3 *pMaximum);
  void GetMeshData(int64_t *pMeshCount, RenderObject **ppMeshes);
  bool IntersectRay(const vec3 &rayPos, const vec3 &rayDir, float *pDistance, const mat4 &modelMatrix, const vec3 &modelPivot);
  void SetBrightness(float brightness);
  void Save(char *filePath);
  bool Load(char *filePath);
  void DownSample(int64_t maxPolys = 1000);
  void BakeColors(bool accurate = false);
  void Render(const mat4 &MVP);
  void Destroy();

private:
  RenderObject *pMeshes;
  uint32_t meshCount = 0;
  vec3 minExtents;
  vec3 maxExtents;
};

#endif // PolyModel_h__
