#include "KhanLeastRects.h"
#include "maths.h"
#include "AStar.h"
#include "LeastRects.h"

Window *pWindow;

struct LeastRectsPathNode
{
  vec2i FindActiveRectTL()
  {
    for (int y = 0; y < m_height; y++)
      for (int x = 0; x < m_width; x++)
        if (m_Image[x + y * m_width])
          return{ x, y };
  }

  vec2i FindActiveRectTR()
  {
    for (int y = 0; y < m_height; y++)
      for (int x = m_width - 1; x >= 0; x--)
        if (m_Image[x + y * m_width])
          return{ x, y };
  }

  vec2i FindActiveRectBL()
  {
    for (int y = m_height - 1; y >= 0; y--)
      for (int x = 0; x < m_width; x++)
        if (m_Image[x + y * m_width])
          return{ x, y };
  }

  vec2i FindActiveRectBR()
  {
    for (int y = m_height - 1; y >= 0; y--)
      for (int x = m_width - 1; x >= 0; x--)
        if (m_Image[x + y * m_width])
          return{ x, y };
  }

  int FindRectRightSide(vec2i startPos)
  {
    for (int x = startPos.x; x < m_width; x++)
      if (!m_Image[x + startPos.y * m_width])
        return x;
  }

  int FindRectLeftSide(vec2i startPos)
  {
    for (int x = startPos.x; x >= 0; x--)
      if (!m_Image[x + startPos.y * m_width])
        return x;
  }

  LeastRectsPathNode(std::vector<KhanRectangle> rects, std::vector<bool> image, int32_t width, int32_t height)
  {
    m_Image = image;
    m_rects = std::move(rects);
    m_width = width;
    m_height = height;
    m_tlCount = 0;
    m_trCount = 0;
    m_blCount = 0;
    m_brCount = 0;
    for (int y = 0; y < m_height; y++)
      for (int x = 0; x < m_width; x++)
        if (m_Image[x + y * m_width])
        {
          if (!m_Image[x + (y - 1) * m_width])
          {
            if (!m_Image[(x - 1) + y * m_width])
              m_tlCount++;
            if (!m_Image[(x + 1) + y * m_width])
              m_trCount++;
          }
          if (!m_Image[x + (y + 1) * m_width])
          {
            if (!m_Image[(x - 1) + y * m_width])
              m_blCount++;
            if (!m_Image[(x + 1) + y * m_width])
              m_brCount++;
          }
        }

    m_cost = std::max(std::max(m_tlCount, m_trCount), std::max(m_blCount, m_brCount)) + m_rects.size();

    for (auto& rect : m_rects)
      m_area += rect.width * rect.height;

    m_hash = std::hash<std::vector<bool>>()(m_Image);
  }
  bool IsComplete() const
  {
    for (int y = 0; y < m_height; y++)
      for (int x = 0; x < m_width; x++)
        if (m_Image[x + y * m_width])
          return false;
    return true;
  }
  bool HeuristicCompare(LeastRectsPathNode const& other) const
  {
    if (m_cost == other.m_cost)
      return m_area > other.m_area;
    else
      return m_cost < other.m_cost;
  }

  size_t GetProblemStateHash() const
  {
    return m_hash;
  }

  bool IsProblemStateEqual(LeastRectsPathNode const& o) const
  {
    return m_Image == o.m_Image;
  }

  std::vector<LeastRectsPathNode> CreateChildNodes(std::vector<KhanRectangle> const& rects)
  {
    std::vector<LeastRectsPathNode> childNodes;

    for (auto &rect : rects)
    {
      std::vector<KhanRectangle> newRects = m_rects;
      newRects.push_back(rect);
      std::vector<bool> childImage = m_Image;
      for (int y = rect.y; y < rect.y + rect.height; y++)
        for (int x = rect.x; x < rect.x + rect.width; x++)
          childImage[x + y * m_width] = false;
      childNodes.emplace_back(newRects, childImage, m_width, m_height);
    }
    return childNodes;
  }

  std::vector<LeastRectsPathNode> ExpandTL()
  {
    std::vector<KhanRectangle> rects;

    auto pos = FindActiveRectTL();
    auto right = FindRectRightSide(pos);

    for (int y = pos.y; y < m_height; y++)
    {
      if (!m_Image[pos.x + y * m_width])
        break;
      for (int x = pos.x; x < right; x++)
      {
        if (m_Image[x + y * m_width])
          rects.push_back({ pos.x, pos.y, x - pos.x + 1, y - pos.y + 1 });
        else
          right = x;
      }
    }
    return CreateChildNodes(rects);
  }

  std::vector<LeastRectsPathNode> ExpandTR()
  {
    std::vector<KhanRectangle> rects;

    auto pos = FindActiveRectTR();
    auto left = FindRectLeftSide(pos);

    for (int y = pos.y; y < m_height; y++)
    {
      if (!m_Image[pos.x + y * m_width])
        break;
      for (int x = pos.x; x > left; x--)
      {
        if (m_Image[x + y * m_width])
          rects.push_back({ x, pos.y, pos.x - x + 1, y - pos.y + 1 });
        else
          left = x;
      }
    }
    return CreateChildNodes(rects);
  }

  std::vector<LeastRectsPathNode> ExpandBL()
  {
    std::vector<KhanRectangle> rects;

    auto pos = FindActiveRectBL();
    auto right = FindRectRightSide(pos);

    for (int y = pos.y; y >= 0; y--)
    {
      if (!m_Image[pos.x + y * m_width])
        break;
      for (int x = pos.x; x < right; x++)
      {
        if (m_Image[x + y * m_width])
          rects.push_back({ pos.x, y, x - pos.x + 1, pos.y - y + 1 });
        else
          right = x;
      }
    }
    return CreateChildNodes(rects);
  }

  std::vector<LeastRectsPathNode> ExpandBR()
  {
    std::vector<KhanRectangle> rects;

    auto pos = FindActiveRectBR();
    auto left = FindRectLeftSide(pos);

    for (int y = pos.y; y >= 0; y--)
    {
      if (!m_Image[pos.x + y * m_width])
        break;
      for (int x = pos.x; x > left; x--)
      {
        if (m_Image[x + y * m_width])
          rects.push_back({ x, y, pos.x - x + 1, pos.y - y + 1 });
        else
          left = x;
      }
    }
    return CreateChildNodes(rects);
  }

  std::vector<LeastRectsPathNode> Expand()
  {
    if (m_tlCount > m_trCount)
      if (m_tlCount > m_blCount)
        return m_tlCount > m_brCount ? ExpandTL() : ExpandBR();
      else
        return m_blCount > m_brCount ? ExpandBL() : ExpandBR();
    else
      if (m_trCount > m_blCount)
        return m_trCount > m_brCount ? ExpandTR() : ExpandBR();
      else
        return m_blCount > m_brCount ? ExpandBL() : ExpandBR();
  }

  int m_cost = 0;
  int m_width = 0;
  int m_height = 0;
  int m_area = 0;
  int m_tlCount, m_trCount, m_blCount, m_brCount;
  std::vector<bool> m_Image;
  std::vector<KhanRectangle> m_rects;
  size_t m_hash;
};

std::vector<KhanRectangle> Quadrangulate(std::vector<bool> image, int32_t width, int32_t height) { return AStar<LeastRectsPathNode, true>::FindPath(LeastRectsPathNode({}, image, width, height)).m_rects; }

void KhanQuadRects()
{
  Window window("LeastRects", false, 800, 600, false);
  pWindow = &window;

  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t *pImage = ImageFile::ReadImage("D:/UserFiles/Desktop/Quads.png", &width, &height);
  for (int i = 0; i < width * height; i++) pImage[i] &= 0x00FFFFFF;
  std::vector<bool> bImage;
  bImage.resize(width * height);
  for (int i = 0; i < width * height; i++) bImage[i] = pImage[i] > 0;

  auto quads = Quadrangulate(bImage, width, height);
  printf("%d", quads.size());
  int scale = 10;

    window.Clear();
  for (auto &quad : quads)
  {
    uint32_t color = rand() + rand() * 256;
    for (int y = 0; y < quad.height * scale; y++)
      for (int x = 0; x < quad.width * scale; x++)
        window.pixels[quad.x * scale + x + (quad.y * scale + y) * window.width] = color;
  }

  bool *pBools = new bool[bImage.size()];
  for (int i = 0; i < bImage.size(); i++)
    pBools[i] = bImage[i];

  int count;
  auto quads2 = LeastRects::FindRectsInImage(pBools, width, height, &count);
  printf("%d\n", count);

  for (int i = 0; i < count; i++)
  {
    uint32_t color = rand() + rand() * 256;
    for (int y = 0; y < quads2[i].height * scale; y++)
      for (int x = 0; x < quads2[i].width * scale; x++)
        window.pixels[quads2[i].xpos * scale + 400 + x + (quads2[i].ypos * scale + y) * window.width] = color;
  }

  pWindow->Swap();

  getchar();
}
