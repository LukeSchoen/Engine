#ifndef Rasterizer_h__
#define Rasterizer_h__
#include "maths.h"

class Rasterizer
{
public:

  struct Scan // horizontally sorted line
  {
    Scan(const vec2 &v1, const vec2 &v2)
    {
      if (v1.x < v2.x)
      {
        m_v1 = v1;
        m_v2 = v2;
      }
      else
      {
        m_v1 = v2;
        m_v2 = v1;
      }
    }
    float Len() const { return m_v2.x - m_v1.x; }
    vec2 Line() const { return m_v2 - m_v1; }
    vec2 m_v1, m_v2;
  };

  struct Span // Vertically sorted line
  {
    Span(const vec2 &v1, const vec2 &v2)
    {
      if (v1.y < v2.y)
      {
        m_v1 = v1;
        m_v2 = v2;
      }
      else
      {
        m_v1 = v2;
        m_v2 = v1;
      }
    }
    float Len() const { return m_v2.y - m_v1.y; }
    vec2 Line() const { return m_v2 - m_v1; }
    vec2 m_v1, m_v2;
  };

  const std::vector<vec2i> &RasterizeTriangle(const vec2 &v1, const vec2 &v2, const vec2 &v3)
  {
    pixels.clear();

    Span s[3] = { { v1, v2 }, { v2, v3 }, { v3, v1 } };

    if (s[2].Len() > s[1].Len()) std::swap(s[1], s[2]);
    if (s[1].Len() > s[0].Len()) std::swap(s[0], s[1]);

    DrawSpan(s[0], s[1]);
    DrawSpan(s[0], s[2]);

    return pixels;
  }

private:
  void DrawSpan(const Span &longSpan, const Span &shortSpan)
  {
    vec2 longLine = longSpan.Line();
    vec2 shortLine = shortSpan.Line();

    if (shortLine.y == 0 || longLine.y == 0) return;

    float gradS = (shortLine.x / shortLine.y);
    float gradL = (longLine.x / longLine.y);

    int yStart = (int)ceil(shortSpan.m_v1.y);
    int yEnd = (int)ceil(shortSpan.m_v2.y);

    float startOffset = (yStart - shortSpan.m_v1.y); // offset here to follow openGL fill conventions

    float sx = gradS * startOffset;
    float lx = gradL * startOffset;

    float longTstart = (shortSpan.m_v1.y - longSpan.m_v1.y) / longLine.y;
    sx += shortSpan.m_v1.x;
    lx += longSpan.m_v1.x * (1.0f - longTstart) + longSpan.m_v2.x * (longTstart);

    for (int y = yStart; y < yEnd; y++)
    {
      vec2 shortVert(sx, y);
      vec2 longVert(lx, y);

      DrawScan({ longVert, shortVert });

      sx += gradS;
      lx += gradL;
    }
  }

  void DrawScan(const Scan &scan)
  {
    int xStart = (int)(scan.m_v1.x + 0.5);
    int xEnd = (int)(scan.m_v2.x + 0.5);
    for (int x = xStart; x < xEnd; x++)
      pixels.push_back(vec2i(x, scan.m_v1.y));
  }

  std::vector<vec2i> pixels;

};

#endif // Rasterizer_h__
