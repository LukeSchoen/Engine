#ifndef ChunkInstance_h__
#define ChunkInstance_h__
#include "Chunk.h"
#include "RenderObject.h"

struct ChunkInstance
{
  bool hasBlocks = false;
  int xPos, zPos;
  Chunk *chunk = nullptr;
  RenderObject *solidMesh = nullptr;
  RenderObject *alphaMesh = nullptr;
  RenderObject *texelMesh = nullptr;
  bool hasChanged = false;
  float lifeTime = 0.0f;
  void DeleteMesh()
  {
    if (solidMesh) delete solidMesh; solidMesh = nullptr;
    if (alphaMesh) delete alphaMesh; alphaMesh = nullptr;
    if (texelMesh) delete texelMesh; texelMesh = nullptr;
  }

  void DeleteMemory()
  {
    if (chunk) delete[] chunk; chunk = nullptr;
  }
};

#endif // ChunkInstance_h__
