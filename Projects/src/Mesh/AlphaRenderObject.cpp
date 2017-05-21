#include "AlphaRenderObject.h"
#include "GL\glew.h"
#include <vector>

struct RenderInstance
{
  RenderObject *pRenderObject;
  Matrix4x4 mvp;
};

struct FaceInstance
{
  FaceInstance()
  {
    faceID = 0;
    distance = 0.0f;
    pRenderInstance = nullptr;
  }
  FaceInstance(int _faceID, float _distance, RenderInstance* _pRenderInstance)
  {
    faceID = _faceID;
    distance = _distance;
    pRenderInstance = _pRenderInstance;
  }
  int faceID;
  float distance;
  RenderInstance* pRenderInstance;
};

static int CompareFaces(const void* pA, const void* pB)
{
  uint16_t A = ((FaceInstance*)pA)->distance;
  uint16_t B = ((FaceInstance*)pB)->distance;
  if (A < B) return -1;
  if (A == B) return 0;
  if (A > B) return 1;
}

static std::vector<RenderInstance> renderInstances;
static std::vector<FaceInstance> faceInstances;

struct SortedRenderer
{
  static void AddRender(RenderObject *pRenderObject, const Matrix4x4 &MVP)
  {
    RenderInstance renderInstance;
    renderInstance.pRenderObject = pRenderObject;
    renderInstance.mvp = MVP;
    renderInstances.push_back(renderInstance);
  }
  static void Render()
  {
    glDepthMask(false);
    faceInstances.clear();
    for (uint32_t roItr = 0; roItr < renderInstances.size(); ++roItr)
    {
      vec3 *pVertices;
      int64_t vertexCount;
      renderInstances[roItr].pRenderObject->AccessAttribute("position0", &vertexCount, nullptr, nullptr, (const void**)&pVertices);
      for (uint32_t faceItr = 0; faceItr < vertexCount; faceItr += 3)
      {
        vec3 p1 = pVertices[faceItr + 0];
        vec3 p2 = pVertices[faceItr + 1];
        vec3 p3 = pVertices[faceItr + 2];
        vec3 r((p1 + p2 + p3) / 3.0f);
        vec4 midPoint = vec4(r.x, r.y, r.z, 1.0f);
        midPoint = Matrix4x4(renderInstances[roItr].mvp) * midPoint;
        faceInstances.push_back(FaceInstance(faceItr, midPoint.z, &(renderInstances[roItr])));
      }
      qsort(faceInstances.data(), faceInstances.size(), sizeof(FaceInstance), CompareFaces);
    }
    for (uint32_t faceItr = 0; faceItr < faceInstances.size(); ++faceItr)
    {
      int firstVertex = faceInstances[faceItr].faceID;
      faceInstances[faceItr].pRenderInstance->pRenderObject->Render(faceInstances[faceItr].pRenderInstance->mvp, firstVertex, 3);
    }
    renderInstances.clear();
    glDepthMask(true);
  }
};

void AlphaRenderObject::Render(const mat4 &MVP)
{
  SortedRenderer::AddRender(this, MVP);
}

void RenderAlphaPolygons()
{
  SortedRenderer::Render();
}