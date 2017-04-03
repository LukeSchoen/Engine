#include "Region.h"
#include "StreamFile.h"
#include "ZLIB.h"

const uint32_t REGION_CHUNK_WIDTH = 32;
const uint32_t REGION_CHUNK_HEIGHT = 1;
const uint32_t REGION_CHUNK_DEPTH = 32;
const uint32_t SECTOR_SIZE = 4096;

struct ChunkHeader
{
  uint32_t fileOffset;
  uint32_t chunkSize;
};

static char RegionPath[260];
static ChunkHeader RegionHeader[4096];
static const unsigned long chunkBufferLength = 1024 * 1024; // 1mb
static uint8_t compressedChunkBuffer[chunkBufferLength];
static uint8_t uncompressedChunkBuffer[chunkBufferLength];

void SwitchEndian(void* data, uint32_t byteCount)
{
  uint8_t* pNewData = new uint8_t[byteCount];
  for (int i = 0; i < byteCount; i++)
    pNewData[i] = ((uint8_t*)data)[byteCount - 1 - i];
  memcpy(data, pNewData, byteCount);
  delete[] pNewData;
}

bool LoadChunkFromRegion(Chunk *chunk, int32_t chunkX, int32_t chunkZ, const char *worldDirectory)
{
  // Empty the chunk
  memset(chunk->blockData, 0, Chunk::width * Chunk::height * Chunk::length);

  // Calculate region file
  int32_t regionX = (int32_t)floor(chunkX / (float)REGION_CHUNK_WIDTH);
  int32_t regionZ = (int32_t)floor(chunkZ / (float)REGION_CHUNK_WIDTH);
  sprintf_s(RegionPath, "%sr.%i.%i.mca", worldDirectory, regionX, regionZ);
  
  if (!File::FileExists(RegionPath)) return false;

  StreamFileReader regionFIle(RegionPath, nullptr, 16); // Open Region

  int regionChunkID = chunkX % REGION_CHUNK_WIDTH + (chunkZ % REGION_CHUNK_DEPTH) * REGION_CHUNK_WIDTH;

  uint32_t cOffset = 0;
  uint32_t cSize = 0;
  regionFIle.SetLocation(regionChunkID * 4);
  regionFIle.ReadBytes(&cOffset, 3);
  regionFIle.ReadBytes(&cSize, 1);
  cOffset = (((cOffset >> 0) & 0xFF) << 16) | (((cOffset >> 8) & 0xFF) << 8) | (((cOffset >> 16) & 0xFF) << 0);
  RegionHeader[regionChunkID].fileOffset = cOffset * SECTOR_SIZE;
  RegionHeader[regionChunkID].chunkSize = cSize * SECTOR_SIZE;

  // Read compressed chunk
  int fileLoc = RegionHeader[regionChunkID].fileOffset;
  if (fileLoc <= 0) return false;
  regionFIle.SetLocation(fileLoc);
  regionFIle.ReadBytes(compressedChunkBuffer, RegionHeader[regionChunkID].chunkSize);

  // Decompress chunk
  uint32_t exactChunkSize = *(uint32_t*)compressedChunkBuffer;
  SwitchEndian(&exactChunkSize, 4);
  unsigned long newLength = chunkBufferLength;
  if (MZ_OK == mz_uncompress((unsigned char*)uncompressedChunkBuffer, &newLength, compressedChunkBuffer + 5, exactChunkSize))
  {
    Chunk::ParseChunk(chunk, uncompressedChunkBuffer, newLength);
    return true;
  }
  return false; // else fail
}
