#ifndef BlockMesher_h__
#define BlockMesher_h__
#include "Maths.h"
#include "BlockTypes.h"

struct BlockWorld;
struct ColouredArrayTexturedRenderObjectMaker;

struct Face
{
  vec3i pos;
  uint32_t Type;
  int width = 1;
  int height = 1;
  Face(vec3i p, uint32_t t)
  {
    pos = p;
    Type = t;
  }
};

struct BlockMesher
{
  static bool GetBlockInShadow(BlockWorld *world, vec3i blockPos, int searchSize = 40);

  static uint8_t GetAONeighbourhood(BlockWorld *world, vec3i blockPos, blockSide side);

  static float ApplyAOtoVert(uint8_t neighbourhood, uint8_t vert, float occlusionFactor = 0.75f);

  static void CreateVoxelFace(BlockWorld *world, vec3i blockPos, std::vector<Face> &faceList, blockSide side);

  static void CreateFaceListMesh(BlockWorld *world, std::vector<Face> &faces, ColouredArrayTexturedRenderObjectMaker *mesh, blockSide side);
};


#endif // BlockMesher_h__
