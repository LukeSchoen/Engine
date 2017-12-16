#include "SoftObject.h"

SoftObject::SoftObject(const std::string &name, Window *window, int xpos, int ypos, int width, int height, uint32_t mainCol, uint32_t backCol) : m_name(name)
, m_window(window)
, m_posx(xpos)
, m_posy(ypos)
, m_width(width)
, m_height(height)
, m_mainCol(mainCol)
, m_backCol(backCol)
{

}

