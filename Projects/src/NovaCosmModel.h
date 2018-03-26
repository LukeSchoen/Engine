#ifndef NovaCosmModel_h__
#define NovaCosmModel_h__
#include "StreamFile.h"
#include "RenderObject.h"
#include "Camera.h"
#include <stdlib.h>
#include "Controls.h"
#include "Assets.h"
#include "Shaders.h"
#include "MeanShiftSeg.h"

struct voxel 
{
  uint8_t x, y, z, r, g, b;
};

struct NovaCosmBlock
{
  bool Ready = false;
  bool Updated = false;
  int64_t discLocation;
  int64_t children[8];
  vec3 position;
  NovaCosmBlock *childPtr[8];
  int64_t voxelCount;
  void *voxelPosData;
  void *voxelColData;
  void *voxelSegmentData;
  RenderObject voxels;
  RenderObject segments;
  RenderObject regionLines;

};

struct NovaCosmModel
{
  float qMult = 512.f;
  bool segmentation = false;

  NovaCosmBlock *root;

  StreamFileReader *fileStream;

  NovaCosmModel(const char *NCSfile)
  {
    glEnable(GL_PROGRAM_POINT_SIZE);
    glLineWidth(2);
    Load(NCSfile); 
  }

  ~NovaCosmModel() {}

  void Load(const char *NCSfile)
  {
    fileStream = new StreamFileReader(NCSfile); // Open File Stream
    root = LoadBlock(); // Load Root Block
  }

  void Render(const mat4 &MVP, bool debug = false)
  {
    RecursiveRender(root, MVP, 0, debug);
  }

  void Stream()
  {
    RecursiveStream(root);
  }

  void AddPoint(vec3 pos, uint32_t col)
  {
    RecursiveAddPoint(root, 0, pos, col);
  }

  void ExportPCF(const char *filePath)
  {
    StreamFileWriter stream(filePath);
    RecursiveExportPCF(&stream, root);
  }

  NovaCosmBlock *GetCamBlock(vec3 pos, int *layer)
  {
    if (segmentation)
      return RecursiveGetCamBlock(root, 0, pos, layer);
    else
      return nullptr;
  }

private:
  NovaCosmBlock *RecursiveGetCamBlock(NovaCosmBlock *block, int layer, vec3 pos, int *outLayer)
  {
    float layerSize = (1.0 / (1LL << layer));
    vec3 blockPos = (block->position + vec3(128, 128, 128)) * layerSize;
    vec3 camPos = vec3(0) - Camera::Position();
    float dist = Max(Max(fabs(camPos.x - blockPos.x), fabs(camPos.y - blockPos.y)), fabs(camPos.z - blockPos.z));

    // Close enough to split ?
    if (dist < qMult * (layerSize * 2))
    {
      vec3 localPos = (pos * (1LL << layer)) - block->position;
      bool midx = localPos.x >= 128;
      bool midy = localPos.y >= 128;
      bool midz = localPos.z >= 128;
      uint8_t child = midx + midy * 2 + midz * 4;
      if (block->childPtr[child] && block->childPtr[child]->Ready)
        return RecursiveGetCamBlock(block->childPtr[child], layer + 1, pos, outLayer);
    }
    *outLayer = layer;
    return block;
  }

  void RecursiveExportPCF(StreamFileWriter *stream, NovaCosmBlock *block)
  {
    bool leaf = true;
    for (uint8_t cItr = 0; cItr < 8; cItr++)
    {
      int64_t childAddress = block->children[cItr];
      if (childAddress > 0)
      {
        if (!block->childPtr[cItr])
          block->childPtr[cItr] = LoadBlock(childAddress);
        RecursiveExportPCF(stream, block->childPtr[cItr]);
        //UnloadBlock(block->childPtr[cItr]);
        leaf = false;
      }
    }
    if (leaf)
      for (int64_t vItr = 0; vItr < block->voxelCount; vItr++)
      {
        int32_t x = ((uint8_t*)block->voxelPosData)[vItr * 3 + 0] + block->position.x;
        int32_t y = ((uint8_t*)block->voxelPosData)[vItr * 3 + 1] + block->position.y;
        int32_t z = ((uint8_t*)block->voxelPosData)[vItr * 3 + 2] + block->position.z;
        int32_t r = ((uint8_t*)block->voxelColData)[vItr * 3 + 0];
        int32_t g = ((uint8_t*)block->voxelColData)[vItr * 3 + 1];
        int32_t b = ((uint8_t*)block->voxelColData)[vItr * 3 + 2];
        uint32_t c = b | (g << 8) | (r << 16);
        stream->WriteBytes(&x, sizeof(x));
        stream->WriteBytes(&y, sizeof(y));
        stream->WriteBytes(&z, sizeof(z));
        stream->WriteBytes(&c, sizeof(c));
      }
  }

  void RecursiveAddPoint(NovaCosmBlock *block, int layer, vec3 pos, uint32_t col)
  {
    vec3 localPos = (pos * (1LL << layer)) - block->position;
    block->voxelCount++;
    block->voxelPosData = realloc(block->voxelPosData, block->voxelCount * 3);
    block->voxelColData = realloc(block->voxelColData, block->voxelCount * 3);
    ((uint8_t*)block->voxelPosData)[(block->voxelCount - 1) * 3 + 0] = Min(round(localPos.x), 255);
    ((uint8_t*)block->voxelPosData)[(block->voxelCount - 1) * 3 + 1] = Min(round(localPos.y), 255);
    ((uint8_t*)block->voxelPosData)[(block->voxelCount - 1) * 3 + 2] = Min(round(localPos.z), 255);

    ((uint8_t*)block->voxelColData)[(block->voxelCount - 1) * 3 + 0] = col & 0xFF;
    ((uint8_t*)block->voxelColData)[(block->voxelCount - 1) * 3 + 1] = (col >> 8) & 0xFF;
    ((uint8_t*)block->voxelColData)[(block->voxelCount - 1) * 3 + 2] = (col >> 16) & 0xFF;

    block->Updated = true;
    

    bool midx = localPos.x >= 128;
    bool midy = localPos.y >= 128;
    bool midz = localPos.z >= 128;
    uint8_t child = midx + midy * 2 + midz * 4;
    if (block->childPtr[child])
      RecursiveAddPoint(block->childPtr[child], layer + 1, pos, col);
  }

  bool RecursiveStream(NovaCosmBlock *block, int layer = 0)
  {
    float layerSize = (1.0 / (1LL << layer));
    vec3 blockPos = (block->position + vec3(128, 128, 128)) * layerSize;
    vec3 camPos = vec3(0) - Camera::Position();
    float dist = Max(Max(fabs(camPos.x - blockPos.x), fabs(camPos.y - blockPos.y)), fabs(camPos.z - blockPos.z));

    // Close enough to split ?
    if (dist < qMult * layerSize)
    {
      // Create children
      for (uint8_t cItr = 0; cItr < 8; cItr++)
      {
        if (!block->childPtr[cItr])
        {
          int64_t childAddress = block->children[cItr];
          if (childAddress > 0)
          {
            block->childPtr[cItr] = LoadBlock(childAddress, layer + 1);
            return true;
          }
        }
      }

      // Descend into children
      for (uint8_t cItr = 0; cItr < 8; cItr++)
      {
        if (block->childPtr[cItr])
        {
          if (RecursiveStream(block->childPtr[cItr], layer + 1))
            return true;
        }
      }
    }
    return false;
  }

  void RecursiveRender(NovaCosmBlock *block, const mat4 &MVP, int layer, bool debug)
  {
    if (!block) return;
    float layerSize = (1.0 / (1LL << layer));
    //layerSize = 1.0;
    vec3 blockPos = (block->position + vec3(128, 128, 128)) * layerSize;
    vec3 camPos = vec3(0) - Camera::Position();
    //float dist = (camPos - blockPos).Length();
    float dist = Max(Max(fabs(camPos.x - blockPos.x), fabs(camPos.y - blockPos.y)), fabs(camPos.z - blockPos.z));

    bool leaf = true;
    // Close enough to split ?
    if (dist < qMult * layerSize)
    {
      bool kidsAllReady = true;
      //renderLock.lock();
      for (uint8_t cItr = 0; cItr < 8; cItr++)
        if (block->children[cItr])
          if (!block->childPtr[cItr] || !block->childPtr[cItr]->Ready)
            kidsAllReady = false;
      if(kidsAllReady)
        for (uint8_t cItr = 0; cItr < 8; cItr++)
          if (block->childPtr[cItr])
          {
            leaf = false;
            RecursiveRender(block->childPtr[cItr], MVP, layer + 1, debug);
          }
    }
    if (leaf)
    {
      mat4 TMVP = MVP;
      TMVP.Transpose();
      vec4 sp = TMVP * vec4(blockPos.x, blockPos.y, blockPos.z, 1);

      float s = 256.f / (1LL << layer);
      if (dist < s || (sp.z > 0 && abs(sp.x) < sp.z + s && abs(sp.y * 0.7) < sp.z + s))
      {
        if (block->Updated)
        {
          block->voxels.AssignAttribute("position", GLAttributeType::AT_UNSIGNED_BYTE, block->voxelPosData, 3, block->voxelCount);
          block->voxels.AssignAttribute("color", GLAttributeType::AT_UNSIGNED_BYTE_NORM, block->voxelColData, 3, block->voxelCount);
          block->Updated = false;
        }

        float pointsize = 1.0 - (debug * 0.8);
        block->voxels.AssignUniform("LAYER", UT_1f, &layerSize);
        block->voxels.AssignUniform("POINTSIZE", UT_1f, &pointsize);
        block->voxels.AssignUniform("regionPos", UT_3f, block->position.Data());

        block->segments.AssignUniform("LAYER", UT_1f, &layerSize);
        block->segments.AssignUniform("POINTSIZE", UT_1f, &pointsize);
        block->segments.AssignUniform("regionPos", UT_3f, block->position.Data());

        if(Controls::KeyDown(SDL_SCANCODE_1))
          block->segments.RenderPoints(MVP);
        else
          block->voxels.RenderPoints(MVP);

        if(debug)
          block->regionLines.RenderLines(MVP);
      }
    }
  }

  NovaCosmBlock *LoadBlock(int64_t discLocation = 0, int layer = 0)
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
    block->voxelSegmentData = malloc(block->voxelCount * 3);
    for (int64_t vItr = 0; vItr < block->voxelCount; vItr++)
    {
      for (int c = 0; c < 3; c++) ((uint8_t*)block->voxelPosData)[vItr * 3 + c] = data[vItr * 6 + c]; // Positions
      for (int c = 0; c < 3; c++) ((uint8_t*)block->voxelColData)[+vItr * 3 + c] = data[vItr * 6 + c + 3]; // Colors
    }
    free(data);

    // GPU
    // Voxels
    static unsigned int shaderID = Shaders::Load(ASSETDIR "NovaCosm/shaders/NovaCosm.vert", ASSETDIR "NovaCosm/shaders/NovaCosm.frag");
    block->voxels.AssignShader(shaderID);
    block->voxels.AssignAttribute("position", AT_UNSIGNED_BYTE, block->voxelPosData, 3, block->voxelCount);
    block->voxels.AssignAttribute("color", AT_UNSIGNED_BYTE_NORM, block->voxelColData, 3, block->voxelCount);

    if (segmentation)
    {
      // Generate Segments
      uint32_t *pColorMap = new uint32_t[256 * 256];
      float *pDepthMap = new float[256 * 256];
      memset(pColorMap, 0, 256 * 256 * sizeof(uint32_t));
      memset(pDepthMap, 0, 256 * 256 * sizeof(float));
      for (int64_t vItr = 0; vItr < block->voxelCount; vItr++)
      {
        auto xyz = &((uint8_t*)block->voxelPosData)[vItr * 3];
        auto rgb = &((uint8_t*)block->voxelColData)[vItr * 3];
        if (xyz[1] >= pDepthMap[xyz[0] + xyz[2] * 256])
        {
          pDepthMap[xyz[0] + xyz[2] * 256] = xyz[1];
          pColorMap[xyz[0] + xyz[2] * 256] = rgb[0] | rgb[1] << 8 | rgb[2] << 16;
        }
      }
      MeanShiftSeg::ApplySegmentation(pColorMap, 256, 256, pColorMap, 7, 6.5, 1, true);
      for (int64_t vItr = 0; vItr < block->voxelCount; vItr++)
      {
        auto xyz = &((uint8_t*)block->voxelPosData)[vItr * 3];
        auto rgb = &((uint8_t*)block->voxelSegmentData)[vItr * 3];
        rgb[0] = (pColorMap[xyz[0] + xyz[2] * 256]) & 255;
        rgb[1] = (pColorMap[xyz[0] + xyz[2] * 256] >> 8) & 255;
        rgb[2] = (pColorMap[xyz[0] + xyz[2] * 256] >> 16) & 255;
      }
      delete[] pColorMap;
      delete[] pDepthMap;

      // Upload Segments
      block->segments.AssignShader(shaderID);
      block->segments.AssignAttribute("position", AT_UNSIGNED_BYTE, block->voxelPosData, 3, block->voxelCount);
      block->segments.AssignAttribute("color", AT_UNSIGNED_BYTE_NORM, block->voxelSegmentData, 3, block->voxelCount);
    }

    // Lines
    block->regionLines.AssignShader(SHADERDIR "ColouredRenderObject.vert", SHADERDIR "ColouredRenderObject.frag");

    float layerSize = (1.0 / (1LL << layer));
    std::vector<vec3> linePosData;
    std::vector<vec3> lineColData;
    // X lines
    linePosData.push_back((block->position + vec3(0, 0, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 0, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(0, 0, 256)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 0, 256)) * layerSize);
    linePosData.push_back((block->position + vec3(0, 256, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 256, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(0, 256, 256)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 256, 256)) * layerSize);
    lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0));  lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0));

    // Y lines
    linePosData.push_back((block->position + vec3(0, 0, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(0, 256, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(0, 0, 256)) * layerSize);
    linePosData.push_back((block->position + vec3(0, 256, 256)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 0, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 256, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 0, 256)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 256, 256)) * layerSize);
    lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0));  lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0));

    // Z Lines
    linePosData.push_back((block->position + vec3(0, 0, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(0, 0, 256)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 0, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 0, 256)) * layerSize);
    linePosData.push_back((block->position + vec3(0, 256, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(0, 256, 256)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 256, 0)) * layerSize);
    linePosData.push_back((block->position + vec3(256, 256, 256)) * layerSize);
    lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0));  lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0)); lineColData.push_back(vec3(0, 1, 0));

    block->regionLines.AssignAttribute("position0", AT_FLOAT, linePosData.data(), 3, linePosData.size());
    block->regionLines.AssignAttribute("colour0", AT_FLOAT, lineColData.data(), 3, lineColData.size());

    block->Ready = true;

    return block;
  }

  void UnloadBlock(NovaCosmBlock *block)
  {
    free(block->voxelPosData);
    free(block->voxelColData);
    free(block->voxelSegmentData);
    delete block;
  }
};

#endif // NovaCosmModel_h__
