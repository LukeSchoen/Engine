#ifndef AlphaRenderObject_h__
#define AlphaRenderObject_h__
#include "RenderObject.h"

struct AlphaRenderObject : public RenderObject
{
  void Render(const mat4 &MVP);
};

void RenderAlphaPolygons();

#endif // AlphaRenderObject_h__
