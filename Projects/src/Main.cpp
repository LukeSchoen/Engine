#include "Glorious.h"
#include "Zelda.h"
#include "Cosmic.h"
#include "NovaCosm.h"
#include "SecondStream.h"
#include "StreamTest.h"
#include "Minecraft.h"
#include "BTCBot.h"
#include "pokemon.h"
#include "DLux.h"
#include "OpenGL.h"
#include "Compression.h"
#include "PhysicsTest.h"
#include "ModelView.h"
#include "MarchingCubes.h"
#include "Grass.h"
#include "PixelLife.h"
#include "DrawTree.h"
#include "NN.h"

#include <vector>
#include "MergeSort.h"
int64_t FindNextComma(char *pData, int64_t i, int64_t size)
{
 for (int64_t c = i; c < size; c++)
  if (pData[c] == ',')
   return c + 1;
 return i + 1;
}

int64_t FindNextLine(char *pData, int64_t i, int64_t size)
{
 for (int64_t c = i; c < size; c++)
  if (pData[c] == '\n')
   return c + 1;
 return i + 1;
}


void main() // console entry
{
 //{
 // int64_t fileSize;
 // StreamFileReader file("D:/UserFiles/Desktop/.btceUSD.csv");
 // StreamFileWriter fileOut("D:/UserFiles/Desktop/block.x");
 // char *pData = (char*)file.ReadToEnd(&fileSize);

 // for (int64_t i = 0; i < fileSize; i)
 // {
 //  int64_t timeStamp = atoi(pData + i);
 //  i = FindNextComma(pData, i, fileSize);
 //  float price = atof(pData + i);
 //  i = FindNextComma(pData, i, fileSize);
 //  float volume = atof(pData + i);
 //  i = FindNextLine(pData, i, fileSize);
 //  fileOut.WriteBytes(&timeStamp, sizeof(timeStamp));
 //  fileOut.WriteBytes(&price, sizeof(price));
 //  fileOut.WriteBytes(&volume, sizeof(volume));
 // }
 //}
//   std::vector<int> data = {5,6,9,1,3,7,2};
//   for (auto & num : data) printf("%d", num);
//   printf("\n\n");
//   data = Sorter::MergeSort(data);
//   for (auto & num : data) printf("%d", num);
//   getchar();

  //Lua();
  //Grass();
  //PixeLife();
  //DLux();
  //Pokemon();
  //BTCBot();
  //SecondStream();
  //StreamTest();
  //Glorious();
  //DrawTree();
  //NovaCosm();
  //Cosmic();
  //Zelda();
  //PhysicsTest();
  //Minecraft();
 NeuralNetTest();
  //OpenGL();
  //Compression();
  //ModelView();
  //MarchingCubes();
}

int wmain(int pc, char* ps){main();} // windows entry
 