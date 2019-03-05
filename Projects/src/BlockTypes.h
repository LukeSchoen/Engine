#ifndef BlockTypes_h__
#define BlockTypes_h__
#include "maths.h"

enum blockSide
{
  blockTop = 0,
  blockBot = 1,
  blockFront = 2,
  blockBack = 3,
  blockRight = 4,
  blockLeft = 5,
};

enum modelType : int8_t
{
  cube,
  cross,
  liquid,
  plane,
};

struct BlockData
{
  bool used = false;
  bool alpha = true;
  modelType model = cube;
  int tile[6];
  bool solid = false;
};

struct MinecraftBlockTypes
{

  MinecraftBlockTypes();

  void AddSolidBlockType(uint8_t blockID, int topTex, int botTex = -1, int frontTex = -1, int backTex = -1, int leftTex = -1, int rightTex = -1);

  void AddAlphaBlockType(uint8_t blockID, modelType model, bool solid, int topTex, int botTex = -1, int frontTex = -1, int backTex = -1, int leftTex = -1, int rightTex = -1);

  int GetBlockTile(uint8_t blockID, int side);

  bool IsBlockEnabled(uint8_t blockID);

  bool IsBlockAlphaed(uint8_t blockID);

  bool IsBlockSolid(uint8_t blockID);

  modelType GetBlockModel(uint8_t blockID);

  BlockData blockData[256];

};

#endif // BlockTypes_h__
