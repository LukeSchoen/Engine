#ifndef RenderObjectMaker_h__
#define RenderObjectMaker_h__
#include "RenderObject.h"
#include "AlphaRenderObject.h"
#include "Buffer.h"
#include <vector>

struct GLTempAttribute
{
  int dataCount;
  char *varName;
  int dataPtr = 0;
  int vertexCount = 0;
  Buffer data;
  GLAttributeType dataType;
};

class RenderObjectMaker
{
public:
  // Returns Compiled Render Object, Caller is responsible for memory
  RenderObject* AssembleRenderObject();

  // Clears Stored memory and prepares to create new render object
  void Clear();

protected:

  RenderObjectMaker();

  ~RenderObjectMaker();

  int SpecifyAttribute(const char *aName, GLAttributeType aType, uint32_t aCount);

  void AddAttribute(int attributeID, const void *pAttrbuteData);

  RenderObject *m_pRenderObject;

  std::vector<GLTempAttribute> m_attributes;
};

struct AlphaRenderObjectMaker : RenderObjectMaker
{
  AlphaRenderObject* AssembleRenderObject();
};

#endif // RenderObjectMaker_h__
