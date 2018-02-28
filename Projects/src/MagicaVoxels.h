#ifndef MagicaVoxels_h__
#define MagicaVoxels_h__

#include <tuple>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "Maths.h"
#include "StreamFile.h"

class MV_RGBA
{
public:
  MV_RGBA() { r = g = b = a = 0; }
  unsigned char r, g, b, a;
};

class MV_Voxel
{
public:
  unsigned char x, y, z, colorIndex;
public:
  MV_Voxel() { x = y = z = colorIndex = 0; }
};


class MV_Model
{
public:
  int version;
  vec3i size;
  std::vector< MV_Voxel > voxels;
  MV_RGBA palette[256];

  MV_Model();

  bool LoadModel(const char *path);
protected:

  void Free(void);
  struct chunk_t
  {
    int id;
    int contentSize;
    int childrenSize;
    int end;
    char header[4];
  };

  bool ReadModelFile(const char *path);

  void ReadChunk(StreamFileReader &file, chunk_t &chunk);
};

std::vector<std::tuple<vec3, uint32_t>> ReadVoxFile(std::string filePath);

#endif // MagicaVoxels_h__
