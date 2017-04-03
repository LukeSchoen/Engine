#ifndef LeastRects_h__
#define LeastRects_h__

struct Rect
{
  Rect(int x = 0, int y = 0, int w = 0, int h = 0)
  {
    xpos = x;
    ypos = y;
    width = w;
    height = h;
  }
  int xpos, ypos;
  int width, height;
};

struct LeastRects
{
  static Rect *FindRectsInImage(bool *image, int width, int height, int *rectCount);
};


#endif // LeastRects_h__
