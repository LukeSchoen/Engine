#ifndef softText_h__
#define softText_h__

#include "assimp\Compiler\pstdint.h"
#include <string.h>
#include "ImageFile.h"
#include "Assets.h"
#include "stdint.h"
#include "Window.h"

struct SoftText
{
  SoftText(Window *window);

  ~SoftText();

  void DrawPriceUSD(int64_t cents, uint32_t mainCol, uint32_t backCol, int xpos, int ypos, int size = 1);

  void DrawPriceBTC(int64_t satoshi, uint32_t mainCol, uint32_t backCol, int xpos, int ypos, int size = 1);

  void DrawNumber(int64_t numb, uint32_t col, int xpos, int ypos, int size = 1);

  void DrawText(const char *text, uint32_t col, int xpos, int ypos, int size = 1);

  uint32_t *img = nullptr;
  Window *win = nullptr;
};
#endif // softText_h__