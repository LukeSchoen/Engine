#ifndef NovaCosmModel_h__
#define NovaCosmModel_h__
#include "StreamFile.h"
#include "RenderObject.h"
#include "Camera.h"
#include <stdlib.h>
#include "Controls.h"
#include "Assets.h"

struct voxel 
{
  uint8_t x, y, z, r, g, b;
};

struct NovaCosmBlock
{
  int64_t discLocation;
  int64_t children[8];
  vec3 position;
  NovaCosmBlock *childPtr[8];
  int64_t voxelCount;
  void *voxelPosData;
  void *voxelColData;
  RenderObject model;
};

struct NovaCosmModel
{
  NovaCosmBlock *root;

  StreamFileReader *fileStream;

  NovaCosmModel(const char *NCSfile) { Load(NCSfile); }

  ~NovaCosmModel() {}

  void Load(const char *NCSfile)
  {
    fileStream = new StreamFileReader(NCSfile); // Open File Stream
    root = LoadBlock(0); // Load Root Block
  }

  void Render(const mat4 &MVP)
  {
    RecursiveRender(root, MVP, 0);
  }

  void Stream()
  {
    //vec3 camPos = vec3(0) - Camera::Position();
    // Recurse Scene Blocks
    // Check if we're close to the camera
    // If so stream in child blocks
    RecursiveStream(root);
  }

  void AddPoint(vec3 pos, uint32_t col)
  {
    RecursiveAddPoint(root, 0, pos, col);
  }

private:
  void RecursiveAddPoint(NovaCosmBlock *block, int layer, vec3 pos, uint32_t col)
  {
    vec3 localPos = (pos * (1LL << layer)) - block->position;
    block->voxelCount++;
    block->voxelPosData = realloc(block->voxelPosData, block->voxelCount * 3);
    block->voxelColData = realloc(block->voxelColData, block->voxelCount * 3);
    ((uint8_t*)block->voxelPosData)[(block->voxelCount - 1) * 3 + 0] = round(localPos.x);
    ((uint8_t*)block->voxelPosData)[(block->voxelCount - 1) * 3 + 1] = round(localPos.y);
    ((uint8_t*)block->voxelPosData)[(block->voxelCount - 1) * 3 + 2] = round(localPos.z);

    ((uint8_t*)block->voxelColData)[(block->voxelCount - 1) * 3 + 0] = col & 0xFF;
    ((uint8_t*)block->voxelColData)[(block->voxelCount - 1) * 3 + 1] = (col >> 8) & 0xFF;
    ((uint8_t*)block->voxelColData)[(block->voxelCount - 1) * 3 + 2] = (col >> 16) & 0xFF;

    block->model.AssignAttribute("position", GLAttributeType::AT_UNSIGNED_BYTE, block->voxelPosData, 3, block->voxelCount);
    block->model.AssignAttribute("color", GLAttributeType::AT_UNSIGNED_BYTE_NORM, block->voxelColData, 3, block->voxelCount);

    bool midx = localPos.x >= 128;
    bool midy = localPos.y >= 128;
    bool midz = localPos.z >= 128;
    uint8_t child = midx + midy * 2 + midz * 4;
    if (block->childPtr[child])
      RecursiveAddPoint(block->childPtr[child], layer + 1, pos, col);
  }

  void RecursiveStream(NovaCosmBlock *block)
  {
    for (uint8_t cItr = 0; cItr < 8; cItr++)
      if (block->childPtr[cItr])
        RecursiveStream(block->childPtr[cItr]);
      else
      {
        int64_t childAddress = block->children[cItr];
        if (childAddress > 0)
          block->childPtr[cItr] = LoadBlock(childAddress);
      }
  }

  void RecursiveRender(NovaCosmBlock *block, const mat4 &MVP, int layer)
  {
    float layerSize = (1.0 / (1LL << layer));
    //layerSize = 1.0;
    vec3 blockPos = (block->position + vec3(128, 128, 128)) * layerSize;
    vec3 camPos = vec3(0) - Camera::Position();
    float dist = (camPos - blockPos).Length();

    bool leaf = true;
    // Close enough to split ?
    if (dist < 256 * layerSize)
    //if (false)
    {
      for (uint8_t cItr = 0; cItr < 8; cItr++)
        if (block->childPtr[cItr])
        {
          leaf = false;
          RecursiveRender(block->childPtr[cItr], MVP, layer + 1);
        }
        else // Stream in data
        {
          int64_t childAddress = block->children[cItr];
          if (childAddress > 0)
            block->childPtr[cItr] = LoadBlock(childAddress);
        }


    }
    if (leaf)
    {
      static float SWAY = 0;
      SWAY = SWAY + 0.00025;

      static float swayAmount = 0;
      if (Controls::KeyDown(SDL_SCANCODE_SPACE))
        swayAmount = Min(swayAmount + 0.002, 1);
      else
        swayAmount = Max(swayAmount - 0.002, 0);

      block->model.AssignUniform("swayAmount", UT_1f, &swayAmount);
      block->model.AssignUniform("SWAY", UT_1f, &SWAY);
      block->model.AssignUniform("LAYER", UT_1f, &layerSize);
      block->model.AssignUniform("regionPos", UT_3f, block->position.Data());
      block->model.RenderPoints(MVP);
    }
  }

  NovaCosmBlock *LoadBlock(int64_t discLocation)
  {
    // CPU
    NovaCosmBlock *block = new NovaCosmBlock;
    block->discLocation = discLocation;
    fileStream->SetLocation(discLocation);
    memset(&block->childPtr, 0, sizeof(block->childPtr));
    fileStream->ReadBytes(block->children, sizeof(block->children));
    fileStream->ReadBytes(&block->position, sizeof(block->position));
    fileStream->ReadBytes(&block->voxelCount, sizeof(block->voxelCount));
    int64_t voxelDataSize = block->voxelCount * sizeof(voxel);

    // Untwist voxel data, yuk!
    uint8_t *data = (uint8_t*)malloc(voxelDataSize);
    fileStream->ReadBytes(data, voxelDataSize);
    block->voxelPosData = malloc(block->voxelCount * 3);
    block->voxelColData = malloc(block->voxelCount * 3);
    for (int64_t vItr = 0; vItr < block->voxelCount; vItr++)
    {
      for (int c = 0; c < 3; c++) ((uint8_t*)block->voxelPosData)[vItr * 3 + c] = data[vItr * 6 + c]; // Positions
      for (int c = 0; c < 3; c++) ((uint8_t*)block->voxelColData)[+vItr * 3 + c] = data[vItr * 6 + c + 3]; // Colors
    }
    free(data);

    // GPU
    block->model.AssignShader(ASSETDIR "NovaCosm/shaders/NovaCosm.vert", ASSETDIR "NovaCosm/shaders/NovaCosm.frag", ASSETDIR "NovaCosm/shaders/NovaCosm.geom");
    block->model.AssignAttribute("position", GLAttributeType::AT_UNSIGNED_BYTE, block->voxelPosData, 3, block->voxelCount);
    block->model.AssignAttribute("color", GLAttributeType::AT_UNSIGNED_BYTE_NORM, block->voxelColData, 3, block->voxelCount);

    return block;
  }

  void UnloadBlock(NovaCosmBlock *block)
  {
    free(block->voxelPosData);
    delete[] block;
  }
};

#endif // NovaCosmModel_h__
