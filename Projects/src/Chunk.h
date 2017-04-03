#ifndef Chunk_h__
#define Chunk_h__
#include "File.h"

struct Chunk
{
  static const int width = 16;
  static const int length = 16;
  static const int height = 256;
  static void ParseChunk(Chunk *chunk, uint8_t *chunkData, uint32_t dataSize);

  uint8_t GetBlock(int x, int y, int z);
  void SetBlock(int x, int y, int z, uint8_t blockID);

  uint8_t blockData[width * length * height];
};


#endif // Chunk_h__
