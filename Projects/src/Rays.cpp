#include "Rays.h"
#include "Window.h"
#include "Controls.h"

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
  float r = 0;
  r = ((ey - sy) / (sx - ex));
  float y = sy;
  int x = (int)sx;
  int e = (int)ex;
  int d = ((e > x) << 1) - 1;
  while (x != e)
  {
    pixels.emplace_back(x, int(y));
    x += d;
    y += r * - d;
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
  if((sy - sx) * d)
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
  if(swapped)
    for (int y = (int)sy; y != (int)ey; y += d) pixels.emplace_back(y, int(sx += r));
  else
    for (int y = (int)sy; y != (int)ey; y += d) pixels.emplace_back(int(sx += r), y);
}

void Rays()
{
  Window window;
  vec2i start;
  vec2i end;
  std::vector<vec2i> pixels;
  while (Controls::Update())
  {

    if (Controls::GetLeftClick())
      start = Controls::GetMouse();

    if (Controls::GetRightClick())
      end = Controls::GetMouse();

    window.Clear();

    for (int i = 0; i < 1; i++)
    {
      pixels.clear();
      MagicLine2D(pixels, start.x, start.y, end.x, end.y);
    }
    for (auto & pix : pixels)
      window.pixels[pix.x + pix.y * window.width] = 0xFFFFFF;

    window.Swap();
  }

}
