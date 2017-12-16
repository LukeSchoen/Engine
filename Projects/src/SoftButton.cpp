#include "SoftButton.h"
#include "Controls.h"
#include "softText.h"
#include <algorithm>

uint32_t lighten(uint32_t col, float amt = 2)
{
  uint8_t *cc = (uint8_t*)&col;
  float red = cc[0] * amt;
  float green = cc[1] * amt;
  float blue = cc[2] * amt;
  return (int)std::min(red, 255.0f) + (int)std::min(green, 255.0f) * 256 + (int)std::min(blue, 255.0f) * 256 * 256;
}

SoftButton::SoftButton(const std::string &name, Window *window, int xpos, int ypos, int width, int height, uint32_t mainCol, uint32_t backCol) : SoftObject(name, window, xpos, ypos, width, height, mainCol, backCol)
{

}

bool SoftButton::Update()
{
  vec2i mp = Controls::GetMouse();
  bool clicked = false;
  if (!Controls::GetLeftClick())
    clickable = true;
  uint32_t mainCol = m_mainCol;
  uint32_t backCol = m_backCol;
  if (mp.x >= m_posx && mp.x < m_posx + m_width && mp.y >= m_posy && mp.y < m_posy + m_height)
  {
    mainCol = lighten(m_mainCol);
    backCol = lighten(m_backCol);
    if (Controls::GetLeftClick())
    {
      clicked = clickable;
      clickable = false;
    }
  }
  // Background
  for (int y = m_posy; y < m_posy + m_height; y++)
    for (int x = m_posx; x < m_posx + m_width; x++)
      m_window->pixels[x + y * m_window->width] = backCol;

  // Border
  for (int x = m_posx; x < m_posx + m_width; x++)
  {
    m_window->pixels[x + m_posy * m_window->width] = mainCol;
    m_window->pixels[x + (m_posy + m_height - 1) * m_window->width] = mainCol;
  }
  for (int y = m_posy; y < m_posy + m_height; y++)
  {
    m_window->pixels[m_posx + y * m_window->width] = mainCol;
    m_window->pixels[(m_posx + m_width - 1) + y * m_window->width] = mainCol;
  }

  // Text
  SoftText text(m_window);
  int x = m_posx + m_width / 2 - (m_name.length() * 4);
  int y = m_posy + m_height / 2 - 4;
  text.DrawText(m_name.c_str(), mainCol, x, y, 1);

  return clicked;
}

