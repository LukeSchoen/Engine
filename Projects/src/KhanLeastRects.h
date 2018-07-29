#ifndef KhanLeastRects_h__
#define KhanLeastRects_h__

#include <vector>
#include "ImageFile.h"
#include "Window.h"

struct KhanRectangle
{
  int x, y, width, height;
};


std::vector<KhanRectangle> Quadrangulate(std::vector<uint32_t> image, int32_t width, int32_t height);

#include "KhanLeastRects.h";

void KhanQuadRects();

#endif // KhanLeastRects_h__