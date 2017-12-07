
#include "BlockWorld.h"
#include "Textures.h"
#include "FaceOptimizer.h"
#include "Chunk.h"
#include "Assets.h"
#include "Region.h"
#include "BlockMesher.h"
#include "CustomRenderObjects.h"
#include "ChunkHandler.h"
#include "Controls.h"
#include "Shaders.h"

RenderObject *TexelMeshChunk(BlockWorld *world, int chunkX, int chunkZ)
{
  // Transparent blocks
  ColouredArrayTexturedRenderObjectMaker mesh;
  for (int y = 25; y < Chunk::height; y++)
    for (int z = 0; z < Chunk::length; z++)
      for (int x = 0; x < Chunk::width; x++)
        BlockMesher::CreateTexelVoxelMesh(world, vec3i(x + chunkX * Chunk::width, y, z + chunkZ * Chunk::length), &mesh);

  RenderObject *model = mesh.AssembleRenderObject();
  model->AssignShader(ASSETDIR "Minecraft/shaders/texelWorld.vert", ASSETDIR "Minecraft/shaders/texelWorld.frag");
  return model;
}

RenderObject *AlphaMeshChunk(BlockWorld *world, int chunkX, int chunkZ)
{
  // Transparent blocks
  ColouredArrayTexturedRenderObjectMaker mesh;
  for (int y = 25; y < Chunk::height; y++)
    for (int z = 0; z < Chunk::length; z++)
      for (int x = 0; x < Chunk::width; x++)
        BlockMesher::CreateAlphaVoxelMesh(world, vec3i(x + chunkX * Chunk::width, y, z + chunkZ * Chunk::length), &mesh);

  RenderObject *model = mesh.AssembleRenderObject();
  model->AssignShader(ASSETDIR "Minecraft/shaders/alphaWorld.vert", ASSETDIR "Minecraft/shaders/alphaWorld.frag");
  return model;
}

RenderObject *SolidMeshChunk(BlockWorld *world, int chunkX, int chunkZ)
{
  ColouredArrayTexturedRenderObjectMaker mesh;

  // Setup face lists
  std::vector<Face> tops, bots, fronts, backs, rights, lefts;
  for (int y = 25; y < Chunk::height; y++)
    for (int z = 0; z < Chunk::length; z++)
      for (int x = 0; x < Chunk::width; x++)
      {
        vec3i pos = vec3i(x + chunkX * Chunk::width, y, z + chunkZ * Chunk::length);
        BlockMesher::CreateVoxelFace(world, pos, tops, blockTop);
        BlockMesher::CreateVoxelFace(world, pos, bots, blockBot);
        BlockMesher::CreateVoxelFace(world, pos, fronts, blockFront);
        BlockMesher::CreateVoxelFace(world, pos, backs, blockBack);
        BlockMesher::CreateVoxelFace(world, pos, rights, blockRight);
        BlockMesher::CreateVoxelFace(world, pos, lefts, blockLeft);
      }

  // Optimize face lists
  if (world->combineFaces)
  {
    FaceOptimizer::OptimizeFaces(world, tops, blockTop);
    FaceOptimizer::OptimizeFaces(world, bots, blockBot);
    FaceOptimizer::OptimizeFaces(world, fronts, blockFront);
    FaceOptimizer::OptimizeFaces(world, backs, blockBack);
    FaceOptimizer::OptimizeFaces(world, rights, blockRight);
    FaceOptimizer::OptimizeFaces(world, lefts, blockLeft);
  }

  // Mesh face lists
  BlockMesher::CreateFaceListMesh(world, tops, &mesh, blockTop); tops.clear();
  BlockMesher::CreateFaceListMesh(world, bots, &mesh, blockBot); bots.clear();
  BlockMesher::CreateFaceListMesh(world, fronts, &mesh, blockFront); fronts.clear();
  BlockMesher::CreateFaceListMesh(world, backs, &mesh, blockBack); backs.clear();
  BlockMesher::CreateFaceListMesh(world, rights, &mesh, blockRight); rights.clear();
  BlockMesher::CreateFaceListMesh(world, lefts, &mesh, blockLeft); lefts.clear();

  RenderObject *model = mesh.AssembleRenderObject();
  model->AssignShader(ASSETDIR "Minecraft/shaders/solidWorld.vert", ASSETDIR "Minecraft/shaders/solidWorld.frag");
  return model;
}


BlockWorld::BlockWorld(const char *regionDirectory, int _viewDist)
{
  worldFile = strdup(regionDirectory);
  viewDist = _viewDist;
  blackTexture = Textures::LoadTextureArray(ASSETDIR "Minecraft/Black.png", 16, 16);
  atlastexture = Textures::LoadTextureArray(ASSETDIR "Minecraft/BlockAtlasBig.png", 256, 256);
}


bool BlockWorld::RayTrace(vec3 _pos, vec3 _dir, float length, vec3i *FirstOccupiedBlock, vec3i *LastEmptyBlock)
{
  // Store Fine Grain Position
  vec3 pos = _pos;

  // Store max travel time
  float travelTime = length;

  // Determine directional positivity
  vec3i attitude(_dir.x > 0, _dir.y > 0, _dir.z > 0);

  // Store voxel grain position
  vec3i wholePos(floor(pos.x), floor(pos.y), floor(pos.z));
  vec3i prevWPos;

  // Store inverse momentum
  vec3 invDir = vec3(1) / _dir;

  while (travelTime > 0)
  {
    // Calculate dimensional crossing
    vec3 crossDist = ((vec3(wholePos) + vec3(attitude)) - pos) * invDir;

    // Determine first dimension crossed
    int FirstCross = (((crossDist.x >= crossDist.y)*(crossDist.z > crossDist.y))) + (((crossDist.x >= crossDist.z)*(crossDist.y >= crossDist.z)) * 2);

    // step across dimensions
    switch (FirstCross)
    {
    case 0:
      wholePos.x += attitude.x * 2 - 1;
      pos += _dir * crossDist.x;
      travelTime -= crossDist.x;
      break;
    case 1:
      wholePos.y += attitude.y * 2 - 1;
      pos += _dir * crossDist.y;
      travelTime -= crossDist.y;
      break;
    case 2:
      wholePos.z += attitude.z * 2 - 1;
      pos += _dir * crossDist.z;
      travelTime -= crossDist.z;
      break;
    }
    if (blockTypes.IsBlockEnabled(GetBlock(wholePos)) && blockTypes.GetBlockModel(GetBlock(wholePos)) != liquid)
    {
      if (FirstOccupiedBlock) *FirstOccupiedBlock = wholePos;
      if (LastEmptyBlock) *LastEmptyBlock = prevWPos;
      return true;
    }
    prevWPos = wholePos;
  }
  return false;
}

bool BlockWorld::BoxCollides(vec3 _pos, vec3 _size)
{
  _pos -= _size * 0.5; // center
  for (int64_t x = floor(_pos.x); x < ceil(_pos.x + _size.x); x++)
    for (int64_t y = floor(_pos.y); y < ceil(_pos.y + _size.y); y++)
      for (int64_t z = floor(_pos.z); z < ceil(_pos.z + _size.z); z++)
        if (blockTypes.IsBlockSolid(GetBlock(vec3i(x, y, z))))
          return true;
  return false;
}

bool BlockWorld::BoxContains(vec3 _pos, vec3 _size, uint8_t blockID)
{
  _pos -= _size * 0.5; // centre
  for (int64_t x = floor(_pos.x); x < ceil(_pos.x + _size.x); x++)
    for (int64_t y = floor(_pos.y); y < ceil(_pos.y + _size.y); y++)
      for (int64_t z = floor(_pos.z); z < ceil(_pos.z + _size.z); z++)
        if (GetBlock(vec3i(x, y, z)) == blockID)
          return true;
  return false;
}

void BlockWorld::SetBlock(vec3i blockPos, uint8_t blockID)
{
  blockPos += vec3i(Chunk::width * 1000000, Chunk::height * 1000000, Chunk::length * 1000000);
  vec3i chunkPos = blockPos / vec3i(Chunk::width, Chunk::height, Chunk::length) - vec3i(1000000);
  int chunkID = chunks.FindChunk(chunkPos.x, chunkPos.z);
  if (chunkID >= 0)
  {
    ChunkInstance &c = *chunks.GetChunkInstance(chunkID);
    c.chunk->SetBlock(blockPos.x % Chunk::width, blockPos.y % Chunk::height, blockPos.z % Chunk::length, blockID);
    c.hasChanged = true;

    // Neighboring chunks
    if (blockPos.x % Chunk::width == 0) { int oc = chunks.FindChunk(c.xPos - 1, c.zPos); if (oc >= 0) chunks.GetChunkInstance(oc)->hasChanged = true; };
    if (blockPos.x % Chunk::width == 15) { int oc = chunks.FindChunk(c.xPos + 1, c.zPos); if (oc >= 0) chunks.GetChunkInstance(oc)->hasChanged = true; };
    if (blockPos.z % Chunk::length == 0) { int oc = chunks.FindChunk(c.xPos, c.zPos - 1); if (oc >= 0) chunks.GetChunkInstance(oc)->hasChanged = true; };
    if (blockPos.z % Chunk::length == 15) { int oc = chunks.FindChunk(c.xPos, c.zPos + 1); if (oc >= 0) chunks.GetChunkInstance(oc)->hasChanged = true; };
  }
}


// Optimized implementation
uint8_t BlockWorld::GetBlock(vec3i blockPos)
{
  int x = (blockPos.x + 160000);
  int z = (blockPos.z + 160000);

  int cx = (x >> 4) - 10000;
  int cz = (z >> 4) - 10000;

  int chunkID = chunks.FindChunk(cx, cz);
  if (chunkID >= 0)
    return chunks.GetChunkInstance(chunkID)->chunk->GetBlock(x & 0xf, (blockPos.y + 256) & 0xFF, z & 0xf);
  return 0;
}

// Reference implementation
// uint8_t BlockWorld::GetBlock(vec3i blockPos)
// {
//   blockPos += vec3i(Chunk::width * 1000000, Chunk::height * 1000000, Chunk::length * 1000000);
//   vec3i chunkPos = blockPos / vec3i(Chunk::width, Chunk::height, Chunk::length) - vec3i(1000000);
//   int chunkID = chunks.FindChunk(chunkPos.x, chunkPos.z);
//   if (chunkID >= 0) return chunks.GetChunkInstance(chunkID)->chunk->GetBlock(blockPos.x % Chunk::width, blockPos.y % Chunk::height, blockPos.z % Chunk::length);
//   return 0;
// }

void BlockWorld::Render(mat4 &VP)
{
  renderListLock.lock();

  // Toggle combine Mode !
  static bool combineDown = false;
  if (Controls::KeyDown(SDL_SCANCODE_2) || Controls::GetControllerButton(9))
  {
    if (!combineDown)
    {
      combineFaces = !combineFaces;
      for (auto &chunkID : meshedChunks)
      {
        loadedChunks.push_back(chunkID);
        chunks.GetChunkInstance(chunkID)->DeleteMesh();
      }
      meshedChunks.clear();
    }
    combineDown = true;
  }
  else combineDown = false;

  lightingAtlas.UploadToGPU();

  // Toggle Wire frame Mode !
  static bool wireDown = false;
  static bool wire = false;
  if (Controls::KeyDown(SDL_SCANCODE_1) || Controls::GetControllerButton(13))
  {
    if (!wireDown)
      wire = !wire;
    wireDown = true;
  }
  else wireDown = false;

  mat4 TVP = VP;
  TVP.Transpose();

  // Draw World
  if (wire)
  {
    glLineWidth(4);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonOffset(0, 1024);
    glEnable(GL_POLYGON_OFFSET_FILL);
    for (int cID = 0; cID < meshedChunks.size(); cID++)
    {
      //chunks.GetChunkInstance(meshedChunks[cID])->mesh->AssignTexture("texture0", lightingAtlas.texture, TT_2D);
      chunks.GetChunkInstance(meshedChunks[cID])->solidMesh->AssignTexture("texture0", blackTexture, TT_2D_ARRAY);
      chunks.GetChunkInstance(meshedChunks[cID])->solidMesh->Render(TVP);
      chunks.GetChunkInstance(meshedChunks[cID])->alphaMesh->AssignTexture("texture0", blackTexture, TT_2D_ARRAY);
      chunks.GetChunkInstance(meshedChunks[cID])->alphaMesh->Render(TVP);
      chunks.GetChunkInstance(meshedChunks[cID])->texelMesh->AssignTexture("texture0", blackTexture, TT_2D_ARRAY);
      chunks.GetChunkInstance(meshedChunks[cID])->texelMesh->Render(TVP);
    }
    glDisable(GL_POLYGON_OFFSET_FILL);
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  for (int cID = 0; cID < meshedChunks.size(); cID++)
  { // Solid Blocks
    //chunks.GetChunkInstance(meshedChunks[cID])->mesh->AssignTexture("texture0", lightingAtlas.texture, TT_2D);
    chunks.GetChunkInstance(meshedChunks[cID])->lifeTime *= 1.2;
    chunks.GetChunkInstance(meshedChunks[cID])->lifeTime += 0.03;
    float f = Min(chunks.GetChunkInstance(meshedChunks[cID])->lifeTime, 1);

    chunks.GetChunkInstance(meshedChunks[cID])->solidMesh->AssignTexture("texture0", atlastexture, TT_2D_ARRAY);
    chunks.GetChunkInstance(meshedChunks[cID])->solidMesh->AssignUniform("fade", UT_1f, &f);
    chunks.GetChunkInstance(meshedChunks[cID])->solidMesh->Render(TVP);
  }

  for (int cID = 0; cID < meshedChunks.size(); cID++)
  { // Texel Blocks
    float f = Min(chunks.GetChunkInstance(meshedChunks[cID])->lifeTime, 1);
    chunks.GetChunkInstance(meshedChunks[cID])->texelMesh->AssignTexture("texture0", atlastexture, TT_2D_ARRAY);
    chunks.GetChunkInstance(meshedChunks[cID])->texelMesh->AssignUniform("fade", UT_1f, &f);
    chunks.GetChunkInstance(meshedChunks[cID])->texelMesh->Render(TVP);
  }

  glDepthMask(false);
  for (int cID = 0; cID < meshedChunks.size(); cID++)
  { // Alpha Blocks
    float f = Min(chunks.GetChunkInstance(meshedChunks[cID])->lifeTime, 1);
    chunks.GetChunkInstance(meshedChunks[cID])->alphaMesh->AssignTexture("texture0", atlastexture, TT_2D_ARRAY);
    chunks.GetChunkInstance(meshedChunks[cID])->alphaMesh->AssignUniform("fade", UT_1f, &f);
    chunks.GetChunkInstance(meshedChunks[cID])->alphaMesh->Render(TVP);
  }
  glDepthMask(true);

  // Cleanup unused resources for the GPU
  if (oldMeshes.size())
  {
    for (int mID = 0; mID < oldMeshes.size(); mID++)
      oldMeshes[mID]->Clear();
    oldMeshes.clear();
  }

  entities.Draw(VP);
  
  renderListLock.unlock();
}

void BlockWorld::Stream(vec3 camPos)
{
  // Calculate viewers chunk location
  vec3i chunkPos = vec3i(floor(camPos.x / Chunk::width), floor(camPos.y / Chunk::height), floor(camPos.z / Chunk::length));

  // Load nearby chunks
  bool foundChunk = false;
  int bestChunkID;
  int bestChunkXpos;
  int bestChunkZpos;
  int bestChunkDist = 99999999;
  int bestChunkListID;
  for (int z = chunkPos.z - (viewDist); z <= chunkPos.z + (viewDist); z++)
    for (int x = chunkPos.x - (viewDist); x <= chunkPos.x + (viewDist); x++)
    {
      int chunkID = chunks.FindChunk(x, z);
      if (chunkID < 0) // chunk not loaded
      {
        int dist = Max(abs(x - chunkPos.x), abs(z - chunkPos.z)) * 10000 + (abs(x - chunkPos.x) + abs(z - chunkPos.z));
        if (dist < bestChunkDist)
        {
          foundChunk = true;
          bestChunkDist = dist;
          bestChunkXpos = x;
          bestChunkZpos = z;
        }
      }
    }
  if (foundChunk)
  {
    int chunkID = chunks.AddChunk(bestChunkXpos, bestChunkZpos);
    ChunkInstance &c = *chunks.GetChunkInstance(chunkID);
    c.chunk = new Chunk;
    if (LoadChunkFromRegion(c.chunk, c.xPos, c.zPos, worldFile))
    {
      c.hasBlocks = true;
      loadedChunks.push_back(chunkID);
    }
  }


  //// un-mesh distant meshed chunks
  for (int i = 0; i < meshedChunks.size(); i++)
  {
    ChunkInstance &c = *chunks.GetChunkInstance(meshedChunks[i]);
    int dist = Max(abs(c.xPos - chunkPos.x), abs(c.zPos - chunkPos.z));
    if (dist >= viewDist)
    {
      if (renderListLock.try_lock())
      {
        oldMeshes.push_back(c.solidMesh);
        c.solidMesh = nullptr;
        loadedChunks.push_back(meshedChunks[i]);
        meshedChunks.erase(meshedChunks.begin() + i);
        renderListLock.unlock();
        i--; continue;
      }
    }
  }

  // un-load distant loaded chunks
  for (int i = 0; i < loadedChunks.size(); i++)
  {
    ChunkInstance &c = *chunks.GetChunkInstance(loadedChunks[i]);
    int dist = Max(abs(c.xPos - chunkPos.x), abs(c.zPos - chunkPos.z));
    if (dist > viewDist)
    {
      if (renderListLock.try_lock())
      {
        chunks.DeleteChunk(loadedChunks[i]);
        c.DeleteMemory();
        loadedChunks.erase(loadedChunks.begin() + i);
        renderListLock.unlock();
        i--; continue;
      }
    }
  }

  // Update all chunks which have been changed at once
  static std::vector<int> updateChunkList;
  static std::vector<RenderObject*> updateSolidMeshList;
  static std::vector<RenderObject*> updateAlphaMeshList;
  static std::vector<RenderObject*> updateTexelMeshList;
  for (int i = 0; i < meshedChunks.size(); i++)
  {
    ChunkInstance &c = *chunks.GetChunkInstance(meshedChunks[i]);
    if (c.hasChanged)
    {
      updateChunkList.push_back(meshedChunks[i]);
      updateSolidMeshList.push_back(SolidMeshChunk(this, c.xPos, c.zPos));
      updateAlphaMeshList.push_back(AlphaMeshChunk(this, c.xPos, c.zPos));
      updateTexelMeshList.push_back(TexelMeshChunk(this, c.xPos, c.zPos));
      c.hasChanged = false;
    }
  }
  if (updateChunkList.size())
  {
    renderListLock.lock();
    for (int i = 0; i < updateChunkList.size(); i++)
    {
      ChunkInstance &c = *chunks.GetChunkInstance(updateChunkList[i]);
      if (c.solidMesh)
      {
        oldMeshes.push_back(c.solidMesh);
        c.solidMesh = updateSolidMeshList[i];
        c.alphaMesh = updateAlphaMeshList[i];
        c.texelMesh = updateTexelMeshList[i];
      }
    }
    renderListLock.unlock();
  }
  updateChunkList.clear();
  updateSolidMeshList.clear();
  updateAlphaMeshList.clear();
  updateTexelMeshList.clear();

  // Mesh loaded chunks who's neighbors are also loaded
  foundChunk = false;
  bestChunkDist = 99999999;
  for (int i = 0; i < loadedChunks.size(); i++)
    {
      ChunkInstance &c = *chunks.GetChunkInstance(loadedChunks[i]);
      if(c.hasBlocks)
        if ((chunks.FindChunk(c.xPos + 1, c.zPos) >= 0) &&
          (chunks.FindChunk(c.xPos - 1, c.zPos) >= 0) &&
          (chunks.FindChunk(c.xPos, c.zPos + 1) >= 0) &&
          (chunks.FindChunk(c.xPos, c.zPos - 1) >= 0))
        {
          int dist = Max(abs(c.xPos - chunkPos.x), abs(c.zPos - chunkPos.z)) * 10000 + (abs(c.xPos - chunkPos.x) + abs(c.zPos - chunkPos.z));
          if (dist < bestChunkDist)
          {
            foundChunk = true;
            bestChunkDist = dist;
            bestChunkID = loadedChunks[i];
            bestChunkXpos = c.xPos;
            bestChunkZpos = c.zPos;
            bestChunkListID = i;
          }
        }
    }
  if (foundChunk)
  {
    // Store Meshed Chunk
    ChunkInstance &c = *chunks.GetChunkInstance(bestChunkID);
    c.solidMesh = SolidMeshChunk(this, bestChunkXpos, bestChunkZpos);
    c.alphaMesh = AlphaMeshChunk(this, bestChunkXpos, bestChunkZpos);
    c.texelMesh = TexelMeshChunk(this, bestChunkXpos, bestChunkZpos);

    // Delete Loaded Chunk
    loadedChunks.erase(loadedChunks.begin() + bestChunkListID);

    // Create meshed chunk
    renderListLock.lock();
    meshedChunks.push_back(bestChunkID);
    renderListLock.unlock();
  }
}
