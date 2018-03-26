#include "Rays.h"
#include "Window.h"
#include "Controls.h"

void MagicLine2D(std::vector<vec2i> &pixels, float sx, float sy, float ex, float ey)
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
    if (Controls::GetLeftClick()) start = Controls::GetMouse();
    if (Controls::GetRightClick()) end = Controls::GetMouse();

    window.Clear();

    for (int i = 0; i < 400000; i++)
    {
      pixels.clear();
      MagicLine2D(pixels, start.x, start.y, end.x, end.y);
    }
    for (auto & pix : pixels)
      window.pixels[pix.x + pix.y * window.width] = 0xFFFFFF;

    window.Swap();
  }

}
