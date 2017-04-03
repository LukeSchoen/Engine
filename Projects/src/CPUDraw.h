#ifndef CPUDraw_h__
#define CPUDraw_h__

#include "PointCloud.h"
#include "Maths.h"
#include "Window.h"

struct CPURenderer
{
  // Fixed point options
  const int fpb = 16;
  const int fpv = (1 << fpb);
  int xm1, xm2, xm3, xm4, ym1, ym2, ym3, ym4, zm1, zm2, zm3, zm4;
  float screenMaxX;
  float screenMaxY;
  int w;
  int h;
  float fw;
  float fh;
  uint32_t *pixels;
  uint32_t *depth;

  void PrepareFrame(const mat4 &MVP, const Window &window, uint32_t *depthBuffer)
  {
    xm1 = MVP.m[0][0] * fpv;
    xm2 = MVP.m[0][1] * fpv;
    xm3 = MVP.m[0][2] * fpv;
    xm4 = MVP.m[0][3] * fpv;
    ym1 = MVP.m[1][0] * fpv;
    ym2 = MVP.m[1][1] * fpv;
    ym3 = MVP.m[1][2] * fpv;
    ym4 = MVP.m[1][3] * fpv;
    zm1 = MVP.m[2][0] * fpv;
    zm2 = MVP.m[2][1] * fpv;
    zm3 = MVP.m[2][2] * fpv;
    zm4 = MVP.m[2][3] * fpv;
    screenMaxX = 1 - (1.0f / window.width);
    screenMaxY = 1 - (1.0f / window.height);
    w = window.width;
    h = window.height;
    fw = w;
    fh = h;
    pixels = window.pixels;
    depth = depthBuffer;
  }

  bool OnScreen(Point &p, float voxelSize = 2) const
  {
    int ix = p.x;
    int iy = p.y;
    int iz = p.z;

    int intz = zm1 * ix + zm2 * iy + zm3 * iz + zm4;
    float x = xm1 * ix + xm2 * iy + xm3 * iz + xm4;
    float y = 0.f - (ym1 * ix + ym2 * iy + ym3 * iz + ym4);
    float z = intz;

    if (z < 0) return true;

    float inv = 0.5f / z;

    x *= inv;
    y *= inv;
    x += 0.5f;
    y += 0.5f;
    x *= fw;
    y *= fh;
    int screenx = (int)x;
    int screeny = (int)y;

    int dVoxelSize = Max(inv * w * voxelSize * fpv, 1);
    int extra = dVoxelSize & 1;
    dVoxelSize >>= 1;
    int left = Max(screenx - dVoxelSize, 0);
    int right = Min(screenx + dVoxelSize + extra, w - 1);
    int top = Max(screeny - dVoxelSize, 0);
    int bottom = Min(screeny + dVoxelSize + extra, h - 1);
    for (int y = top; y < bottom; y++)
      for (int x = left; x < right; x++)
        return true;
    return false;
  }

  void DrawPoint(Point &p, float voxelSize = 2) const
  {
    int ix = p.x;
    int iy = p.y;
    int iz = p.z;

    int intz = zm1 * ix + zm2 * iy + zm3 * iz + zm4;
    if (intz < 1) return;

    float x = xm1 * ix + xm2 * iy + xm3 * iz + xm4;
    float y = 0.f - (ym1 * ix + ym2 * iy + ym3 * iz + ym4);
    float z = intz;

    float inv = 0.5f / z;

    x *= inv;
    y *= inv;
    x += 0.5f;
    y += 0.5f;
    x *= fw;
    y *= fh;
    int screenx = (int)x;
    int screeny = (int)y;

    if (true)
      //if (z < fpv * 250 * voxelSize)
    {
      // Quad Mode
      int dVoxelSize = Max(inv * w * voxelSize * fpv, 1);
      int extra = dVoxelSize & 1;
      dVoxelSize >>= 1;
      int left = Max(screenx - dVoxelSize, 0);
      int right = Min(screenx + dVoxelSize + extra, w - 1);
      int top = Max(screeny - dVoxelSize, 0);
      int bottom = Min(screeny + dVoxelSize + extra, h - 1);
      for (int y = top; y < bottom; y++)
        for (int x = left; x < right; x++)
        {
          int index = x + y * w;
          if (intz > depth[index]) continue;
          pixels[index] = p.color;
          depth[index] = intz;
        }
    }
    else
    {
      // Pixel Mode
      if (screenx < 0 || screenx >= w || screeny < 0 || screeny >= h) return;
      int index = screenx + screeny * w;
      if (intz > depth[index]) return;
      pixels[index] = p.color;
      depth[index] = intz;
    }
  }

};
#endif // CPUDraw_h__