#ifndef SoftLine_h__
#define SoftLine_h__

#include "Window.h"

class SoftLine
{
public:
  static void DrawLine(Window *window, uint32_t color, int startX, int startY, int endX, int endY);
  static void DrawDropLine(Window *window, uint32_t color, int startX, int startY, int endX, int endY, int dropHeight);
};

#endif // SoftLine_h__
