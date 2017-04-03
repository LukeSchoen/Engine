#include "ChunkHandler.h"
#include "ChunkInstance.h"

ChunkHandler::ChunkHandler(int maxLoadedChunks /*= 65536*/)
{
  _maxLoadedChunks = maxLoadedChunks;
  data = new ChunkInstance[maxLoadedChunks];
  chunkLookup = new int[chunkLookupWidth * chunkLookupWidth];
  for (int i = 0; i < chunkLookupWidth * chunkLookupWidth; i++) chunkLookup[i] = -1;
}

ChunkHandler::~ChunkHandler()
{
  delete[] data;
  delete[] chunkLookup;
}

ChunkInstance * ChunkHandler::GetChunkInstance(int chunkID)
{
  return &data[chunkID];
}

int ChunkHandler::FindChunk(int xPos, int zPos)
{
  return chunkLookup[xPos + (chunkLookupWidth / 2) + (zPos + (chunkLookupWidth / 2)) * chunkLookupWidth];
}

void ChunkHandler::DeleteChunk(int chunkID)
{
  ChunkInstance &c = *GetChunkInstance(chunkID);
  chunkLookup[c.xPos + (chunkLookupWidth / 2) + (c.zPos + (chunkLookupWidth / 2)) * chunkLookupWidth] = -1;
  Old(chunkID);
}

int ChunkHandler::AddChunk(int xPos, int zPos)
{
  int newChunkID = New();
  chunkLookup[xPos + (chunkLookupWidth / 2) + (zPos + (chunkLookupWidth / 2)) * chunkLookupWidth] = newChunkID;
  data[newChunkID].xPos = xPos;
  data[newChunkID].zPos = zPos;
  data[newChunkID].lifeTime = 0.05f;
  return newChunkID;
}
