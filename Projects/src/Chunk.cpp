#include "Chunk.h"


// Optimized implementation
uint8_t Chunk::GetBlock(int x, int y, int z)
{
  return blockData[x + (z << 4) + (y << 8)];
}

// Reference implementation
//uint8_t Chunk::GetBlock(int x, int y, int z)
//{
//  if ((x < 0 | y < 0 | z < 0) | (x >= Chunk::width | y >= Chunk::height | z >= Chunk::length))
//    return 0;
//  return blockData[x + z * Chunk::width + y * Chunk::width * Chunk::length];
//}

void Chunk::SetBlock(int x, int y, int z, uint8_t blockID)
{
  if ((x < 0 | y < 0 | z < 0) | (x >= Chunk::width | y >= Chunk::height | z >= Chunk::length))
    return;
  blockData[x + z * Chunk::width + y * Chunk::width * Chunk::length] = blockID;
}

void Chunk::ParseChunk(Chunk *chunk, uint8_t *chunkData, uint32_t dataSize)
{
  //Find Block Data
  int BlockStart = 0;
  int section = 0;
  for (int i = 0; i < dataSize; i++)
    if (chunkData[i] == 'B')
      if ((chunkData[i + 1] == 'l') & (chunkData[i + 2] == 'o') & (chunkData[i + 3] == 'c') & (chunkData[i + 4] == 'k') & (chunkData[i + 5] == 's'))
        memcpy(chunk->blockData + (section++ * 16 * 16 * 16), chunkData + i + 10, 16 * 16 * 16);
}
