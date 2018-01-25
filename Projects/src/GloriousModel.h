#ifndef GloriousModel_h__
#define GloriousModel_h__
#include "StreamFile.h"
#include "RenderObject.h"
#include "Camera.h"
#include <stdlib.h>
#include "Controls.h"
#include "CustomRenderObjects.h"
#include "DynamicTextureArrayAtlas.h"
#include "BlockMesher.h"
#include "FaceOptimizer.h"
#include "Assets.h"
#include "SortedList.h"
#include "ImageFile.h"
#include <string>

struct voxel
{
  uint8_t x, y, z, r, g, b;
};

struct GloriousBlock
{
  bool Ready = false;
  int64_t discLocation;
  int64_t children[8];
  vec3 position;
  GloriousBlock *childPtr[8];
  int64_t voxelCount;
  void *voxelPosData;
  void *voxelColData;
  //RenderObject model;
  RenderObject *Smodel;
  std::vector<int> tiles;
  int faceCount = 0;
};

struct GloriousModel
{
  int facesDrawn = 0;

  int64_t quality = 320;

  GloriousBlock *root;

  StreamFileReader *fileStream;

  GloriousModel(const char *NCSfile)
    : atlas(512, 512, 512)
  {
    atlas.UploadToGPU();
    Load(NCSfile);
  }

  ~GloriousModel() {}

  DynamicTextureArrayAtlas atlas;

  Pool atlasFreeTiles;

  std::vector<std::tuple<GloriousBlock*, int64_t>> streamOptions;

  void Load(const char *NCSfile)
  {
    fileStream = new StreamFileReader(NCSfile); // Open File Stream
    root = LoadBlock(0); // Load Root Block
  }

  void Render(const mat4 &MVP)
  {
    RecursiveRender(root, MVP, 0);
    //printf("\nfaces drawn %d\n\n", facesDrawn * 60);
    facesDrawn = 0;
  }

  void Stream(const mat4 &MVP)
  {
    if (atlas.layers - atlasFreeTiles.UseCount < 5)
    {
      Sleep(1);
      return;
    }
    streamOptions.clear();
    RecursiveStream(root, MVP, 0);
    // Pick Best Block
    if (!streamOptions.size()) return;
    float bestScore = 99999999999;
    GloriousBlock *bestPtr = nullptr;
    for (auto & block : streamOptions)
    {
      auto blockPtr = std::get<0>(block);
      auto blockScore = std::get<1>(block);
      if (blockScore < bestScore)
      {
        bestScore = blockScore;
        bestPtr = blockPtr;
      }
    }
    if (!bestPtr) return;
    for (uint8_t cItr = 0; cItr < 8; cItr++)
    {
      if (!bestPtr->childPtr[cItr])
      {
        int64_t childAddress = bestPtr->children[cItr];
        if (childAddress > 0)
        {
          bestPtr->childPtr[cItr] = LoadBlock(childAddress);
          return;
        }
      }
    }

  }

  void GenGMesh(const char *NCSfile, const char *gmshFile)
  {
    fileStream = new StreamFileReader(NCSfile); // Open File Stream
    auto stream = new StreamFileWriter(gmshFile); // Open File Stream
    RecursiveGmesh(stream, root);
  }

private:

  void RecursiveGmesh(StreamFileWriter *stream, GloriousBlock *block)
  {
    // Load Block
    block = LoadBlock(0);

    //mat4 MVP;
    //atlas.UploadToGPU();
    //block->Smodel->UploadToGPU();
    //block->Smodel->Render(MVP);

    int64_t discLocation = stream->GetLocation();

    //write node
    stream->WriteBytes(block->children, sizeof(block->children)); // Dummy children data ( will be over written )
    stream->WriteBytes(&block->position, sizeof(block->position)); // Real position data

    char *pMesh;
    int pMeshLen;
    block->Smodel->Serialize(&pMesh, &pMeshLen);
    stream->WriteBytes(pMesh, pMeshLen);
    delete[] pMesh;

    int64_t textureCount = block->tiles.size();
    stream->WriteBytes(&textureCount, sizeof(int64_t));

    //glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, GL_UNSIGNED_INT_8_8_8_8, texture);
    for (auto & tile : block->tiles)
    {
      stream->WriteBytes(atlas.image + tile * atlas.width * atlas.height, atlas.width * atlas.height * sizeof(uint32_t));
      //ImageFile::WriteImagePNG("C:/temp/output/layer.png", atlas.image + tile * atlas.width * atlas.height, atlas.width, atlas.height);
      //atlas.image;
    }
    //static char buffer[256] = { 0 };
    //for (int64_t i = 0; i < atlas.layers; i++)
    //{
    //  sprintf(buffer, "C:/temp/layer%d.png", i);
    //  stream->WriteBytes(atlas.image + i * atlas.width * atlas.height, atlas.width * atlas.height * sizeof(uint32_t));
    //  ImageFile::WriteImagePNG(buffer, atlas.image + (i * atlas.width * atlas.height), atlas.width, atlas.height);
    //}

    atlas;

    // Unload block
    UnloadBlock(block);

    //write child nodes
    for (int8_t i = 0; i < 8; i++)
      if (block->children[i])
      {
        int64_t head = stream->GetLocation();
        stream->SetLocation(discLocation + i * sizeof(int64_t));
        stream->WriteBytes(&head, sizeof(head));
        stream->SetLocation(head);
        RecursiveGmesh(stream, LoadBlock(block->children[i]));
      }

  }

  bool RecursiveStream(GloriousBlock *block, const mat4 &MVP, int layer)
  {
    float layerSize = (1.0 / (1LL << layer));
    vec3 blockPos = (block->position + vec3(128, 128, 128)) * layerSize;
    vec3 camPos = vec3(0) - Camera::Position();
    float dist = Max(Max(fabs(camPos.x - blockPos.x), fabs(camPos.y - blockPos.y)), fabs(camPos.z - blockPos.z));

    if (ProjectBlock(block, MVP, layer))
    if (dist < quality * layerSize) // Close enough to split ?
    {
      // Create children
      for (uint8_t cItr = 0; cItr < 8; cItr++)
      {
        if (!block->childPtr[cItr])
        {
          int64_t childAddress = block->children[cItr];
          if (childAddress > 0)
          {
            streamOptions.emplace_back(block, layer * 100000 + dist);
            return false;
            //block->childPtr[cItr] = LoadBlock(childAddress);
            //return true;
          }
        }
      }

      // Descend into children
      for (uint8_t cItr = 0; cItr < 8; cItr++)
      {
        if (block->childPtr[cItr])
        {
          if (RecursiveStream(block->childPtr[cItr], MVP, layer + 1))
            return true;
        }
      }
    }
    return false;
  }

  void RecursiveRender(GloriousBlock *block, const mat4 &MVP, int layer)
  {
    float layerSize = (1.0 / (1LL << layer));
    vec3 blockPos = (block->position + vec3(128, 128, 128)) * layerSize;
    vec3 camPos = vec3(0) - Camera::Position();
    float dist = Max(Max(fabs(camPos.x - blockPos.x), fabs(camPos.y - blockPos.y)), fabs(camPos.z - blockPos.z));

    bool leaf = true;
    // Close enough to split ?
    if (dist < quality * layerSize)
    {
      bool kidsAllReady = true;
      //renderLock.lock();
      for (uint8_t cItr = 0; cItr < 8; cItr++)
        if (block->children[cItr])
          if (!block->childPtr[cItr] || !block->childPtr[cItr]->Ready)
            kidsAllReady = false;
      //renderLock.unlock();
      if (kidsAllReady)
      {
        static SortedList childList;
        childList.list.clear();
        for (uint8_t cItr = 0; cItr < 8; cItr++)
          if (block->childPtr[cItr])
          {
            float clayerSize = (1.0 / (1LL << (layer + 1)));
            vec3 cblockPos = (block->childPtr[cItr]->position + vec3(128, 128, 128)) * clayerSize;
            float cdist = (camPos - cblockPos).Length();
            childList.AddElement(block->childPtr[cItr], 0.f - cdist);
          }
        for (uint8_t cItr = 0; cItr < childList.GetCount(); cItr++)
        {
          leaf = false;
          RecursiveRender((GloriousBlock*)childList.GetElement(cItr), MVP, layer + 1);
        }
      }
    }
    else
    {
      // Unload distant child blocks
      for (uint8_t cItr = 0; cItr < 8; cItr++)
        if (block->childPtr[cItr])
        {
          //renderLock.lock();
          RecursiveUnload(block->childPtr[cItr]);
          block->childPtr[cItr] = nullptr;
          //renderLock.unlock();
        }
    }

    //if (ProjectBlock(block, MVP, layer))
      if (leaf)
      {
        facesDrawn += block->faceCount;
        //renderLock.lock();
        block->Smodel->AssignUniform("LAYER", UT_1f, &layerSize);
        block->Smodel->AssignUniform("regionPos", UT_3f, block->position.Data());

        // Slow bits
        if (!Controls::KeyDown(SDL_SCANCODE_4))
        {
          atlas.UploadToGPU();
          block->Smodel->UploadToGPU();
        }

        block->Smodel->Render(MVP);
        //renderLock.unlock();
      }
  }

  void RecursiveUnload(GloriousBlock *block)
  {
    for (uint8_t cItr = 0; cItr < 8; cItr++)
      if (block->childPtr[cItr])
        RecursiveUnload(block->childPtr[cItr]);
    UnloadBlock(block);
  }

  GloriousBlock *LoadBlock(int64_t discLocation)
  {
    // CPU
    GloriousBlock *block = new GloriousBlock;
    block->discLocation = discLocation;
    fileStream->SetLocation(discLocation);
    memset(&block->childPtr, 0, sizeof(block->childPtr));
    fileStream->ReadBytes(block->children, sizeof(block->children));
    fileStream->ReadBytes(&block->position, sizeof(block->position));
    fileStream->ReadBytes(&block->voxelCount, sizeof(block->voxelCount));
    int64_t voxelDataSize = block->voxelCount * sizeof(voxel);

    if (block->voxelCount == 0)
      return block;

    // Untwist voxel data, yuk!
    uint8_t *data = (uint8_t*)malloc(voxelDataSize);
    fileStream->ReadBytes(data, voxelDataSize);
    block->voxelPosData = malloc(block->voxelCount * 3);
    block->voxelColData = malloc(block->voxelCount * 3);
    for (int64_t vItr = 0; vItr < block->voxelCount; vItr++)
    {
      for (int c = 0; c < 3; c++) ((uint8_t*)block->voxelPosData)[vItr * 3 + c] = data[vItr * 6 + c]; // Positions
      for (int c = 0; c < 3; c++) ((uint8_t*)block->voxelColData)[vItr * 3 + c] = data[vItr * 6 + c + 3]; // Colors
    }
    free(data);

    // Create Grid
    uint32_t *grid = (uint32_t*)calloc(256 * 256 * 256, sizeof(uint32_t));
    uint32_t *ugrid = (uint32_t*)calloc(256 * 256 * 256, sizeof(uint32_t));

    uint8_t minX = 255, minY = 255, minZ = 255;
    uint8_t maxX = 0, maxY = 0, maxZ = 0;

    for (int v = 0; v < block->voxelCount; v++)
    {
      int x = ((uint8_t*)block->voxelPosData)[v * 3 + 0];
      int y = ((uint8_t*)block->voxelPosData)[v * 3 + 1];
      int z = ((uint8_t*)block->voxelPosData)[v * 3 + 2];
      int r = ((uint8_t*)block->voxelColData)[v * 3 + 0];
      int g = ((uint8_t*)block->voxelColData)[v * 3 + 1];
      int b = ((uint8_t*)block->voxelColData)[v * 3 + 2];
      uint32_t i = x + y * 256 + z * 256 * 256;
      uint32_t c = r + g * 256 + b * 256 * 256;
      grid[i] = c;
      ugrid[i] = c;
      minX = Min(minX, x);
      minY = Min(minY, y);
      minZ = Min(minZ, z);
      maxX = Max(maxX, x);
      maxY = Max(maxY, y);
      maxZ = Max(maxZ, z);
    }

    //uint8_t *ugrid = new uint8_t[256 * 256 * 256];
    //for (int y = 0; y < 256; y++)
    //  for (int z = 0; z < 256; z++)
    //    for (int x = 0; x < 256; x++)
    //      ugrid[x + y * 256 + z * 256 * 256] = grid[x + y * 256 + z * 256 * 256] > 0;

    // Create Slices
    std::vector<Face> tops;
    ColouredArrayTexturedRenderObjectMaker maker;

    // Strips
//     for (int y = 0; y < 256; y++)
//       for (int z = 0; z < 256; z++)
//         for (int x = 0; x < 256; x++)
//         {
//           int i = x + y * 256 + z * 256 * 256;
//           uint32_t t = grid[i];
//           if (false) tops.emplace_back(vec3(x, y, z), 1); // Add single face
//           if (t) // Add contiguous strip
//             for (int ix = x + 1; ix < 257; ix++)
//               if (ix == 256 || !grid[ix + y * 256 + z * 256 * 256])
//               {
//                 Face f(vec3(x, y, z), ix - x);
//                 f.width = ix - x;
//                 tops.push_back(f);
//                 x = ix;
//                 break;
//               }
//         }

    // Rects
    for (int y = minY; y <= maxY; y++)
      for (int z = minZ; z <= maxZ; z++)
        for (int x = minX; x <= maxX; x++)
        {
          uint32_t u = ugrid[x + y * 256 + z * 256 * 256];
          if (u)
          {
            block->faceCount++;
            int rowWidth = 256 - x;
            for (int ix = x + 1; ix < 256; ix++)
              if (!ugrid[ix + y * 256 + z * 256 * 256])
              {
                rowWidth = ix - x;
                break;
              }
            int columnHeight = 256 - z;
            for (int iz = z + 1; iz < 256; iz++)
              for (int ix = x; ix < x + rowWidth; ix++)
                if (!ugrid[ix + y * 256 + iz * 256 * 256])
                {
                  columnHeight = iz - z;
                  iz = 257;
                  break;
                }
            Face f(vec3(x, y, z), rowWidth * columnHeight);
            f.width = rowWidth;
            f.height = columnHeight;
            tops.push_back(f);
            for (int fz = 0; fz < columnHeight; fz++)
              for (int fx = 0; fx < rowWidth; fx++)
                ugrid[(x + fx) + y * 256 + (z + fz) * 256 * 256] = 0;
          }
        }

    if (true)
    {
      //FaceOptimizer::SimpleSplitOptimizeCombineFaces(tops, blockTop, 0.5, 0);
      //FaceOptimizer::SimpleSplitOptimizeCombineFaces(tops, blockTop, 1.0, 0);
      //FaceOptimizer::SimpleSplitOptimizeCombineFaces(tops, blockTop, 2.0, 0);
      FaceOptimizer::SimpleSplitOptimizeCombineFaces(tops, blockTop, 3.3333, 0);
      //FaceOptimizer::SimpleSplitOptimizeCombineFaces(tops, blockTop, 4, 0);
    }
    //     FaceOptimizer::SimpleSplitOptimizeCombineFaces(tops, blockTop, 2, 0);
    //     FaceOptimizer::SimpleSplitOptimizeCombineFaces(tops, blockTop, 4, 0);
    //     FaceOptimizer::SimpleSplitOptimizeCombineFaces(tops, blockTop, 8, 0);
    //     FaceOptimizer::SimpleSplitOptimizeCombineFaces(tops, blockTop, 16, 0);

        //printf("built block\n");
    int tile = -1;

    for (int i = 0; i < tops.size(); i++)
    {
      vec2 tl, br;
      uint32_t *img = new uint32_t[tops[i].width * tops[i].height];

      for (int z = 0; z < tops[i].height; z++)
        for (int x = 0; x < tops[i].width; x++)
        {
          int index = (tops[i].pos.x + x) + (tops[i].pos.y) * 256 + (tops[i].pos.z + z) * 256 * 256;
          int c = grid[index];
          if (c) c |= 0xFF000000;
          img[x + z * tops[i].width] = c;
        }

      //renderLock.lock();
      bool added = false;
      for (int t : block->tiles)
        if (atlas.AddTile(img, tops[i].width, tops[i].height, t, &tl, &br))
        {
          added = true;
          tile = t;
          break;
        }
      if (!added)
      {
        tile = atlasFreeTiles.New();
        block->tiles.emplace_back(tile);
        atlas.AddTile(img, tops[i].width, tops[i].height, tile, &tl, &br);
      }
      delete[] img;
      //renderLock.unlock();

      //vec3 c(rand() / (RAND_MAX + 0.0), rand() / (RAND_MAX + 0.0), rand() / (RAND_MAX + 0.0));
      vec3 c(1, 1, 1);
      maker.AddVertex(tops[i].pos + vec3(0, 0, 0), vec3(tl.x, tl.y, tile), c);
      maker.AddVertex(tops[i].pos + vec3(tops[i].width, 0, 0), vec3(br.x, tl.y, tile), c);
      maker.AddVertex(tops[i].pos + vec3(0, 0, tops[i].height), vec3(tl.x, br.y, tile), c);

      maker.AddVertex(tops[i].pos + vec3(tops[i].width, 0, 0), vec3(br.x, tl.y, tile), c);
      maker.AddVertex(tops[i].pos + vec3(tops[i].width, 0, tops[i].height), vec3(br.x, br.y, tile), c);
      maker.AddVertex(tops[i].pos + vec3(0, 0, tops[i].height), vec3(tl.x, br.y, tile), c);
    }
    free(grid);
    free(ugrid);
    for (int t : block->tiles) atlas.ClearLayer(t);
    block->Smodel = maker.AssembleRenderObject();
    block->Smodel->AssignShader(ASSETDIR "Glorious/shaders/Glorious.vert", ASSETDIR "Glorious/shaders/Glorious.frag");
    block->Smodel->AssignTexture("texture0", atlas.texture, TT_2D_ARRAY);

    // GPU
    //block->model.AssignShader(ASSETDIR "NovaCosm/shaders/NovaCosm.vert", ASSETDIR "NovaCosm/shaders/NovaCosm.frag", ASSETDIR "NovaCosm/shaders/NovaCosm.geom");
    //block->model.AssignAttribute("position", GLAttributeType::AT_UNSIGNED_BYTE, block->voxelPosData, 3, block->voxelCount);
    //block->model.AssignAttribute("color", GLAttributeType::AT_UNSIGNED_BYTE_NORM, block->voxelColData, 3, block->voxelCount);

    //renderLock.lock();
    block->Ready = true;
    //renderLock.unlock();

    return block;
  }

  void UnloadBlock(GloriousBlock *block)
  {
    free(block->voxelPosData);
    free(block->voxelColData);
    block->Smodel->Clear();
    for (int t : block->tiles) atlasFreeTiles.Old(t);
    delete block;
  }

  int ProjectBlock(GloriousBlock *block, const mat4 &MVP, int layer)
  {
    mat4 lMVP = MVP;
    lMVP.Transpose();
    float layerSize = (1.0 / (1LL << layer));
    // Projects
    auto blockPos = block->position;

    bool visable = false;

    vec2 low(99999, 99999);
    vec2 high(-99999, -99999);

    for (int z = 0; z <= 1; z++)
      for (int y = 0; y <= 1; y++)
        for (int x = 0; x <= 1; x++)
        {
          bool onScreen = false;
          auto pos = (blockPos + vec3(x * 256, y * 256, z * 256)) * layerSize;
          auto viewPos = lMVP * vec4(pos.x, pos.y, pos.z, 1);
          onScreen = viewPos.w > 0 - layerSize - 1.75;
          viewPos = viewPos / viewPos.w;
          vec2 p = vec2(viewPos.x, viewPos.y);
          if (onScreen)
          {
            low = vec2(Min(low.x, p.x), Min(low.y, p.y));
            high = vec2(Max(low.x, p.x), Max(low.y, p.y));
            visable = true;
          }
        }

    if (!visable)
      return 0;

    low = vec2(Max(low.x, -1), Max(low.y, -1));
    high = vec2(Min(high.x, 1), Min(high.y, 1));

    float blockSize = Max(high.x - low.x, high.y - low.y);

    //auto highPos = (blockPos + vec3(256, 256, 256)) * layerSize;
    //auto highPosView = lMVP * vec4(highPos.x, highPos.y, highPos.z, 1);
    //lowPosView = lowPosView / lowPosView.w;
    //highPosView = highPosView / highPosView.w;
    
    //if (lowPosView.w > 0.f && highPosView.w > 0.f)
    if (blockSize > 0.f)
      return 1;
    return 0;
  }

};

#endif // GloriousModel_h__
