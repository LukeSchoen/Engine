#include "BlockTypes.h"

MinecraftBlockTypes::MinecraftBlockTypes()
{
  //stone
  AddBlockType(1, 1);
  //grass
  AddBlockType(2, 0, 2, 3, 3, 3, 3);
  //dirt
  AddBlockType(3, 2);
  //Cobble Stone
  AddBlockType(4, 16);
  //Planks
  AddBlockType(5, 4);
  //Bedrock
  AddBlockType(7, 17);
  //water
  AddBlockType(8, 205);
  AddBlockType(9, 205);

  //Lava
  AddBlockType(10, 237);
  AddBlockType(11, 237);

  //Sand
  AddBlockType(12, 18);
  //Gravel
  AddBlockType(13, 19);
  //Gold
  AddBlockType(14, 32);
  //Iron
  AddBlockType(15, 33);
  //Coal
  AddBlockType(16, 34);
  //Wood
  AddBlockType(17, 20);
  //leaves
  AddBlockType(18, 52);


  //Glass
  AddBlockType(20, 49);

  //Lapis ore
  AddBlockType(21, 160);
  //Lapis block
  AddBlockType(22, 144);
  
  //Dispenser
  AddBlockType(23, 62, 62, 46, 45, 45, 45);

  //Bed
  AddBlockType(26, 135);

  //Smooth sand stone
  AddBlockType(24, 176);

  //Note Block
  AddBlockType(25, 74, 74, 74, 74, 74, 74);

  //Powered rail
  AddBlockType(27, 179);

  //Tall grass
  AddBlockType(31, 39);

  //Wool
  AddBlockType(35, 64);

  //Dandelions
  AddBlockType(37, 13);
  //Roses
  AddBlockType(38, 12);
  //Brown Mushrooms
  AddBlockType(39, 29);
  //Brown Mushrooms
  AddBlockType(40, 28);
  //Gold block
  AddBlockType(41, 23);
  //Iron Block
  AddBlockType(42, 22);
  //double Slab
  AddBlockType(43, 5);
  //Slab
  AddBlockType(44, 5);

  //Bricks
  AddBlockType(45, 7);

  //TNT
  AddBlockType(46, 8);

  //book shelf
  AddBlockType(47, 35);

  //Mossy CobbleStone
  AddBlockType(48, 36);
  //Obsidian
  AddBlockType(49, 37);

  //Torch
  AddBlockType(50, 80);

  //Fire
  AddBlockType(51, 47);



  //Mob Spawner
  AddBlockType(52, 65);

  //wood stairs
  AddBlockType(53, 4);

  //Chest
  AddBlockType(54, 25);

  //Diamond Ore
  AddBlockType(56, 50);

  //Diamond Block
  AddBlockType(57, 24);

  //Crafting table
  AddBlockType(58, 43, 4, 60, 60, 59, 59);

  //Wheat Crop
  AddBlockType(59, 95);

  //Farm Land
  AddBlockType(60, 86);

  //Furnace off
  AddBlockType(61, 62, 62, 43, 45, 45, 45);
  //Furnace on
  AddBlockType(62, 62, 62, 60, 45, 45, 45);

  //Standing Sign
  AddBlockType(63, 4);

  //Stone Stairs
  AddBlockType(67, 16);

  //Wood Door
  AddBlockType(64, 97);
  
  //Ladder
  AddBlockType(65, 83);
  
  //Rail
  AddBlockType(66, 128);

  //Wall Sign
  AddBlockType(68, 4);

  //Lever
  AddBlockType(69, 16);

  //Stone Pressure plate
  AddBlockType(70, 1);

  //Wood pressure plate
  AddBlockType(72, 4);

  //Redstone Ore
  AddBlockType(73, 51);
  //Snow
  AddBlockType(78, 66);

  //Redstone Torch
  AddBlockType(76, 99);

  //Stone button
  AddBlockType(77, 1);

  //Ice
  AddBlockType(79, 67);
  //Cactus
  AddBlockType(81, 69);
  //Clay
  AddBlockType(82, 72);
  //Sugar Cane
  AddBlockType(83, 73);

  //Juke box
  AddBlockType(84, 75, 74, 74, 74, 74, 74);
  //Wood Fence
  AddBlockType(85, 4);
  //Pumpkin
  AddBlockType(86, 102);

  // Nether rack
  AddBlockType(87, 104);

  // Soul Sand 
  AddBlockType(88, 105);

  //Glow stone
  AddBlockType(89, 105);

  //Cake
  AddBlockType(92, 121);
}

void MinecraftBlockTypes::AddBlockType(uint8_t blockID, int topTex, int botTex, int frontTex, int backTex, int rightTex, int leftTex)
{
  botTex   = botTex   == -1 ? topTex : botTex;
  frontTex = frontTex == -1 ? topTex : frontTex;
  backTex  = backTex  == -1 ? topTex : backTex;
  rightTex = rightTex == -1 ? topTex : rightTex;
  leftTex  = leftTex  == -1 ? topTex : leftTex;

  blockData[blockID].used = true;

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
