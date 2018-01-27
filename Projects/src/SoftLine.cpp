#include "SoftLine.h"

void SoftLine::DrawLine(Window *window, uint32_t color, int startX, int startY, int endX, int endY)
{
  // 2D-DDA algorithm
  int posx = startX;
  int posy = startY;
  float dirx = endX - startX + FLT_EPSILON;
  float diry = endY - startY + FLT_EPSILON;
  int attx = dirx > 0;
  int atty = diry > 0;
  float dirLen = sqrt(dirx * dirx + diry * diry);
  dirx /= dirLen;
  diry /= dirLen;
  float idirx = 1.0 / dirx;
  float idiry = 1.0 / diry;
  float fposx = posx;
  float fposy = posy;
  float currLen = 0;
  while (currLen < dirLen)
  {
    if (posx >= 0 && posx < window->width && posy >= 0 && posy < window->height) window->pixels[posx + posy * window->width] = color;
    float crossx = ((posx + attx) - fposx) * idirx;
    float crossy = ((posy + atty) - fposy) * idiry;
    bool cross = crossx < crossy;
    if (cross)
    {
      posx += attx * 2 - 1;
      fposx += dirx * crossx;
      fposy += diry * crossx;
      currLen += crossx;
    }
    else
    {
      posy += atty * 2 - 1;
      fposx += dirx * crossy;
      fposy += diry * crossy;
      currLen += crossy;
    }
  }
}

void SoftLine::DrawDropLine(Window *window, uint32_t color, int startX, int startY, int endX, int endY, int dropHeight)
{
  // 2D-DDA algorithm
  int posx = startX;
  int posy = startY;
  float dirx = endX - startX + FLT_EPSILON;
  float diry = endY - startY + FLT_EPSILON;
  int attx = dirx > 0;
  int atty = diry > 0;
  float dirLen = sqrt(dirx * dirx + diry * diry);
  dirx /= dirLen;
  diry /= dirLen;
  float idirx = 1.0 / dirx;
  float idiry = 1.0 / diry;
  float fposx = posx;
  float fposy = posy;
  float currLen = 0;
  while (currLen < dirLen)
  {
    if (posx >= 0 && posx < window->width && posy >= 0 && posy < window->height)
      for (int64_t d = posy; d < dropHeight; d++)
        window->pixels[posx + d * window->width] = color;
    float crossx = ((posx + attx) - fposx) * idirx;
    float crossy = ((posy + atty) - fposy) * idiry;
    bool cross = crossx < crossy;
    if (cross)
    {
      posx += attx * 2 - 1;
      fposx += dirx * crossx;
      fposy += diry * crossx;
      currLen += crossx;
    }
    else
    {
      posy += atty * 2 - 1;
      fposx += dirx * crossy;
      fposy += diry * crossy;
      currLen += crossy;
    }
  }
}
