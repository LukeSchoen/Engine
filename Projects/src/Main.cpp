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
#include <vector>
#include "MergeSort.h"
#include "Sims.h"
#include "WireWars.h"
#include "BioMorphs.h"

//void GetScreen() // face detection and so forth, mwa
//{
//  // get screen DC and memory DC to copy to
//  HDC hScreenDC = GetDC(0);
//  HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
//
//  // create a DIB to hold the image
//  BITMAPINFO bmi = { 0 };
//  bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
//  bmi.bmiHeader.biWidth = GetDeviceCaps(hScreenDC, HORZRES);
//  bmi.bmiHeader.biHeight = -GetDeviceCaps(hScreenDC, VERTRES);
//  bmi.bmiHeader.biPlanes = 1;
//  bmi.bmiHeader.biBitCount = 32;
//  LPVOID pBits;
//  HBITMAP hBitmap = CreateDIBSection(hMemoryDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
//
//  // select new bitmap into memory DC
//  HGDIOBJ hOldBitmap = SelectObject(hMemoryDC, hBitmap);
//
//  // copy from the screen to memory
//  BitBlt(hMemoryDC, 0, 0, bmi.bmiHeader.biWidth, -bmi.bmiHeader.biHeight, hScreenDC, 0, 0, SRCCOPY);
//
//  // Do work here.
//
//  // clean up
//  SelectObject(hMemoryDC, hOldBitmap);
//  DeleteDC(hMemoryDC);
//  ReleaseDC(0, hScreenDC);
//
//  // the image data is now in pBits in 32-bpp format
//  // free this when finished using DeleteObject(hBitmap);
//}

void main() // console entry
{
  //WireWars();
  //Lua();
  //Grass();
  //PixeLife();
  //DLux();
  //Pokemon();
  //BTCBot();
  //SecondStream();
  //StreamTest();
  //Glorious();
  //NovaCosm();
  //DrawTree();
  //Cosmic();
  //Zelda();
  //PhysicsTest();
  //Minecraft();
  //OpenGL();
  //Compression();
  //ModelView();
  //MarchingCubes();
  //Sims();
  BioMorphs();
}

int wmain(int pc, char* ps){main();} // windows entry
 