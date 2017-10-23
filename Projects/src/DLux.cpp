#include "Window.h"
#include "Threads.h"
#include "FrameRate.h"
#include "Controls.h"
#include "Camera.h"
#include <stdlib.h>
#include "Maths.h"
#include "PolyModel.h"
#include "Textures.h"
#include "Shaders.h"
#include <vector>
#include "Audio.h"
#include <thread>
#include <mutex>
#include "PointCloud.h"
#include "ImageFile.h"
#include "RayTracer.h"
#include "DLux.h"


void KeepCameraAboveGround()
{
  Camera::Update(1.f);
  vec3 pos = Camera::Position();
  pos.y = Min(pos.y, -0.1);
  Camera::SetPosition(pos);
}

void RotatePlanetsAroundSun(RayTracer *tracer)
{
  for (int p = 1; p < tracer->WorldSpheres.size(); p++)
  {
    vec3 pos = tracer->WorldSpheres[p].m_pos;
    float rotSpeed = pos.Length();
    mat4 orbit;
    orbit.RotateY(0.02 / rotSpeed);
    tracer->WorldSpheres[p].m_pos = orbit * vec4(pos.x, pos.y, pos.z, 1.0f);
  }
}

void KeepCameraOutOfPlanets(RayTracer *tracer)
{
  for (auto &Planet : tracer->WorldSpheres)
  {
    if ((((vec3() - Camera::Position()) - Planet.m_pos).Length() - 0.1) < Planet.m_radius)
    {
      vec3 newDir = ReflectRay(Camera::Direction(), ((vec3() - Camera::Position()) - Planet.m_pos).Normalized());

      Camera::SetRotation(newDir);

      while ((((vec3() - Camera::Position()) - Planet.m_pos).Length() - 0.1) < Planet.m_radius)
        Camera::SetPosition(Camera::Position() + Camera::Direction() * 0.05);
    }
  }
}

void StartScreenShot(RayTracer *tracer)
{
  const int hiresX = 1920 * 10;
  const int hiresY = 1080 * 10;
  if (Controls::KeyDown(SDL_SCANCODE_0))
  {
    tracer->window->width = hiresX;
    tracer->window->height = hiresY;
    tracer->window->pixels = new uint32_t[hiresX * hiresY];
  }
}

void FinishScreenShot(RayTracer *tracer)
{
  if (Controls::KeyDown(SDL_SCANCODE_0))
  {
    for (int y = 0; y < tracer->window->height; y++)
      for (int x = 0; x < tracer->window->width; x++)
      {
        uint32_t &c = tracer->window->pixels[x + y * tracer->window->width];
        uint8_t r, g, b;
        r = c & 0xFF;
        g = (c >> 8) & 0xFF;
        b = (c >> 16) & 0xFF;
        c = b | (g << 8) | (r << 16) | (0xFF << 24);
      }
    ImageFile::WriteImagePNG("Render.png", tracer->window->pixels, tracer->window->width, tracer->window->height);
    system("Render.png");
    exit(0);
  }
}

void DLux()
{
  // Setup Window
  Controls::SetMouseLock(true);
  Window window("RayTracer", false, 1200, 600, false);
  //Window window("RayTracer", false, 1920/1.5, 1080/1.5, true);

  {
    // Create Ray tracer
    RayTracer tracer(&window);

    // Main Game Loop
    while (Controls::Update())
    {
      KeepCameraAboveGround();
      RotatePlanetsAroundSun(&tracer);
      KeepCameraOutOfPlanets(&tracer);
      StartScreenShot(&tracer);

      tracer.Draw();

      FinishScreenShot(&tracer);
      FrameRate::AddDraw(tracer.WorldSpheres.size());
      FrameRate::Update();
      window.Swap();
    }
  }

}