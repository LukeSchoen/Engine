#include "Rays.h"
#include "Time.h"
#include "Window.h"
#include "Controls.h"

void JamesLine(std::vector<vec2i> &pixels, uint32_t sx, uint32_t sy, uint32_t ex, uint32_t ey)
{
  int startX = sx; 
  int startY = sy;
  int endX = ex;
  int endY = ey;
  int i = 0;
  int swapped = false;
  int IncrValue, endVal;
  int YLen = endY - startY;
  int XLen = endX - startX;

  if (abs(YLen) > abs(XLen))
  {
    bool swapvar = YLen;
    YLen = XLen;
    XLen = swapvar;
    swapped = true;
  }

  endVal = XLen >> 1;

  if (XLen < 0)
  {
    IncrValue = -1;
    XLen = -XLen;
  }
  else IncrValue = 1;

  int IncrVar;
  if (XLen == 0) IncrVar = 0;
  else IncrVar = (YLen << 16) / XLen;

  int j = 0;
  if (swapped)
  {
    while (i <= endVal)
    {
      i += IncrValue;
      j += IncrVar;
      {
        pixels.emplace_back(startX + (j >> 16), startY + i);
        pixels.emplace_back(endX - (j >> 16), endY - i);
      }
    }

    while (i <= -endVal)
    {
      i += -IncrValue;
      j += -IncrVar;
      {
        pixels.emplace_back(startX - (j >> 16), startY - i);
        pixels.emplace_back(endX + (j >> 16), endY + i);
      }
    }
  }
  else
  {
    while (i <= endVal)
    {
      i += IncrValue;
      j += IncrVar;
      {
        pixels.emplace_back(startX + i, startY + (j >> 16));
        pixels.emplace_back(endX - i, endY - (j >> 16));
      }
    }

    while (i <= -endVal)
    {
      i += -IncrValue;
      j += -IncrVar;
      {
        pixels.emplace_back(startX - i, startY - (j >> 16));
        pixels.emplace_back(endX + i, endY + (j >> 16));
      }
    }
  }

}


void MagicLine2D(std::vector<vec2i> &pixels, uint32_t sx, uint32_t sy, uint32_t ex, uint32_t ey)
{
  float r = 0;
  r = ((ey - sy) / ((float)sx - ex));
  float y = sy;
  int x = (int)sx;
  int e = (int)ex;
  int d = ((e > x) << 1) - 1;
  while (x != e)
  {
    pixels.emplace_back(x, int(y));
    x += d;
    y += r * -d;
  }
}


void float_MagicLine2D(std::vector<vec2i> &pixels, float sx, float sy, float ex, float ey)
{
  float r = ((ey - sy) / (sx - ex));
  float y = sy;
  int x = (int)sx;
  int e = (int)ex;
  int d = ((e > x) << 1) - 1;
  while (x != e)
  {
    pixels.emplace_back(x, int(y));
    x += d;
    y += r * -d;
  }
}


void _MagicLine2D(std::vector<vec2i> &pixels, float sx, float sy, float ex, float ey)
{
  pixels.emplace_back(sx, sy);

  int outx = sx;
  int outy = sy;
  int *pOutx = &outx;
  int *pOuty = &outy;

  if (sx > ex) std::swap(sx, ex); // Soft to allow for x increments
  if (sy > ey) std::swap(sy, ey); // Sort to allow for y increments
  if (ey - sy > ex - sx) // Sort 
  {
    std::swap(sx, sy);
    std::swap(ex, ey);
    std::swap(pOutx, pOuty);
  }
  int d = ((ex > sx) << 1) - 1;
  float r = 0;
  if ((sy - sx) * d)
    r = (ey - sy) / (sy - sx) * d;

  float y = sy;
  int x = (int)sx;
  int e = (int)ex;
  while (x < e)
  {
    *pOutx = x++;
    *pOuty = int(y += r);
  }

  pixels.emplace_back(outx, outy);
}

void _MagicLine3D(std::vector<vec3i> &pixels, float sx, float sy, float ex, float ey)
{
  pixels.emplace_back(sx, sy);

  int outx = sx;
  int outy = sy;
  int *pOutx = &outx;
  int *pOuty = &outy;

  if (sx > ex) std::swap(sx, ex); // Soft to allow for x increments
  if (sy > ey) std::swap(sy, ey); // Sort to allow for y increments
  if (ey - sy > ex - sx) // Sort 
  {
    std::swap(sx, sy);
    std::swap(ex, ey);
    std::swap(pOutx, pOuty);
  }
  int d = ((ex > sx) << 1) - 1;
  float r = 0;
  if ((sy - sx) * d)
    r = (ey - sy) / (sy - sx) * d;

  float y = sy;
  int x = (int)sx;
  int e = (int)ex;
  while (x < e)
  {
    *pOutx = x++;
    *pOuty = int(y += r);
  }

  pixels.emplace_back(outx, outy);
}


void __MagicLine2D(std::vector<vec2i> &pixels, float sx, float sy, float ex, float ey)
{
  pixels.emplace_back(sx, sy);
  bool swapped = false;
  if (abs(sx - ex) > abs(sy - ey))
  {
    std::swap(sx, sy);
    std::swap(ex, ey);
    swapped = true;
  }
  int d = ((ey > sy) << 1) - 1;
  float r = (ex - sx) / (ey - sy) * d;
  if (swapped)
    for (int y = (int)sy; y != (int)ey; y += d) pixels.emplace_back(y, int(sx += r));
  else
    for (int y = (int)sy; y != (int)ey; y += d) pixels.emplace_back(int(sx += r), y);
}

void Rays()
{
  Window window;
  vec2i start = { 1, 256 };
  vec2i end = { (int)window.width - 1, 256 };
  std::vector<vec2i> pixels;
  while (Controls::Update())
  {

    if (Controls::GetLeftClick())
      start = Controls::GetMouse();

    if (Controls::GetRightClick())
      end = Controls::GetMouse();

    window.Clear();

    const int lineAmt = 1000000;

    //for (int i = 0; i < 1; i++)
    {
      int startTime = clock();
      for (int i = 0; i < lineAmt; i++)
      {

        //pixels.clear();
        //for (int i = window.width; i > 0 ; i--)
        //{
        //  pixels.emplace_back(256, i + 1);
        //}

        //__MagicLine2D(pixels, start.x, start.y, end.x, end.y);
        JamesLine(pixels, start.x, start.y, end.x, end.y);
        //MagicLine2D(pixels, start.x, start.y, end.x, end.y);
      }
      int endTime = clock();
      printf("James line took %d milliseconds to draw %d lines\n\n", endTime - startTime, lineAmt);

    }
    for (auto & pix : pixels)
      if(pix.x >= 0 && pix.x < window.width && pix.y >= 0 && pix.y < window.height)
        window.pixels[pix.x + pix.y * window.width] = 0xFFFFFF;

    window.Swap();
  }

}
