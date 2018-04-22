#include "VoxelTracer.h"
#include "Window.h"
#include "Controls.h"
#include "FrameRate.h"
#include "Threads.h"
#include <stdlib.h>
#include "Camera.h"

void VoxelTracer()
{

  Threads::SetFastMode();

  Window window;

  Camera camera;

  const int worldWWdth = 256;
  const int worldHeight = 256;
  const int worldLength = 256;

  uint32_t *pWorld = new uint32_t[worldWWdth * worldHeight * worldLength];
  for (int i = 0; i < 1000; i++)
    pWorld[(rand() & 255) + (rand() & 255) * worldWWdth + (rand() & 255) * worldWWdth * worldHeight] = (rand() & 255) | ((rand() & 255) << 8) | ((rand() & 255) << 16);

  while (Controls::Update())
  {
    camera.Update();

    window.Clear();

    vec2 invWinSize = { 1.0f / window.width, 1.0f / window.height };

    for (int y = 0; y < window.height; y++)
      for (int x = 0; x < window.width; x++)
      {
        vec3 rayPos = camera.Position();
        vec3 rayDir = vec3(x * invWinSize.x * 2 - 1.f, y * invWinSize.y * 2 - 1.f, 2.4);

        camera.Rotation();

        
        window.pixels[x + y * window.width] = 0;

      }

    window.Swap();

    FrameRate::Update();

  }

}
