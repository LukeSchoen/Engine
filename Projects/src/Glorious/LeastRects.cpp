#include "LeastRects.h"
#include <stdio.h>
#include "Vector.h"
#include <memory.h>
#include "Maths.h"

Vector rects(sizeof(Rect));
int *constrastX;
int *constrastY;
int width;
int height;
bool *image;

void AddRect(Rect r)
{
  rects.Add(&r);
}

bool CellIsOn(int x, int y)
{

  if (x < 0 | x >= width | y < 0 | y >= height) return false;
  else return image[x + y * width];
}

void CardinalitySimplification(int maxNeighbourCount)
{
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
    {
      int neighbours = 0;
      for (int ny = -1; ny < 2; ny++)
        for (int nx = -1; nx < 2; nx++)
          if (!(nx == 0 & ny == 0))
            if (CellIsOn(x + nx, y + ny))
              neighbours++;
      if (neighbours > maxNeighbourCount)
        image[x + y * width] = true;
    }
}

bool CellInRect(int x, int y, Rect &rect)
{
  if ((x >= rect.xpos & x < rect.xpos + rect.width) & (y >= rect.ypos & y < rect.ypos + rect.height)) return true;
  return false;

}

bool HomeomorphicMerger(int excessAllowance, bool overlapAllowed)
{
  bool success = false;
  for (int sliceItr = 0; sliceItr < rects.count; sliceItr++)
  {
    for (int otherSliceItr = sliceItr + 1; otherSliceItr < rects.count; otherSliceItr++)
    {
      Rect *thisSlice = (Rect*)rects.Get(sliceItr);
      Rect *thatSlice = (Rect*)rects.Get(otherSliceItr);

      // Attempt a non-isomorphic combination
      int currentArea = thisSlice->width * thisSlice->height + thatSlice->width * thatSlice->height;
      int combinedXpos = Min(thisSlice->xpos, thatSlice->xpos);
      int combinedYpos = Min(thisSlice->ypos, thatSlice->ypos);
      int combinedWidth = Max(thisSlice->xpos + thisSlice->width, thatSlice->xpos + thatSlice->width) - combinedXpos;
      int combinedHeight = Max(thisSlice->ypos + thisSlice->height, thatSlice->ypos + thatSlice->height) - combinedYpos;
      int combinedArea = combinedWidth * combinedHeight;

      // Would it increase the overdraw too much ?
      if ((combinedArea - currentArea) > excessAllowance)
        continue;

      if (!overlapAllowed)
      {
        // Would it overlap another rectangle?
        bool overlap = false;
        for (int y = combinedYpos; y < combinedYpos + combinedHeight; y++)
          for (int x = combinedXpos; x < combinedXpos + combinedWidth; x++)
            if (!CellInRect(x, y, *thisSlice) && !CellInRect(x, y, *thatSlice))
              if(image[x + y * width])
                overlap = true;
        if (overlap)
          continue;
      }

      // If that's all good then okay let's merge!
      rects.Del(otherSliceItr);
      thisSlice->xpos = combinedXpos;
      thisSlice->ypos = combinedYpos;
      thisSlice->width = combinedWidth;
      thisSlice->height = combinedHeight;
      success = true;
      // Claim new area
      for (int y = combinedYpos; y < combinedYpos + combinedHeight; y++)
        for (int x = combinedXpos; x < combinedXpos + combinedWidth; x++)
          image[x + y * width] = true;
      break;
    }
  }
  return success;
}

bool IsomorphicMerger()
{
  bool success = false;
  for (int sliceItr = 0; sliceItr < rects.count; sliceItr++)
  {
    bool merging = true;
    while (merging)
    {
      merging = false;
      for (int otherSliceItr = sliceItr + 1; otherSliceItr < rects.count; otherSliceItr++)
      {
        Rect *thisSlice = (Rect*)rects.Get(sliceItr);
        Rect *thatSlice = (Rect*)rects.Get(otherSliceItr);

        // Merge Down
        if (thisSlice->xpos == thatSlice->xpos && thisSlice->width == thatSlice->width && thatSlice->ypos == thisSlice->ypos + thisSlice->height)
        {
          merging = true;
          success = true;
          thisSlice->height += thatSlice->height;
          rects.Del(otherSliceItr);
          break;
        }

        // Merge Right
        if (thisSlice->ypos == thatSlice->ypos && thisSlice->height == thatSlice->height && thatSlice->xpos == thisSlice->xpos + thisSlice->width)
        {
          merging = true;
          success = true;
          thisSlice->width += thatSlice->width;
          rects.Del(otherSliceItr);
          break;
        }

        // Merge Up
        if (thisSlice->xpos == thatSlice->xpos && thisSlice->width == thatSlice->width && thisSlice->ypos == thatSlice->ypos + thatSlice->height)
        {
          merging = true;
          success = true;
          thisSlice->ypos = thatSlice->ypos;
          thisSlice->height += thatSlice->height;
          rects.Del(otherSliceItr);
          break;
        }

        // Merge Left
        if (thisSlice->ypos == thatSlice->ypos && thisSlice->height == thatSlice->height && thisSlice->xpos == thatSlice->xpos + thatSlice->width)
        {
          merging = true;
          success = true;
          thisSlice->xpos = thatSlice->xpos;
          thisSlice->width += thatSlice->width;
          rects.Del(otherSliceItr);
          break;
        }
      }
    }
  }
  return success;
}

void ContrastSlice(int xpos, int ypos, int xpos2, int ypos2)
{
  // Find Highest Contrast
  int bestSplitContrast = 0;
  int bestSplitPosition;
  int bestSplitAXIS; // 0=x 1=y

  // Search x
  for (int x = xpos; x < xpos2 - 1; x++)
    if (constrastX[x] > bestSplitContrast)
    {
      bestSplitContrast = constrastX[x];
      bestSplitPosition = x+1;
      bestSplitAXIS = 0;
    }

  // Search y
  for (int y = ypos; y < ypos2 - 1; y++)
    if (constrastY[y] > bestSplitContrast)
    {
      bestSplitContrast = constrastY[y];
      bestSplitPosition = y+1;
      bestSplitAXIS = 1;
    }

  if (bestSplitContrast > 0) // Branches
  {
    if (bestSplitAXIS == 0)
    { // X
      ContrastSlice(xpos, ypos, bestSplitPosition, ypos2);
      ContrastSlice(bestSplitPosition, ypos, xpos2, ypos2);
    }
    else
    { // Y
      ContrastSlice(xpos, ypos, xpos2, bestSplitPosition);
      ContrastSlice(xpos, bestSplitPosition, xpos2, ypos2);
    }
  }
  else // leaves
  {
    if (image[xpos + ypos * width]) // Homogeneous solid
      AddRect(Rect(xpos, ypos, xpos2 - xpos, ypos2 - ypos));
  }
}

Rect *LeastRects::FindRectsInImage(bool *_image, int _width, int _height, int *rectCount)
{
  // Localise Data
  image = _image;
  width = _width;
  height = _height;

  int oons = 0;
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      if (image[x + y * width]) oons++;

  // Fill in troublesome missing cells
  //CardinalitySimplification(5);
  //CardinalitySimplification(4);

  // Count Contrast
  constrastX = new int[width - 1];
  constrastY = new int[height - 1];
  memset(constrastX, 0, sizeof(int) * (width - 1));
  memset(constrastY, 0, sizeof(int) * (height - 1));
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
    {
      if ((x < width - 1) && (image[x + y * width] ^ image[(x + 1) + y * width])) constrastX[x]++;
      if ((y < height - 1) && (image[x + y * width] ^ image[x + (y + 1) * width])) constrastY[y]++;
    }

  // Split
  ContrastSlice(0, 0, width, height);

  // Merge Direct Neighbors
  while (IsomorphicMerger());

  // Merge Indirect Neighbors
  while(HomeomorphicMerger(64, true));

  // Print Info
  int ons = 0;
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      if (image[x + y * width]) ons++;

  printf("Original pixel Count: %d, Final Pixel Count %d\n", oons, ons);
  printf("Rects: %d, Geometric Efficiency Score: %f\n", rects.count, (float)oons / rects.count);
  printf("Alpha: %d, Fragments Efficiency Score: %f\n", ons - oons, (float)oons / ons);

  //getchar();

  // Cleanup temporary data and return rects
  Rect *foundRects = new Rect[rects.count];
  memcpy(foundRects, rects.Data(), rects.count * sizeof(Rect));
  *rectCount = rects.count;
  rects.Clear();
  delete[] constrastX;
  delete[] constrastY;
  return foundRects;
}