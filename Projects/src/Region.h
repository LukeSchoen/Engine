#ifndef Region_h__
#define Region_h__

#include "Chunk.h"
#include <stdint.h>

bool LoadChunkFromRegion(Chunk *chunk, int32_t chunkX, int32_t chunkZ, const char *worldDirectory);

#endif // Region_h__