#include "BlockWorld.h"
#include "CustomRenderObjects.h"
#include "BlockMesher.h"

uint32_t *CreateGradient2(vec3 v1, vec3 v2, vec3 v3, vec3 v4)
{
  static uint32_t img[2 * 2];
  for (int y = 0; y < 2; y++)
    for (int x = 0; x < 2; x++)
    {
      float ax = x;
      float ay = y;
      vec3 top = v1 * (1.0f - ax) + v2 *ax;
      vec3 bot = v3 * (1.0f - ax) + v4 *ax;
      vec3 val = top * (1.0f - ay) + bot *ay;
      img[x + y * 2] = int(val.x * 255) + (int(val.y * 255) << 8) + (int(val.z * 255) << 16);
    }
  return img;
}

uint32_t *CreateGradient4(vec3 v1, vec3 v2, vec3 v3, vec3 v4)
{
  static uint32_t img[4 * 4];
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
    {
      float ax = x / 3.0f;
      float ay = y / 3.0f;
      vec3 top = v1 * (1.0f - ax) + v2 *ax;
      vec3 bot = v3 * (1.0f - ax) + v4 *ax;
      vec3 val = top * (1.0f - ay) + bot *ay;
      img[x + y * 4] = int(val.x * 255) + (int(val.y * 255) << 8) + (int(val.z * 255) << 16);
    }
  return img;
}

uint32_t *CreateGradient8(vec3 v1, vec3 v2, vec3 v3, vec3 v4)
{
  static uint32_t img[8 * 8];
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++)
    {
      float ax = x / 7.0f;
      float ay = y / 7.0f;
      vec3 top = v1 * (1.0f - ax) + v2 *ax;
      vec3 bot = v3 * (1.0f - ax) + v4 *ax;
      vec3 val = top * (1.0f - ay) + bot *ay;
      img[x + y * 8] = int(val.x * 255) + (int(val.y * 255) << 8) + (int(val.z * 255) << 16);
    }
  return img;
}

uint32_t *CreateGradient16(vec3 v1, vec3 v2, vec3 v3, vec3 v4)
{
  static uint32_t img[16 * 16];
  for (int y = 0; y < 16; y++)
    for (int x = 0; x < 16; x++)
    {
      float ax = x / 15.0f;
      float ay = y / 15.0f;
      vec3 top = v1 * (1.0f - ax) + v2 *ax;
      vec3 bot = v3 * (1.0f - ax) + v4 *ax;
      vec3 val = top * (1.0f - ay) + bot *ay;
      img[x + y * 16] = int(val.x * 255) + (int(val.y * 255) << 8) + (int(val.z * 255) << 16);
    }
  return img;
}

uint32_t *CreateGradient32(vec3 v1, vec3 v2, vec3 v3, vec3 v4)
{
  static uint32_t img[32 * 32];
  for (int y = 0; y < 32; y++)
    for (int x = 0; x < 32; x++)
    {
      float ax = x / 31.0f;
      float ay = y / 31.0f;
      vec3 top = v1 * (1.0f - ax) + v2 *ax;
      vec3 bot = v3 * (1.0f - ax) + v4 *ax;
      vec3 val = top * (1.0f - ay) + bot *ay;
      img[x + y * 32] = int(val.x * 255) + (int(val.y * 255) << 8) + (int(val.z * 255) << 16);
    }
  return img;
}

bool BlockMesher::GetBlockInShadow(BlockWorld *world, vec3i blockPos, int searchSize /*= 40*/)
{
  for (int c = 1; c <= searchSize; c++)
    if (world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(0, c, 0))))
      return true;
  return false;
}

uint8_t BlockMesher::GetAONeighbourhood(BlockWorld *world, vec3i blockPos, blockSide side)
{
  uint8_t o = 0;
  uint8_t i = 0;
  for (int x = -1; x <= 1; x++)
    for (int y = -1; y <= 1; y++)
    {
      if (!x && !y) continue;
      uint8_t taken;
      switch (side)
      {
      case blockTop: taken = world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(x, 1, y))); break;
      case blockBot: taken = world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(x, -1, y))); break;
      case blockFront: taken = world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(x, y, 1))); break;
      case blockBack: taken = world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(x, y, -1))); break;
      case blockRight: taken = world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(1, y, x))); break;
      case blockLeft: taken = world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(-1, y, x))); break;
      }
      o = o | (taken << i++);
    }
  return o;
}

float BlockMesher::ApplyAOtoVert(uint8_t neighbourhood, uint8_t vert, float occlusionFactor /*= 0.7f*/)
{
  uint8_t a;
  switch (vert)
  {
  case 0: a = neighbourhood & 0b00001011; break;
  case 1: a = neighbourhood & 0b00010110; break;
  case 2: a = neighbourhood & 0b01101000; break;
  case 3: a = neighbourhood & 0b11010000; break;
  }
  float light = 1.0f;
  for (uint8_t i = 0; i < 8; i++)
    if ((a >> i) & 1)
      light *= occlusionFactor;
  return light;
}

void BlockMesher::CreateVoxelFace(BlockWorld *world, vec3i blockPos, std::vector<Face> &faceList, blockSide side)
{
  uint8_t block = world->GetBlock(blockPos);
  if (!world->blockTypes.IsBlockEnabled(block)) return;
  vec3i off;
  switch (side)
  {
  case blockTop:    off = vec3i(0, 1, 0);   break;
  case blockBot:    off = vec3i(0, -1, 0);  break;
  case blockFront:  off = vec3i(0, 0, 1);   break;
  case blockBack:   off = vec3i(0, 0, -1);  break;
  case blockRight:  off = vec3i(1, 0, 0);   break;
  case blockLeft:   off = vec3i(-1, 0, 0);  break;
  }

  if (!world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + off)))
  {
    faceList.emplace_back(blockPos, block);
  }
}

void BlockMesher::CreateFaceListMesh(BlockWorld *world, std::vector<Face> &faces, ColouredArrayTexturedRenderObjectMaker *mesh, blockSide side)
{
  for (int f = 0; f < faces.size(); f++)
  {
    vec3i off;
    bool YZS = false; // frontBack
    bool XYS = false; // leftRight
    vec3 c;
    switch (side)
    {
    case blockTop:    c = 1;   off = vec3i(0, 1, 0);  break;
    case blockBot:    c = 0.4; off = vec3i(0, 0, 0);  break;
    case blockFront:  c = 0.8; off = vec3i(0, 0, 1); YZS = true; break;
    case blockBack:   c = 0.8; off = vec3i(0, 0, 0); YZS = true; break;
    case blockRight:  c = 0.6; off = vec3i(1, 0, 0); XYS = true; break;
    case blockLeft:   c = 0.6; off = vec3i(0, 0, 0); XYS = true; break;
    }

    int w = faces[f].width;
    int h = faces[f].height;

    vec3i verts[6];
    verts[0] = vec3i(0, 0, 0);
    verts[1] = vec3i(w, 0, 0);
    verts[2] = vec3i(0, 0, h);
    verts[3] = vec3i(w, 0, h);
    for (int s = 0; s < 4; s++)
    {
      if (YZS) verts[s] = vec3i(verts[s].x, verts[s].z, verts[s].y);
      if (XYS) verts[s] = vec3i(verts[s].y, verts[s].x, verts[s].z);
    }

    //uint8_t tile = world->blockTypes.GetBlockTile(faces[f].Type, side);
    //vec2 uvs[4];
    //uvs[0] = vec2(0, h);
    //uvs[1] = vec2(w, h);
    //uvs[2] = vec2(0, 0);
    //uvs[3] = vec2(w, 0);
    //for (int s = 0; s < 4; s++) // Rotate
    //{
    //  if (XYS) uvs[s] = vec2i(uvs[s].y, w - uvs[s].x);
    //}


    // Lighting
    uint8_t n = 0;
    vec3 nc = c;
    if (side == blockTop)
    {
      n = GetAONeighbourhood(world, faces[f].pos, blockTop);
      if (GetBlockInShadow(world, faces[f].pos)) nc = nc * 0.5f;
    }

    if (side == blockBot)
    {
      n = GetAONeighbourhood(world, faces[f].pos, blockBot);
      nc = nc * 0.5;
    }

    if (side == blockFront)
    {
      n = GetAONeighbourhood(world, faces[f].pos, blockFront);
      if (GetBlockInShadow(world, faces[f].pos + vec3i(0, 0, 1))) nc = nc * 0.5f;
    }

    if (side == blockBack)
    {
      n = GetAONeighbourhood(world, faces[f].pos, blockBack);
      if (GetBlockInShadow(world, faces[f].pos + vec3i(0, 0, -1))) nc = nc * 0.5f;
    }

    if (side == blockRight)
    {
      n = GetAONeighbourhood(world, faces[f].pos, blockRight);
      if (GetBlockInShadow(world, faces[f].pos + vec3i(1, 0, 0))) nc = nc * 0.5f;
    }

    if (side == blockLeft)
    {
      n = GetAONeighbourhood(world, faces[f].pos, blockLeft);
      if (GetBlockInShadow(world, faces[f].pos + vec3i(-1, 0, 0))) nc = nc * 0.5f;
    }

    vec3 c1;
    vec3 c2;
    vec3 c3;
    vec3 c4;
    if (side == blockRight || side == blockLeft)
    {
      c1 = nc * ApplyAOtoVert(n, 0);
      c2 = nc * ApplyAOtoVert(n, 1);
      c3 = nc * ApplyAOtoVert(n, 2);
      c4 = nc * ApplyAOtoVert(n, 3);
    }
    else
    {
      c1 = nc * ApplyAOtoVert(n, 0);
      c2 = nc * ApplyAOtoVert(n, 2);
      c3 = nc * ApplyAOtoVert(n, 1);
      c4 = nc * ApplyAOtoVert(n, 3);
    }

    uint8_t tile = world->blockTypes.GetBlockTile(faces[f].Type, side);
    vec3 uvs[4];
    uvs[0] = vec3(0, h, tile);
    uvs[1] = vec3(w, h, tile);
    uvs[2] = vec3(0, 0, tile);
    uvs[3] = vec3(w, 0, tile);
    for (int s = 0; s < 4; s++) // Rotate
    {
      if (XYS) uvs[s] = vec3i(uvs[s].y, w - uvs[s].x, uvs[s].z);
    }

    mesh->AddVertex(vec3(faces[f].pos + verts[0] + off), uvs[0], c1);
    mesh->AddVertex(vec3(faces[f].pos + verts[1] + off), uvs[1], c2);
    mesh->AddVertex(vec3(faces[f].pos + verts[2] + off), uvs[2], c3);

    mesh->AddVertex(vec3(faces[f].pos + verts[1] + off), uvs[1], c2);
    mesh->AddVertex(vec3(faces[f].pos + verts[3] + off), uvs[3], c4);
    mesh->AddVertex(vec3(faces[f].pos + verts[2] + off), uvs[2], c3);
  }
  faces.clear();
}

//     float ITS = 1.0;
//     uint8_t block = world->GetBlock(blockPos);
//     if (!world->blockTypes.IsBlockEnabled(block)) return;
// 
//     // Top
//     if (!world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(0, 1, 0))))
//     {
//       float light = 1.0f * blockInShadow(world, blockPos);
//       int tile = world->blockTypes.GetBlockTile(block, blockTop);
//       uint8_t n = AONeighbourhood(world, blockPos, blockTop);
// 
//       // Triangle 1
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y + 1, blockPos.z), vec3(0, ITS, tile), light * ApplyAOtoVert(n, 0));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y + 1, blockPos.z), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y + 1, blockPos.z + 1), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//       // Triangle 2
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y + 1, blockPos.z), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y + 1, blockPos.z + 1), vec3(ITS, 0, tile), light * ApplyAOtoVert(n, 3));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y + 1, blockPos.z + 1), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//     }
// 
// 
//     // Bottom
//     if (!world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(0, -1, 0))))
//     {
//       float light = 0.4f * blockInShadow(world, blockPos);
//       int tile = world->blockTypes.GetBlockTile(block, blockBot);
//       uint8_t n = AONeighbourhood(world, blockPos, blockBot);
//       // Triangle 1
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y, blockPos.z), vec3(0, ITS, tile), light * ApplyAOtoVert(n, 0));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y, blockPos.z), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y, blockPos.z + 1), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//       // Triangle 2
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y, blockPos.z), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y, blockPos.z + 1), vec3(ITS, 0, tile), light * ApplyAOtoVert(n, 3));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y, blockPos.z + 1), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//     }
// 
//     // Front
//     if (!world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(0, 0, 1))))
//     {
//       float light = 0.8f * blockInShadow(world, blockPos + vec3i(0, 0, 1));
//       int tile = world->blockTypes.GetBlockTile(block, blockFront);
//       uint8_t n = AONeighbourhood(world, blockPos, blockFront);
//       // Triangle 1
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y, blockPos.z + 1), vec3(0, ITS, tile), light * ApplyAOtoVert(n, 0));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y, blockPos.z + 1), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y + 1, blockPos.z + 1), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//       // Triangle 2
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y, blockPos.z + 1), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y + 1, blockPos.z + 1), vec3(ITS, 0, tile), light * ApplyAOtoVert(n, 3));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y + 1, blockPos.z + 1), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//     }
// 
// 
//     // Back
//     if (!world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(0, 0, -1))))
//     {
//       float light = 0.8f * blockInShadow(world, blockPos + vec3i(0, 0, -1));
//       int tile = world->blockTypes.GetBlockTile(block, blockBack);
//       uint8_t n = AONeighbourhood(world, blockPos, blockBack);
//       // Triangle 1
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y, blockPos.z), vec3(0, ITS, tile), light * ApplyAOtoVert(n, 0));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y, blockPos.z), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y + 1, blockPos.z), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//       // Triangle 2
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y, blockPos.z), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y + 1, blockPos.z), vec3(ITS, 0, tile), light * ApplyAOtoVert(n, 3));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y + 1, blockPos.z), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//     }
// 
//     // Right
//     if (!world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(1, 0, 0))))
//     {
//       float light = 0.6f * blockInShadow(world, blockPos + vec3i(1, 0, 0));
//       int tile = world->blockTypes.GetBlockTile(block, blockRight);
//       uint8_t n = AONeighbourhood(world, blockPos, blockRight);
//       // Triangle 1
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y, blockPos.z), vec3(0, ITS, tile), light * ApplyAOtoVert(n, 0));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y, blockPos.z + 1), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y + 1, blockPos.z), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//       // Triangle 2
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y, blockPos.z + 1), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y + 1, blockPos.z + 1), vec3(ITS, 0, tile), light * ApplyAOtoVert(n, 3));
//       mesh->AddVertex(vec3(blockPos.x + 1, blockPos.y + 1, blockPos.z), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//     }
// 
//     // Left
//     if (!world->blockTypes.IsBlockEnabled(world->GetBlock(blockPos + vec3i(-1, 0, 0))))
//     {
//       float light = 0.6f * blockInShadow(world, blockPos + vec3i(-1, 0, 0));
//       int tile = world->blockTypes.GetBlockTile(block, blockLeft);
//       uint8_t n = AONeighbourhood(world, blockPos, blockLeft);
//       // Triangle 1
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y, blockPos.z), vec3(0, ITS, tile), light * ApplyAOtoVert(n, 0));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y, blockPos.z + 1), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y + 1, blockPos.z), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//       // Triangle 2
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y, blockPos.z + 1), vec3(ITS, ITS, tile), light * ApplyAOtoVert(n, 2));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y + 1, blockPos.z + 1), vec3(ITS, 0, tile), light * ApplyAOtoVert(n, 3));
//       mesh->AddVertex(vec3(blockPos.x, blockPos.y + 1, blockPos.z), vec3(0, 0, tile), light * ApplyAOtoVert(n, 1));
//     }
//   }