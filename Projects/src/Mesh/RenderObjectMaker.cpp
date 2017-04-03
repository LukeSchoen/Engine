#include "RenderObjectMaker.h"

RenderObjectMaker::RenderObjectMaker()
{
  m_pRenderObject = new RenderObject;
}

RenderObjectMaker::~RenderObjectMaker()
{
  //Clear();
}

int RenderObjectMaker::SpecifyAttribute(const char *name, GLAttributeType type, uint32_t count)
{
  GLTempAttribute attribute;
  int nameLen = strlen(name) + 1;
  attribute.varName = new char[nameLen];
  memcpy(attribute.varName, name, nameLen);
  attribute.dataCount = count;
  attribute.dataType = type;
  m_attributes.push_back(attribute);
  int attribCount = m_attributes.size() - 1;
  return attribCount;
}

void RenderObjectMaker::AddAttribute(int attributeID, const void *pAttrbuteData)
{
  int dataSize = GetAttributeSize(m_attributes[attributeID].dataType) * m_attributes[attributeID].dataCount;
  int dataPtr = m_attributes[attributeID].dataPtr;
  m_attributes[attributeID].dataPtr += dataSize;
  m_attributes[attributeID].vertexCount++;
  m_attributes[attributeID].data.Grow(dataSize + dataPtr);
  memcpy((char*)(m_attributes[attributeID].data.pData) + dataPtr, pAttrbuteData, dataSize);
}

RenderObject* RenderObjectMaker::AssembleRenderObject()
{
  int attributeCount = m_attributes.size();
  for (int aItr = 0; aItr < attributeCount; aItr++)
  {
    m_pRenderObject->AssignAttribute(m_attributes[aItr].varName, m_attributes[aItr].dataType, m_attributes[aItr].data.pData, m_attributes[aItr].dataCount, m_attributes[aItr].vertexCount);
  }
  return m_pRenderObject;
}

void RenderObjectMaker::Clear()
{
  for (int aItr = 0; aItr < m_attributes.size(); aItr++)
  {
    delete[] m_attributes[aItr].varName;
    m_attributes[aItr].data.Clear();
  }
  m_attributes.clear();
  delete m_pRenderObject;
  m_pRenderObject = new RenderObject;
}

AlphaRenderObject* AlphaRenderObjectMaker::AssembleRenderObject()
{
  return (AlphaRenderObject*)RenderObjectMaker::AssembleRenderObject();
}
