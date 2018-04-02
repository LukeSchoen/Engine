#include "Tracey.h"
#include "Window.h"
#include "Controls.h"
#include "Camera.h"

  int worldWidth = 256;
  int worldHeight = 256;
  int worldLength = 256;

  uint32_t *world = new uint32_t[worldWidth * worldHeight * worldWidth];

void RayTrace(uint32_t *pPixel, float sx, float sy, float ex, float ey)
{
  bool swapped = false;
  if (abs(sx - ex) > abs(sy - ey))
  {
    std::swap(sx, sy);
    std::swap(ex, ey);
    swapped = true;
  }
  int d = ((ey > sy) << 1) - 1;
  if (swapped)
  {
    float r = (ex - sx) / (ey - sy) * d;
      for (int y = (int)sy; y != (int)ey; y += d)
      {
        world[int(sx += r) + y * worldWidth];
        *pPixel;

      }
  }
  else
  {
    float r = (ex - sx) / (ey - sy) * d;
      for (int y = (int)sy; y != (int)ey; y += d)
      {
        //world[y + int(sx += r) * width];
        //*pPixel;

      }
  }
}


void Tracey()
{
  Window window("Tracey");
  float aspectRatio = window.width / (float)window.height;
  //float FOV = 45 * DegsToRads;


  while (Controls::Update())
  {
    window.Clear();

    Camera::Update();

    vec3 camPos = (vec3() - Camera::Position());


    for (int y = 0; y < window.height; y++)
      for (int x = 0; x < window.width; x++)
      {
        vec3 rayDirection(0 - (x / (float)window.width) * aspectRatio, 0 - (y / (float)window.height), -1.f);

        vec3 rayPos = camPos;

        //Rays();

        //window.pixels[x + y * window.width] = ;
      }

    window.Swap();
  }



}

