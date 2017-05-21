#include "BlockTypes.h"

MinecraftBlockTypes::MinecraftBlockTypes()
{
  //stone
  AddSolidBlockType(1, 1);
  //grass
  AddSolidBlockType(2, 0, 2, 3, 3, 3, 3);
  //dirt
  AddSolidBlockType(3, 2);
  //Cobble Stone
  AddSolidBlockType(4, 16);
  //Planks
  AddSolidBlockType(5, 4);
  //Bedrock
  AddSolidBlockType(7, 17);

  //water
  AddAlphaBlockType(8, liquid, false, 205);
  AddAlphaBlockType(9, liquid, false, 205);

  //Lava
  AddAlphaBlockType(10, liquid, false, 237);
  AddAlphaBlockType(11, liquid, false, 237);

  //Sand
  AddSolidBlockType(12, 18);
  //Gravel
  AddSolidBlockType(13, 19);
  //Gold
  AddSolidBlockType(14, 32);
  //Iron
  AddSolidBlockType(15, 33);
  //Coal
  AddSolidBlockType(16, 34);
  //Wood
  AddSolidBlockType(17, 20);
  //leaves
  AddAlphaBlockType(18, cube, true, 52);

  //Glass
  AddAlphaBlockType(20, cube, true, 49);

  //Lapis ore
  AddSolidBlockType(21, 160);
  //Lapis block
  AddSolidBlockType(22, 144);
  
  //Dispenser
  AddSolidBlockType(23, 62, 62, 46, 45, 45, 45);

  //Bed
  AddAlphaBlockType(26, cube, true, 135);

  //Smooth sand stone
  AddSolidBlockType(24, 176);

  //Note Block
  AddSolidBlockType(25, 74, 74, 74, 74, 74, 74);

  //Powered rail
  AddSolidBlockType(27, 179);

  //Tall grass
  AddAlphaBlockType(31, cross, false, 39);

  //Wool
  AddSolidBlockType(35, 64);

  //Dandelions
  AddAlphaBlockType(37, cross, false, 13);
  //Roses
  AddAlphaBlockType(38, cross, false, 12);
  //Brown Mushrooms
  AddAlphaBlockType(39, cross, false, 29);
  //Red Mushrooms
  AddAlphaBlockType(40, cross, false, 28);
  //Gold block
  AddSolidBlockType(41, 23);
  //Iron Block
  AddSolidBlockType(42, 22);
  //double Slab
  AddSolidBlockType(43, 5);

  //Slab
  AddAlphaBlockType(44, cube, true, 5);

  //Bricks
  AddSolidBlockType(45, 7);

  //TNT
  AddSolidBlockType(46, 8);

  //book shelf
  AddSolidBlockType(47, 35);

  //Mossy CobbleStone
  AddSolidBlockType(48, 36);

  //Obsidian
  AddSolidBlockType(49, 37);

  //Torch
  AddAlphaBlockType(50, cross, false, 80);

  //Fire
  AddSolidBlockType(51, 47);



  //Mob Spawner
  AddSolidBlockType(52, 65);

  //wood stairs
  AddSolidBlockType(53, 4);

  //Chest
  AddSolidBlockType(54, 25);

  //Diamond Ore
  AddSolidBlockType(56, 50);

  //Diamond Block
  AddSolidBlockType(57, 24);

  //Crafting table
  AddSolidBlockType(58, 43, 4, 60, 60, 59, 59);

  //Wheat Crop
  AddAlphaBlockType(59, cross, false, 95);

  //Farm Land
  AddSolidBlockType(60, 86);

  //Furnace off
  AddSolidBlockType(61, 62, 62, 43, 45, 45, 45);

  //Furnace on
  AddSolidBlockType(62, 62, 62, 60, 45, 45, 45);

  //Standing Sign
  AddSolidBlockType(63, 4);

  //Stone Stairs
  AddSolidBlockType(67, 16);

  //Wood Door
  AddSolidBlockType(64, 97);
  
  //Ladder
  AddAlphaBlockType(65, cube, false, 83);
  
  //Rail
  AddAlphaBlockType(66, plane, false, 128);

  //Wall Sign
  AddSolidBlockType(68, 4);

  //Lever
  AddAlphaBlockType(69, cross, false, 16);

  //Stone Pressure plate
  AddAlphaBlockType(70, plane, false, 1);

  //Wood pressure plate
  AddAlphaBlockType(72, plane, false, 4);

  //Redstone Ore
  AddSolidBlockType(73, 51);

  //Snow
  AddSolidBlockType(78, 66);

  //Redstone Torch
  AddAlphaBlockType(76, cross, false, 99);

  //Stone button
  AddAlphaBlockType(77, cube, false, 1);

  //Ice
  AddSolidBlockType(79, 67);
  //Cactus
  AddSolidBlockType(81, 69);
  //Clay
  AddSolidBlockType(82, 72);
  //Sugar Cane
  AddAlphaBlockType(83, cube, true, 73);

  //Juke box
  AddSolidBlockType(84, 75, 74, 74, 74, 74, 74);
  //Wood Fence
  AddSolidBlockType(85, 4);
  //Pumpkin
  AddSolidBlockType(86, 102);

  // Nether rack
  AddSolidBlockType(87, 104);

  // Soul Sand 
  AddSolidBlockType(88, 105);

  //Glow stone
  AddSolidBlockType(89, 105);

  //Cake
  AddSolidBlockType(92, 121);
}

void MinecraftBlockTypes::AddSolidBlockType(uint8_t blockID, int topTex, int botTex, int frontTex, int backTex, int rightTex, int leftTex)
{
  botTex   = botTex   == -1 ? topTex : botTex;
  frontTex = frontTex == -1 ? topTex : frontTex;
  backTex  = backTex  == -1 ? topTex : backTex;
  rightTex = rightTex == -1 ? topTex : rightTex;
  leftTex  = leftTex  == -1 ? topTex : leftTex;

  blockData[blockID].used = true;
  blockData[blockID].solid = true;
  blockData[blockID].alpha = false;
  blockData[blockID].model = cube;

  blockData[blockID].tile[0] = topTex;
    blockData[blockID].tile[1] = botTex;
    blockData[blockID].tile[2] = frontTex;
    blockData[blockID].tile[3] = backTex;
    blockData[blockID].tile[4] = rightTex;
    blockData[blockID].tile[5] = leftTex;
}




void MinecraftBlockTypes::AddAlphaBlockType(uint8_t blockID, modelType model, bool solid, int topTex, int botTex /*= -1*/, int frontTex /*= -1*/, int backTex /*= -1*/, int leftTex /*= -1*/, int rightTex /*= -1*/)
{
  botTex = botTex == -1 ? topTex : botTex;
  frontTex = frontTex == -1 ? topTex : frontTex;
  backTex = backTex == -1 ? topTex : backTex;
  rightTex = rightTex == -1 ? topTex : rightTex;
  leftTex = leftTex == -1 ? topTex : leftTex;

  blockData[blockID].used = true;
  blockData[blockID].alpha = true;
  blockData[blockID].solid = solid;
  blockData[blockID].model = model;

  blockData[blockID].tile[0] = topTex;
  blockData[blockID].tile[1] = botTex;
  blockData[blockID].tile[2] = frontTex;
  blockData[blockID].tile[3] = backTex;
  blockData[blockID].tile[4] = rightTex;
  blockData[blockID].tile[5] = leftTex;
}

int MinecraftBlockTypes::GetBlockTile(uint8_t blockID, int side)
{
  return blockData[blockID].tile[side];
}

bool MinecraftBlockTypes::IsBlockEnabled(uint8_t blockID)
{
  //if (blockID > 0) if (!blockData[blockID].used) printf("unknown %d\n", blockID); // Report unknwon blocks
  return blockData[blockID].used;
}

bool MinecraftBlockTypes::IsBlockAlphaed(uint8_t blockID)
{
  return blockData[blockID].alpha;
}

bool MinecraftBlockTypes::IsBlockSolid(uint8_t blockID)
{
  return blockData[blockID].solid;
}

modelType MinecraftBlockTypes::GetBlockModel(uint8_t blockID)
{
  return blockData[blockID].model;
}
