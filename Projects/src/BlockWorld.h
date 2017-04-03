#ifndef NewBlockWorld_h__
#define NewBlockWorld_h__

#include "RenderObject.h"
#include "Chunk.h"
#include <vector>
#include "BlockTypes.h"
#include "ChunkHandler.h"
#include <mutex>
#include "DynamicTextureArrayAtlas.h"

struct BlockWorld
{
  ChunkHandler chunks;
  BlockWorld(const char *regionDirectory, int viewDist = 12);

  std::vector<int> loadedChunks;
  std::vector<int> meshedChunks;
  MinecraftBlockTypes blockTypes;
  std::vector<RenderObject*> oldMeshes;

  bool RayTrace(vec3 _pos, vec3 _dir, float length, vec3i *FirstOccupiedBlock = nullptr, vec3i *LastEmptyBlock = nullptr);

  void SetBlock(vec3i blockPos, uint8_t blockID);
  uint8_t GetBlock(vec3i blockPos);

  void Render(mat4 &MVP);
  void Stream(vec3 viewerPos);

  DynamicTextureArrayAtlas lightingAtlas;

  std::mutex renderListLock;
private:
  int viewDist = 4;
  const char *worldFile;


  GLuint blackTexture;
  GLuint atlastexture;
};

#endif // NewBlockWorld_h__