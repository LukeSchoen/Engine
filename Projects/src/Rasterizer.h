#ifndef Rasterizer_h__
#define Rasterizer_h__
#include "maths.h"

class Texture
{
public:

  vec4 TriLinearSample(vec2 UVpos, double pixelDensity)
  {
    // Calculate Pixel to Voxel ratio
    pixelDensity *= width * height;

    // Use bilinear where appropriate
    if ((layerCount < 2) || (pixelDensity < 1))
      return BiLinearSample(UVpos, 0);

    // Calculate mip map layer
    double mipLayer = log(pixelDensity) / log(4.0); // Each factor of 4 is a new layer
    int layer = Min(floor(mipLayer), layerCount - 2);

    // Calculate mip map ratio
    double mipFactor = FractionalPart(mipLayer);
    mipFactor *= mipFactor;

    // Sample mip map layers
    vec4 highResSample = BiLinearSample(UVpos, layer);
    vec4 lowResSample = BiLinearSample(UVpos, layer + 1);

    // Inter mip map interpolation
    return (highResSample * mipFactor + lowResSample * (1.0 - mipFactor));
  }

  vec4 BiLinearSample(vec2 UVpos, int64_t layer)
  {
    UVpos.y = 1.0-UVpos.y; // flip uv vertically

    // Calculate mip map layer image size
    int w = width >> layer;
    int h = height >> layer;

    w = Max(w, 1);
    h = Max(h, 1);

    // Offset to center of pixels
    UVpos = UVpos - vec2(0.5f / w, 0.5f / h);

    // Normalize UV coordinates
    UVpos = { (float)FractionalPart(UVpos.x), (float)FractionalPart(UVpos.y) };

    // Convert to pixel coordinates
    UVpos = UVpos * vec2(w, h);

    // Calculate bilinear interpolation factors
    double xoff = FractionalPart(UVpos.x);
    double yoff = FractionalPart(UVpos.y);

    // Resolve pixel locations
    int x1 = int(UVpos.x) % w;
    int y1 = int(UVpos.y) % h;
    int x2 = (x1 + 1) % w;
    int y2 = (y1 + 1) % h;

    // Reverse vertical texture locations
    y1 = (h - 1) - y1;
    y2 = (h - 1) - y2;

    // Read color data
    vec4 *pImage = layers[layer];

    vec4 c1 = pImage[x1 + y1 * w];
    vec4 c2 = pImage[x2 + y1 * w];
    vec4 c3 = pImage[x1 + y2 * w];
    vec4 c4 = pImage[x2 + y2 * w];

    // BiLinear Interpolation
    vec4 top = c1 * (1.0f - xoff) + c2 * xoff;
    vec4 bottom = c3 * (1.0f - xoff) + c4 * xoff;
    return bottom * yoff + top * (1.0f - yoff);
  }

  bool CreateFromImage(const char *filePath)
  {
    uint32_t w;
    uint32_t h;
    uint32_t *img = ImageFile::ReadImage(filePath, &w, &h);
    if (img == nullptr)
      return false;

    int64_t longestSide = Max(w, h);

    width = w;
    height = h;

    layerCount = 0;
    while ((longestSide >>= 1) > 1)
      layerCount++;

    if (layers != nullptr)
      delete[] layers;

    layers = new vec4*[layerCount];

    layers[0] = new vec4[w * h];
    for (int y = 0; y < h; y++)
      for (int x = 0; x < w; x++)
      {
        uint32_t c = img[x + y * w];
        layers[0][x + y * w] = vec4(c & 0x000000FF, (c & 0x0000FF00) >> 8, (c & 0x00FF0000) >> 16, (c & 0xFF000000) >> 24) / 255.0;
      }

    delete[] img;

    CreateMipChain();
  }

  void CreateMipChain()
  {
    int64_t w = width;
    int64_t h = height;
    for (int mip = 1; mip < layerCount; mip++)
    {
      w >>= 1;
      h >>= 1;
      layers[mip] = new vec4[w * h];
      for (int y = 0; y < h; y++)
      {
        for (int x = 0; x < w; x++)
        {
          vec4 mean = layers[mip - 1][((x * 2) + (y * 2) * w * 2)];
          mean = mean + layers[mip - 1][((x * 2 + 1) + (y * 2) * w * 2)];
          mean = mean + layers[mip - 1][((x * 2 + 1) + (y * 2 + 1) * w * 2)];
          mean = mean + layers[mip - 1][((x * 2) + (y * 2 + 1) * w * 2)];
          layers[mip][x + y * w] = mean / 4.0f;
        }
      }
    }
  }

private:
  int64_t width = 0;
  int64_t height = 0;
  int64_t layerCount = 0;
  vec4 **layers = nullptr;
};

class Rasterizer
{
private:
  struct Scan;
  struct Span;

public:
  struct Vertex
  {
    Vertex() = default;

    Vertex(vec2 position, int vertID)
    {
      inf.x = float(vertID == 0);
      inf.y = float(vertID == 1);
      inf.z = float(vertID == 2);
      pos = position + vec2(0.5, 0.5); // half pixel offset ensures identical results to openGL
    }

    Vertex(vec2 position, vec3 influence)
    {
      inf = influence;
      pos = position;
    }
    vec3 inf; // the relative influence from the triangles vertices (v1i + v2i + v3i = 1.0)
    vec2 pos;
  };

  std::vector<Vertex> fragments;

  const std::vector<Vertex> &RasterizeTriangle(const vec2 &p0, const vec2 &p1, const vec2 &p2)
  {
    fragments.clear();

    Vertex v1(p0, 0);
    Vertex v2(p1, 1);
    Vertex v3(p2, 2);

    Span s[3] = { Span(v1, v2) ,Span(v2, v3),Span(v3, v1) };

    if (s[2].Len() > s[1].Len()) std::swap(s[1], s[2]);
    if (s[1].Len() > s[0].Len()) std::swap(s[0], s[1]);

    ScanSpan(s[0], s[1]);
    ScanSpan(s[0], s[2]);

    return fragments;
  }

private:

  void ScanSpan(const Span &longSpan, const Span &shortSpan)
  {
    // Skip if span is empty
    vec2 shortLine = shortSpan.Line();
    vec2 longLine = longSpan.Line();
    if (shortLine.y == 0 || longLine.y == 0) return;

    // Calculate Spans Vertical Extents
    float shortTop = shortSpan.m_v1.pos.y;
    float shortBot = shortSpan.m_v2.pos.y;
    int yTop = (int)ceil(shortTop);
    int yBot = (int)ceil(shortBot);

    // Calculate Spans Stepping rates
    float shortStepX = (shortLine.x / shortLine.y);
    float longStepX = (longLine.x / longLine.y);
    vec3 shortStepInf = (shortSpan.m_v2.inf - shortSpan.m_v1.inf) / shortLine.y;
    vec3 longStepInf = (longSpan.m_v2.inf - longSpan.m_v1.inf) / longLine.y;

    // Calculate where on the long edge we are starting
    float longStart = (shortTop - longSpan.m_v1.pos.y) / longLine.y;

    // Calculate Scans Initial xPosition
    float shortPosX = shortSpan.m_v1.pos.x;
    float longPosX = longSpan.m_v1.pos.x * (1.0f - longStart) + longSpan.m_v2.pos.x * (longStart);

    // Calculate Scans Initial Influence
    vec3 shortInf = shortSpan.m_v1.inf;
    vec3 longInf = longSpan.m_v1.inf * (1.0f - longStart) + longSpan.m_v2.inf * (longStart);

    // Apply OpenGL fragment filling conventions
    float fillOffset = (yTop - shortTop);
    shortPosX += shortStepX * fillOffset;
    longPosX += longStepX * fillOffset;

    shortInf += shortStepInf * fillOffset;
    longInf += longStepInf * fillOffset;


    // Draw Scans in Span
    for (int y = yTop; y < yBot; y++)
    {
      Vertex shortEdge({ shortPosX, (float)y }, shortInf);
      Vertex longEdge({ longPosX, (float)y }, longInf);

      ScanLine({ shortEdge, longEdge });

      shortPosX += shortStepX;
      longPosX += longStepX;
      shortInf += shortStepInf;
      longInf += longStepInf;
    }
  }

  void ScanLine(const Scan &scan)
  {
    // Calculate Scan Extents
    int leftPosX = (int)(scan.m_v1.pos.x);
    int rightPosX = (int)(scan.m_v2.pos.x);
    int posY = (int)scan.m_v1.pos.y;

    // Calculate Scan Stepping Rates
    vec3 influence = scan.m_v1.inf;
    vec3 stepInfluence = (scan.m_v2.inf - scan.m_v1.inf) / (float(rightPosX) - leftPosX);

    // Draw Scan
    for (int x = leftPosX; x < rightPosX; x++)
    {
      Vertex vert({ (float)x, (float)posY }, influence);
      fragments.push_back(vert);
      influence += stepInfluence;
    }
  }

  struct Scan // horizontally sorted line
  {
    Scan(const Vertex &v1, const Vertex &v2)
    {
      m_v1 = v1;
      m_v2 = v2;
      if (v1.pos.x > v2.pos.x) std::swap(m_v1, m_v2);
    }
    float Len() const { return m_v2.pos.x - m_v1.pos.x; }
    vec2 Line() const { return m_v2.pos - m_v1.pos; }
    vec3 InfDiff() const { return m_v2.inf - m_v1.inf; }
    Vertex m_v1, m_v2;
  };

  struct Span // Vertically sorted line
  {
    Span(const Vertex &v1, const Vertex &v2)
    {
      m_v1 = v1;
      m_v2 = v2;
      if (v1.pos.y > v2.pos.y) std::swap(m_v1, m_v2);
    }
    float Len() const { return m_v2.pos.y - m_v1.pos.y; }
    vec2 Line() const { return m_v2.pos - m_v1.pos; }
    vec3 InfDiff() const { return m_v2.inf - m_v1.inf; }
    Vertex m_v1, m_v2;
  };
};

#endif // Rasterizer_h__
