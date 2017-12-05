#ifndef SoftObject_h__
#define SoftObject_h__

#include "Window.h"
#include <string>

class SoftObject
{
public:

  SoftObject(const std::string &name, Window *window, int xpos, int ypos, int width, int height, uint32_t mainCol, uint32_t backCol)
    : m_name(name)
    , m_window(window)
    , m_posx(xpos)
    , m_posy(ypos)
    , m_width(width)
    , m_height(height)
    , m_mainCol(mainCol)
    , m_backCol(backCol)
  {}

  std::string m_name;
  Window *m_window;
  int m_posx, m_posy, m_width, m_height;
  uint32_t m_mainCol, m_backCol;
};

#endif // SoftObject_h__
