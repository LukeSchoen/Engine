#ifndef NewBlockWorld_h__
#define NewBlockWorld_h__

#include <vector>
#include <mutex>
#include "Chunk.h"
#include "BlockTypes.h"
#include "RenderObject.h"
#include "ChunkHandler.h"
#include "EntityHandler.h"
#include "DynamicTextureArrayAtlas.h"

struct BlockWorld
{
  ChunkHandler chunks;
  EntityHandler entities;
  BlockWorld(const char *regionDirectory, int viewDist = 12);

  std::vector<int> loadedChunks;
  std::vector<int> meshedChunks;
  MinecraftBlockTypes blockTypes;
  std::vector<RenderObject*> oldMeshes;

  bool RayTrace(vec3 _pos, vec3 _dir, float length, vec3i *FirstOccupiedBlock = nullptr, vec3i *LastEmptyBlock = nullptr);

  bool BoxCollides(vec3 _pos, vec3 _size);
  bool BoxContains(vec3 _pos, vec3 _size, uint8_t blockID);

  void SetBlock(vec3i blockPos, uint8_t blockID);
  uint8_t GetBlock(vec3i blockPos);

  void Render(mat4 &VP);
  void Stream(vec3 viewerPos);

  DynamicTextureArrayAtlas lightingAtlas;

  std::mutex renderListLock;

  bool combineFaces = false;
private:
  int viewDist = 4;
  const char *worldFile;

  GLuint blackTexture;
  GLuint atlastexture;
};

#endif // NewBlockWorld_h__