#ifndef ChunkHandler_h__
#define ChunkHandler_h__
#include "Pool.h"
#include "ChunkInstance.h"

struct ChunkHandler : private Pool
{
  ChunkHandler(int maxLoadedChunks = 65536);

  ~ChunkHandler();

  ChunkInstance *GetChunkInstance(int chunkID);

  int FindChunk(int xPos, int zPos);

  void DeleteChunk(int chunkID);

  int AddChunk(int xPos, int zPos);

private:
  int _maxLoadedChunks;
  ChunkInstance *data;
  int *chunkLookup;
  const int chunkLookupWidth = 1024;
};

#endif // ChunkHandler_h__