#ifndef SimpleUD_h__
#define SimpleUD_h__

#include "Window.h"
#include "Controls.h"
#include "Camera.h"

float winWidth = 800;
float winHeight = 600;
float MVP[16];

uint32_t *pixels;

bool CubeVisible(float x, float y, float z, float w, float h, float l)
{
  float vz = MVP[0 + 8] * x + MVP[1 + 8] * y + MVP[2 + 8] * z + MVP[3 + 8];
  if (vz < 0) return false; // behind cameras

  float vx = MVP[0] * x + MVP[1] * y + MVP[2] * z + MVP[3] * 1;
  float vy = MVP[0 + 4] * x + MVP[1 + 4] * y + MVP[2 + 4] * z + MVP[3] * 1;

  float iz = 1.0 / vz;

  float px = vx * iz;
  float py = vy * iz;

  float sx = px * winWidth;
  float sy = py * winHeight;

  if (!(sx >= 0 && sy >= 0 && sx < winWidth&& sy < winHeight)) return false; // Off screen

  return true;
}

void DrawPoint(float x, float y, float z, uint32_t col)
{
  float vz = MVP[0 + 8] * x + MVP[1 + 8] * y + MVP[2 + 8] * z + MVP[3 + 8];
  if (vz < 0) return; // behind cameras

  float vx = MVP[0] * x + MVP[1] * y + MVP[2] * z + MVP[3] * 1;
  float vy = MVP[0 + 4] * x + MVP[1 + 4] * y + MVP[2 + 4] * z + MVP[3] * 1;

  float iz = 1.0 / vz;

  float px = vx * iz;
  float py = vy * iz;

  float sx = ((px * 0.5) + 0.5) * winWidth;
  float sy = ((py * 0.5) + 0.5) * winHeight;

  if (!(sx >= 0 && sy >= 0 && sx < winWidth&& sy < winHeight)) return; // Off screen

  int xpos = (int)sx;
  int ypos = (int)sy;

  //pixels[xpos + ypos * winWidth] = col;
}


void SimpleUD()
{

  Window window;

  while (Controls::Update())
  {
    window.Clear();
    Camera::Update();


    if (CubeVisible(0, 0, 0, 10, 10, 10))
    {

    }


    window.Swap();
  }

};

#endif // SimpleUD_h__
