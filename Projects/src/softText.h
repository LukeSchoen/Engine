#include "assimp\Compiler\pstdint.h"
#include <string.h>
#include "ImageFile.h"
#include "Assets.h"

struct SoftText
{
  SoftText(Window *window)
  {
    win = window;
    uint32_t x, y;
    img = ImageFile::ReadImage(ASSETDIR "font.bmp", &x, &y);
  }

  void DrawPrice(int64_t cents, uint32_t col, int xpos, int ypos, int size = 1)
  {
    int valDollars = cents / 100;
    int valCents = cents % 100;
    static char buff[1024];
    sprintf(buff, "$%d.%d", valDollars, valCents);
    DrawText(buff, col, xpos, ypos, size);
  }

  void DrawText(int64_t numb, uint32_t col, int xpos, int ypos, int size = 1)
  {
    static char buff[1024];
    DrawText(itoa(numb, buff, 10), col, xpos, ypos, size);
  }

  void DrawText(const char *text, uint32_t col, int xpos, int ypos, int size = 1)
  {
    int strl = strlen(text);
    for (int c = 0; c < strl; c++)
    {
      char m = (text[c] - 33);
      int ix = m % 32;
      int iy = m / 32;
      if (text[c] != ' ')
        for (int y = 0; y < 8; y++)
          for (int x = 0; x < 8; x++)
            if (img[(ix * 8 + x) + (iy * 8 + y) * 256] == 0xFFFFFFFF)
              for (int sy = 0; sy < size; sy++)
                for (int sx = 0; sx < size; sx++)
                  win->pixels[(xpos + (x)* size + sx) + (ypos + (y)* size + sy) * win->width] = col;
      if (text[c] == '\n')
        ypos += 8 * size;
      xpos += 8 * size;
    }
  }

  uint32_t *img = nullptr;
  Window *win = nullptr;
};