#include "softText.h"
#include "Window.h"
#include <string>
#include <stdlib.h>

SoftText::SoftText(Window *window)
{
  win = window;
  uint32_t x, y;
  img = ImageFile::ReadImage(ASSETDIR "font.bmp", &x, &y);
}

SoftText::~SoftText()
{
  delete[] img;
}

void SoftText::DrawPriceUSD(int64_t cents, uint32_t mainCol, uint32_t backCol, int xpos, int ypos, int size /*= 1*/)
{
  bool negative = cents < 0;
  if (negative) cents = 0 - cents;
  int valDollars = cents / 100;
  int valCents = cents % 100;
  char negs[2] = {};
  if (negative)
    negs[0] = '-';
  static char buff[1024];
  std::string cen = std::to_string(valCents);
  while (cen.length() < 2)
    cen = "0" + cen;
  sprintf(buff, "usd $%s%d.%s", negs, valDollars, cen.c_str());
  DrawText(buff, backCol, xpos, ypos, size);
  sprintf(buff, "     %s%d", negs, valDollars);
  DrawText(buff, mainCol, xpos, ypos, size);
}

void SoftText::DrawPriceBTC(int64_t satoshi, uint32_t mainCol, uint32_t backCol, int xpos, int ypos, int size /*= 1*/)
{
  int coins = satoshi / 100000000;
  int satoshis = satoshi % 100000000;
  std::string satosh = std::to_string(satoshis);
  while (satosh.length() < 8)
    satosh = "0" + satosh;
  static char buff[1024];
  sprintf(buff, "btc %d.%s", coins, satosh.c_str());
  DrawText(buff, backCol, xpos, ypos, size);
  sprintf(buff, "    %d", coins);
  DrawText(buff, mainCol, xpos, ypos, size);
}

void SoftText::DrawNumber(int64_t numb, uint32_t col, int xpos, int ypos, int size /*= 1*/)
{
  static char buff[1024];
  DrawText(itoa(numb, buff, 10), col, xpos, ypos, size);
}

void SoftText::DrawText(const char *text, uint32_t col, int xpos, int ypos, int size /*= 1*/)
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
              {
                int px = (xpos + (x)* size + sx);
                int py = (ypos + (y)* size + sy);
                if(px >= 0 && py >= 0 && px < win->width && py < win->height)
                  win->pixels[px + py * win->width] = col;
              }

    if (text[c] == '\n')
      ypos += 8 * size;
    xpos += 8 * size;
  }
}

