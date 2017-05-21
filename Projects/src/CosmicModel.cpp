#include "CosmicModel.h"
#include "Shaders.h"
#include "Assets.h"

CosmicModel::CosmicModel(const char *PathToCVS /*= nullptr*/)
{
  if (PathToCVS) LoadModel(PathToCVS);
}

void CosmicModel::LoadModel(const char *PathToCVS)
{
  // Open File
  delete[] CVS;
  CVS = new StreamFileReader(PathToCVS);

  // Read Block Count
  CVS->ReadBytes(&blockCount, sizeof(blockCount));
  blockCount = Min(blockCount, 5000);

  // Load Block Table
  blocks = new Block[blockCount];

  // Read in block table
  for (int blockID = 0; blockID < blockCount; blockID++)
  {
    Block &block = blocks[blockID];
    CVS->ReadBytes(&block.pos, sizeof(block.pos));
    CVS->ReadBytes(&block.size, sizeof(block.size));
    CVS->ReadBytes(&block.layerCount, sizeof(block.layerCount));

    for (int Lid = 0; Lid < detailLayers; Lid++)
    {
      CVS->ReadBytes(&block.layers[Lid].VertexCount, sizeof(block.layers[Lid].VertexCount));
      CVS->ReadBytes(&block.layers[Lid].DiscLocation, sizeof(block.layers[Lid].DiscLocation));
    }
  }
}

void CosmicModel::Render(const mat4 &MVP)
{
  vec3 camPos = vec3(0) - Camera::Position();
  for (int blockID = 0; blockID < blockCount; blockID++)
  {
    Block &block = blocks[blockID];
    if (block.StreamedOut)
    {
      // Block behind camera?
      //float cullSize = (vec3(0.5f, 0.5f, 0.5f) * block.size).Length();
      //vec3 blockCentre = block.pos + block.size * 0.5;
      //vec4 blockPosView = MVP * vec4(blockCentre, 1.0);
      //if (blockPosView.z < -cullSize) continue;

      // Block outside frustum?
      //if ((blockCentre - camPos).Length() > block.size)
      {
        //float screenOff = 1.0f + (cullSize * 2.0) / blockPosView.z;
        //blockPosView = blockPosView / blockPosView.z;
        //if (blockPosView.x > screenOff || blockPosView.x < -screenOff) continue;
        //if (blockPosView.y > screenOff || blockPosView.y < -screenOff) continue;
      }


      float layerSize = 1 << blocks[blockID].CurrentLayer;
      blocks[blockID].model.AssignUniform("LAYER", UT_1f, &layerSize);
      blocks[blockID].model.RenderPoints(MVP);
    }
  }
  glFlush();
}

void CosmicModel::Stream()
{
  // Stream Data from Disc
  vec3 camPos = vec3(0)-Camera::Position();
  int curLoad = 0;
  for (int blockID = 0; blockID < blockCount; blockID++)
  {
    Block &block = blocks[blockID];
    vec3 camToBlock = camPos - (block.pos + (block.size * 0.5));
    float dist = camToBlock.Length();

    uint32_t desiredLayer = 5;

    if (dist < block.size * 64 * LodMultiplier) desiredLayer = 4;
    if (dist < block.size * 32 * LodMultiplier) desiredLayer = 3;
    if (dist < block.size * 16 * LodMultiplier) desiredLayer = 2;
    if (dist < block.size * 8 * LodMultiplier) desiredLayer = 1;
    if (dist < block.size * 4 * LodMultiplier) desiredLayer = 0;


    if (block.CurrentLayer != desiredLayer)
    {
      curLoad++;
      StreamFromDist(block, desiredLayer);
      StreamToGPU(blocks[blockID]);
      if (curLoad == maxLoad) break;
    }
  }

  // Stream Data To GPU
//   for (int blockID = 0; blockID < blockCount; blockID++)
//     if (blocks[blockID].StreamedIn)
}

void CosmicModel::StreamFromDist(Block &block, uint32_t layer)
{
  block.CurrentLayer = layer;
  uint32_t &VertexCount = block.layers[layer].VertexCount;
  uint32_t &DiscLocation = block.layers[layer].DiscLocation;
  if (vertexBufferSize < VertexCount)
  {
    posData = (float*)realloc(posData, VertexCount * 3 * sizeof(float));
    colData = (float*)realloc(colData, VertexCount * 3 * sizeof(float));
    vertexBufferSize = VertexCount;
  }
  CVS->SetLocation(DiscLocation);
  CVS->ReadBytes(posData, sizeof(float) * 3 * VertexCount);
  CVS->ReadBytes(colData, sizeof(float) * 3 * VertexCount);
  block.StreamedIn = true;
}

void CosmicModel::StreamToGPU(Block &block)
{
  block.model.Clear();
  block.model.AssignShader(ASSETDIR "Assets/Cosmic/shaders/cosmic.vert", ASSETDIR "Cosmic/shaders/cosmic.frag", ASSETDIR "Assets/Cosmic/shaders/cosmic.geom");
  block.model.AssignAttribute("position", GLAttributeType::AT_FLOAT, posData, 3, block.layers[block.CurrentLayer].VertexCount);
  block.model.AssignAttribute("color", GLAttributeType::AT_FLOAT, colData, 3, block.layers[block.CurrentLayer].VertexCount);
  block.StreamedIn = false;
  block.StreamedOut = true;
}