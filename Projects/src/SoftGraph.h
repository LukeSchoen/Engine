#ifndef SoftGraph_h__
#define SoftGraph_h__

#include "SoftObject.h"
#include "Okex.h"

class SoftGraph : public SoftObject
{
public:

  SoftGraph(const std::string &name, Window *window, int xpos, int ypos, int width, int height, uint32_t mainCol, uint32_t backCol)
    : SoftObject(name, window, xpos, ypos, width, height, mainCol, backCol)
  { }

  void SetValueList(std::vector<Trade> *ValueList)
  {

  }

  void Update()
  {
    // Background
    for (int y = m_posy; y < m_posy + m_height; y++)
      for (int x = m_posx; x < m_posx + m_width; x++)
        m_window->pixels[x + y * m_window->width] = m_backCol;

    // Border
    for (int x = m_posx; x < m_posx + m_width; x++)
    {
      m_window->pixels[x + m_posy * m_window->width] = m_mainCol;
      m_window->pixels[x + (m_posy + m_height - 1) * m_window->width] = m_mainCol;
    }
  }
};

#endif // SoftGraph_h__