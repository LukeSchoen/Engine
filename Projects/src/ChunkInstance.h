#ifndef ChunkInstance_h__
#define ChunkInstance_h__
#include "Chunk.h"
#include "RenderObject.h"

struct ChunkInstance
{
  bool hasBlocks = false;
  int xPos, zPos;
  Chunk *chunk = nullptr;
  RenderObject *mesh = nullptr;
  bool hasChanged = false;
  float lifeTime = 0.0f;
  void DeleteMesh()
  {
    if (mesh) delete mesh; mesh = nullptr;
  }

  void DeleteMemory()
  {
    if (chunk) delete[] chunk; chunk = nullptr;
  }
};

#endif // ChunkInstance_h__
