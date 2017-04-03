#ifndef CosmicModel_h__
#define CosmicModel_h__
#include <stdint.h>
#include "Math.h"
#include "Camera.h"
#include "StreamFile.h"
#include "RenderObject.h"

const int detailLayers = 6;
const float LodMultiplier = 0.5;
const int maxLoad = 10;

struct Layer
{
  uint32_t VertexCount;
  uint32_t DiscLocation;
};

struct Block
{
  vec3 pos;
  float size = 0.0f;
  uint32_t layerCount = 0;
  uint32_t CurrentLayer = 999;
  bool StreamedOut = false;
  bool StreamedIn = false;
  Layer layers[detailLayers];
  RenderObject model;
};

struct CosmicModel
{
  uint32_t blockCount = 0;
  Block* blocks;
  StreamFileReader *CVS = nullptr;

  int vertexBufferSize = 0;
  float *posData = nullptr;
  float *colData = nullptr;

  CosmicModel(const char *PathToCVS = nullptr);

  void LoadModel(const char *PathToCVS);
  void Render(const mat4 &MVP);
  void Stream();

private:

  void StreamFromDist(Block &block, uint32_t layer);
  void StreamToGPU(Block &block);
};

#endif // CosmicModel_h__