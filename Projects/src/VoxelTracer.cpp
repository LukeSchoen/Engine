#include "VoxelTracer.h"
#include "Window.h"
#include "Controls.h"
#include "FrameRate.h"
#include "Threads.h"
#include <stdlib.h>
#include "Camera.h"
#include "Assets.h"
#include "ImageFile.h"

void VoxelTracer()
{
  Threads::SetFastMode();

  Window window("pos", false, 400, 300);

  Camera camera;

  const int worldWdth = 256;
  const int worldHeight = 256;
  const int worldLength = 256;

  camera.SetPosition(vec3(-128, -128, -128));

  uint32_t *pWorld = new uint32_t[worldWdth * worldHeight * worldLength];

  // Add random colord dots to world
  for (int i = 0; i < 1000; i++)
    pWorld[(rand() & 255) + (rand() & 255) * worldWdth + (rand() & 255) * worldWdth * worldHeight] = (rand() & 255) | ((rand() & 255) << 8) | ((rand() & 255) << 16);

  uint32_t w, h;
  uint32_t *pImg = ImageFile::ReadImage(ASSETDIR "voxeltracer/brick.png", &w, &h);

  // Put walls around world
  for (int x = 0; x < worldWdth; x++)
    for (int y = 0; y < worldHeight; y++)
    {
      pWorld[x + y * worldWdth] = pImg[x + (255 - y) * 256];
      pWorld[x + y * worldWdth + 255 * worldWdth * worldHeight] = pImg[y + (255 - x) * 256];
      pWorld[x + y * worldWdth * worldHeight] = pImg[y + (255 - x) * 256];
      pWorld[x + y * worldWdth * worldHeight + 255 * worldWdth] = pImg[x + (255 - y) * 256];
      pWorld[x * worldWdth + y * worldWdth * worldHeight] = pImg[x + (255 - y) * 256];
      pWorld[x * worldWdth + y * worldWdth * worldHeight] = pImg[x + (255 - y) * 256 + 255];
    }

  Controls::SetMouseLock(true);

  while (Controls::Update())
  {
    camera.Update();
    window.Clear();

    float sinX = sin(camera.Rotation().x);
    float cosX = cos(camera.Rotation().x);
    float sinY = sin(camera.Rotation().y);
    float cosY = cos(camera.Rotation().y);

    for (int y = 0; y < window.height; y++)
      for (int x = 0; x < window.width; x++)
      {
        // Create Ray
        vec3 rayPos = vec3() - camera.Position();
        vec3 rayDir = vec3(x * (1.0 / window.width) * 2 - 1.f, 0 - (y * (1.0 / window.height) * 2 - 1.f), -2.4);

        // Rotate Ray by camera direction
        vec3 rayDirProj = rayDir;
        rayDir.y = (((rayDirProj.y * cosX)) + ((rayDirProj.z * sinX)));
        rayDir.z = (((rayDirProj.y * (-sinX))) + ((rayDirProj.z * cosX)));
        rayDir.x = (((rayDirProj.x * cosY)) + ((rayDir.z * sinY)));
        rayDir.z = (((rayDirProj.x * (-sinY))) + ((rayDir.z * cosY)));

        uint32_t color = 0;

        if (false)
        {
          // Magic tracer
          int majorAxis = 0;
          if (abs(rayDir.y) > abs(rayDir.x) && abs(rayDir.y) > abs(rayDir.z)) majorAxis = 1;
          if (abs(rayDir.z) > abs(rayDir.x) && abs(rayDir.z) > abs(rayDir.y)) majorAxis = 2;
          float majorDirection = rayDir.x;
          if (majorAxis == 1) majorDirection = rayDir.y;
          if (majorAxis == 2) majorDirection = rayDir.z;
          int d = ((majorDirection > 0) << 1) - 1;

          if (majorAxis == 0)
          {
            float yr = rayDir.y / rayDir.x * d;
            float zr = rayDir.z / rayDir.x * d;
            while (color == 0)
            {
              rayPos.x += d;
              rayPos.y += yr;
              rayPos.z += zr;
              if (rayPos.x < 0 || rayPos.x >= worldWdth || rayPos.y < 0 || rayPos.y >= worldWdth || rayPos.z < 0 || rayPos.z >= worldWdth) { color = 0; break; }
              color = pWorld[int(rayPos.x) + int(rayPos.y) * worldWdth + int(rayPos.z) * worldWdth * worldHeight];
            }
          }

          if (majorAxis == 1)
          {
            float xr = rayDir.x / rayDir.y * d;
            float zr = rayDir.z / rayDir.y * d;
            while (color == 0)
            {
              rayPos.y += d;
              rayPos.x += xr;
              rayPos.z += zr;
              if (rayPos.x < 0 || rayPos.x >= worldWdth || rayPos.y < 0 || rayPos.y >= worldWdth || rayPos.z < 0 || rayPos.z >= worldWdth) { color = 0; break; }
              color = pWorld[int(rayPos.x) + int(rayPos.y) * worldWdth + int(rayPos.z) * worldWdth * worldHeight];
            }
          }

          if (majorAxis == 2)
          {
            float xr = rayDir.x / rayDir.z * d;
            float yr = rayDir.y / rayDir.z * d;
            while (color == 0)
            {
              rayPos.z += d;
              rayPos.x += xr;
              rayPos.y += yr;
              if (rayPos.x < 0 || rayPos.x >= worldWdth || rayPos.y < 0 || rayPos.y >= worldWdth || rayPos.z < 0 || rayPos.z >= worldWdth) { color = 0; break; }
              color = pWorld[int(rayPos.x) + int(rayPos.y) * worldWdth + int(rayPos.z) * worldWdth * worldHeight];
            }
          }
        }

        if (false)
        {
          // Step Tracer
          int dist = 0;
          while (dist < 100 && color == 0)
          {
            dist++;
            rayPos = rayPos + (rayDir * 0.1f);
            if (rayPos.x < 0 || rayPos.x >= worldWdth || rayPos.y < 0 || rayPos.y >= worldWdth || rayPos.z < 0 || rayPos.z >= worldWdth) { color = 0; break; }
            color = pWorld[int(rayPos.x) + int(rayPos.y) * worldWdth + int(rayPos.z) * worldWdth * worldHeight];
          }
        }

        if (true)
        {
          // DDA Tracer
          vec3 pos = rayPos;
          vec3i attitude = vec3i((rayDir.x > 0) * 2 - 1, (rayDir.y > 0) * 2 - 1, (rayDir.z > 0) * 2 - 1);
          vec3i wholePos(floor(pos.x ), floor(pos.y), floor(pos.z));

          if (abs(rayDir.x) < FLT_EPSILON * 100) rayDir.x = FLT_EPSILON * 100;
          if (abs(rayDir.y) < FLT_EPSILON * 100) rayDir.y = FLT_EPSILON * 100;
          if (abs(rayDir.z) < FLT_EPSILON * 100) rayDir.z = FLT_EPSILON * 100;
          
          vec3 invDir = vec3(1) / rayDir;
          while (color == 0)
          {
            // Calculate dimensional crossing
            vec3 crossDist = ((vec3(wholePos) + vec3(attitude)) - pos) * invDir;

            // Determine first dimension crossed
            int FirstCross = (((crossDist.x >= crossDist.y)*(crossDist.z > crossDist.y))) + (((crossDist.x >= crossDist.z)*(crossDist.y >= crossDist.z)) * 2);

            // Step across dimensions
            switch (FirstCross)
            {
            case 0:
              wholePos.x += attitude.x;
              pos += rayDir * crossDist.x;
              break;
            case 1:
              wholePos.y += attitude.y;
              pos += rayDir * crossDist.y;
              break;
            case 2:
              wholePos.z += attitude.z;
              pos += rayDir * crossDist.z;
              break;
            }
            if (wholePos.x < 0 || wholePos.x >= worldWdth || wholePos.y < 0 || wholePos.y >= worldWdth || wholePos.z < 0 || wholePos.z >= worldWdth)
            {
              color = 0;
              break;
            }
            color = pWorld[int(pos.x) + int(pos.y) * worldWdth + int(pos.z) * worldWdth * worldHeight];
            if (color)
              break;
          }
        }
        window.pixels[x + y * window.width] = color;
      }
    window.Swap();
    FrameRate::Update();
  }

}
