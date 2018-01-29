#include "SoftGraph.h"
#include "softText.h"
#include "SoftLine.h"
#include "Controls.h"

SoftGraph::SoftGraph(const std::string &name, Window *window, int xpos, int ypos, int width, int height, uint32_t mainCol, uint32_t backCol) : SoftObject(name, window, xpos, ypos, width, height, mainCol, backCol)
{

}

void SoftGraph::SetValueList(std::vector<int64_t> *valueList)
{
  m_valueList = valueList;
}

// void SoftGraph::SetValueList(std::vector<Trade> *ValueList)
// {
// 
// }

void SoftGraph::Update()
{
  static std::vector<int64_t> zoomedValueList;
  zoomedValueList.clear();
  if (m_valueList)
  {
    int64_t start = Max(int64_t(m_valueList->size()) - int64_t(m_zoom), 0);
    int64_t finish = m_valueList->size();
    for (int64_t i = start; i < finish; i++)
    {
      m_zoom = m_zoom;
      zoomedValueList.push_back((*m_valueList)[i]);
    }
  }

  // Background
  for (int y = m_posy; y < m_posy + m_height; y++)
    for (int x = m_posx; x < m_posx + m_width; x++)
      m_window->pixels[x + y * m_window->width] = m_backCol;

  if (zoomedValueList.size() > 0)
  {
    vec2i mp = Controls::GetMouse();
    bool hovered = false;
    if (mp.x >= m_posx && mp.x < m_posx + m_width && mp.y >= m_posy && mp.y < m_posy + m_height) hovered = true;

    static SoftText text(m_window);
    int64_t minPrice = INT_MAX;
    int64_t maxPrice = INT_MIN;
    // Draw min / max price
    for (auto & price : zoomedValueList)
    {
      minPrice = Min(minPrice, price);
      maxPrice = Max(maxPrice, price);
    }

    // Draw line graph
    double x = m_posx;
    double step = double(m_width - 1) / (zoomedValueList.size() - 1);

    bool hoverTextDraw = false;
    int64_t hoverTextY = 0;
    int64_t hoverLineX = m_posx;
    int64_t hoverLineY = m_posy;
    int64_t hoverLineY2 = m_posy;
    int64_t hoverTextPrice = 0;
    for (int64_t i = 0; i < zoomedValueList.size() - 1; i++)
    {
      double currentY = m_posy + (m_height - 1) * (1.0 - (((zoomedValueList)[i] - minPrice) / double(maxPrice - minPrice)));
      double nextY = m_posy + (m_height - 1) * (1.0 - (((zoomedValueList)[i + 1] - minPrice) / double(maxPrice - minPrice)));
      SoftLine::DrawDropLine(m_window, 0x333333, x, currentY, x + step, nextY, m_posy + m_height);
      SoftLine::DrawLine(m_window, m_mainCol, x, currentY, x + step, nextY);

      if(hovered)
        if(mp.x >= x && mp.x < x+step)
        {
          hoverTextDraw = true;
          hoverTextPrice = (zoomedValueList)[i];
          hoverTextY = Min(Max(int(currentY) + 8, m_posy + 16), m_posy + m_height - 32);
          hoverLineY = int(currentY);
          hoverLineY2 = int(nextY);
          hoverLineX = x;
        }
      x += step;
    }

    if (hoverTextDraw)
    {
      text.DrawPriceUSD(hoverTextPrice, 0xFFDF00, 0xFFDF00, m_posx + 16, hoverTextY, 2);
      for (int ix = m_posx; ix < m_posx + m_width; ix++) if (ix & 1) m_window->pixels[ix + hoverLineY2 * m_window->width] = m_mainCol;
      for (int ix = m_posx; ix < m_posx + m_width; ix++) if (ix & 1) m_window->pixels[ix + hoverLineY * m_window->width] = 0xFFDF00;
      for (int y = m_posy; y < m_posy + m_height; y++) if (y & 1) m_window->pixels[hoverLineX + y * m_window->width] = m_mainCol;
      for (int y = m_posy; y < m_posy + m_height; y++) if (y & 1) m_window->pixels[int(hoverLineX + step) + y * m_window->width] = m_mainCol;
    }

    text.DrawPriceUSD(maxPrice, m_mainCol, m_mainCol, m_posx + m_width - 50 - ceil(log10(maxPrice)) * 8, m_posy + 6, 1);
    text.DrawPriceUSD(minPrice, m_mainCol, m_mainCol, m_posx + m_width - 50 - ceil(log10(minPrice)) * 8, m_posy + m_height - 16, 1);

    if (zoomedValueList.size() > 0)
    {
      double currentY = m_posy + (m_height - 1) * (1.0 - (((zoomedValueList)[zoomedValueList.size()-1] - minPrice) / double(maxPrice - minPrice)));
      text.DrawPriceUSD((zoomedValueList)[zoomedValueList.size()-1], 0xFFDF00, 0xFFDF00, m_posx + m_width - 50 - ceil(log10(minPrice)) * 8, Min(Max(int(currentY) + 4, m_posy + 6), m_posy + m_height - 16), 1);
    }

    if(hovered)
    {
      //for (int x = m_posx; x < m_posx + m_width; x++) m_window->pixels[x + mp.y * m_window->width] = m_mainCol;
      float mousey = 1.0 - (double(mp.y - m_posy) / m_height);

      int scroll = Controls::GetMouseScroll();
      while (scroll < 0)
      {
        m_targetZoom *= 1.1;
        if (m_targetZoom > 256.0f) m_targetZoom = 256.0f;
        scroll++;
      }
      while (scroll > 0)
      {
        m_targetZoom *= 0.9;
        if (m_targetZoom < 8.0f) m_targetZoom = 8.0f;
        scroll--;
      }
    }
    m_zoom = (m_zoom * 4 + m_targetZoom) * 0.2;
  }

  // Border
  for (int x = m_posx; x < m_posx + m_width; x++)
  {
    m_window->pixels[x + m_posy * m_window->width] = m_mainCol;
    m_window->pixels[x + (m_posy + m_height - 1) * m_window->width] = m_mainCol;
  }
  for (int y = m_posy; y < m_posy + m_height; y++)
  {
    m_window->pixels[m_posx + y * m_window->width] = m_mainCol;
    m_window->pixels[m_posx + y * m_window->width + (m_width - 1)] = m_mainCol;
  }
}

