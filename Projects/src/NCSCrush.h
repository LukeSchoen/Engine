#ifndef ncsCrush_h__
#define ncsCrush_h__

#include "NovaCosmModel.h"

struct ncsCrush
{
  static NovaCosmBlock *LoadBlock(StreamFileReader &inputStream, int64_t discLocation)
  {
    NovaCosmBlock *block = new NovaCosmBlock;
    block->discLocation = discLocation;
    inputStream.SetLocation(discLocation);
    memset(&block->childPtr, 0, sizeof(block->childPtr));
    inputStream.ReadBytes(block->children, sizeof(block->children));
    inputStream.ReadBytes(&block->position, sizeof(block->position));
    inputStream.ReadBytes(&block->voxelCount, sizeof(block->voxelCount));
    int64_t voxelDataSize = block->voxelCount * sizeof(voxel);
    block->voxelPosData = (uint8_t*)malloc(voxelDataSize);
    inputStream.ReadBytes(block->voxelPosData, voxelDataSize);
    return block;
  }

  static void RecursiveCrush(StreamFileReader &inputStream, StreamFileWriter &outputStream, NovaCosmBlock *block)
  {
    // Copy block to output
    outputStream.SetLocation(block->discLocation);
    outputStream.WriteBytes(block->children, sizeof(block->children));
    outputStream.WriteBytes(&block->position, sizeof(block->position));
    outputStream.WriteBytes(&block->voxelCount, sizeof(block->voxelCount));
    // Crush block voxels
    voxel *oldOrder = (voxel*)block->voxelPosData;
    voxel *newOrder = (voxel*)malloc(block->voxelCount * sizeof(voxel));
    voxel me = oldOrder[0];
    voxel him;
    for (int64_t m = 0; m < block->voxelCount; m++)
    {
      newOrder[m] = me;
      int bestDist = 256 + 256 + 256;
      int64_t bestH;
      // Find closest voxel
      for (int64_t h = 0; h < block->voxelCount; h++)
      {
        if (m == h) continue;
        him = oldOrder[m];
        if ((him.x + him.y + him.z + him.r + him.g + him.b) == 0) continue;
        int dist = abs(me.x - him.x) + abs(me.y - him.y) + abs(me.z - him.z);
        if (dist < bestDist)
        {
          bestDist = dist;
          bestH = h;
        }
      }
      if (bestDist < 256 + 256 + 256)
      {
        me = him;
        him = { 0,0,0,0,0,0 };
      }
    }
    outputStream.WriteBytes(newOrder, sizeof(voxel) * block->voxelCount);
    free(newOrder);

    static int c = 0;
    c++;
    printf("crushed block %d\n", c);


    // Crush child blocks
    for (uint8_t cItr = 0; cItr < 8; cItr++)
    {
      if (block->childPtr[cItr])
      {
        RecursiveCrush(inputStream, outputStream, block->childPtr[cItr]);
      }
      else
      {
        int64_t childAddress = block->children[cItr];
        if (childAddress > 0)
        {
          block->childPtr[cItr] = LoadBlock(inputStream, childAddress);
          RecursiveCrush(inputStream, outputStream, block->childPtr[cItr]);
        }
      }
    }
  }

  static void CrushNCS(const char *inputFile, const char *outputFile)
  {
    StreamFileReader inputStream(inputFile);
    StreamFileWriter outputStream(outputFile);
    NovaCosmBlock *root = LoadBlock(inputStream, 0);
    RecursiveCrush(inputStream, outputStream, root);
  }
};


#endif // ncsCrush_h__
