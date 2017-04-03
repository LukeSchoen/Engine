#include "RayTracer.h"
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

struct Job
{
  Job(Window *a_window, int a_startX, int a_startY, int a_width, int a_height)
  {
    window = a_window;
    startX = a_startX;
    startY = a_startY;
    width = a_width;
    height = a_height;
  }
  Window *window;
  int startX, startY;
  int width, height;
};

struct ColoredSphere
{
  ColoredSphere(vec3 a_pos, float a_radius, uint32_t a_color = 0xFFFFFFFF)
    : m_pos(a_pos)
    , m_radius(a_radius)
    , m_color(a_color)
  {
    m_color = a_color;
  }

  ColoredSphere(vec3 a_pos, float a_radius, vec3 a_color = vec3(1, 1, 1))
    : m_pos(a_pos)
    , m_radius(a_radius)
  {
    uint8_t r, g, b;
    b = a_color.x * 255;
    g = a_color.y * 255;
    r = a_color.z * 255;
    m_color = r | (g << 8) | (b << 16);
  }

  vec3 m_pos;
  float m_radius;
  uint32_t m_color;
};

__declspec(noinline) bool RayOnSphere(vec3 rayPos, vec3 rayDir, const ColoredSphere &sphere, float *t)
{

  vec3 rayToSphere = rayPos - sphere.m_pos;
  float rayCo = rayToSphere.DotProduct(rayDir);
  float c = rayToSphere.DotProduct(rayToSphere) - sphere.m_radius * sphere.m_radius;

  // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
  if (c > 0.0f && rayCo > 0.0f)
    return false;
  float discr = rayCo*rayCo - c;

  // A negative discriminant corresponds to ray missing sphere 
  if (discr < 0.0f)
    return false;

  // Ray now found to intersect sphere, compute smallest t value of intersection
  *t = -rayCo - sqrt(discr);

  // If t is negative, ray started inside sphere so clamp t to zero 
  if (*t < 0.0f) *t = 0.0f;

  return true;
}

vec3 Reflect(vec3 dir, vec3 normal)
{
  return (dir - (normal * 2 * dir.DotProduct(normal))).Normalized();
}

bool TraceRay(const vec3 &rayPos, const vec3 &rayDir, const std::vector<ColoredSphere> &spheres, const vec3 &lightDirection, vec3 *hitPos, vec3 *hitNor, vec3 *hitCol)
{
  float closestDist = FLT_MAX;
  uint32_t closestColor = 0;
  vec3 closestSpherePos;
  for (auto &sphere : spheres)
  {
    float dist;
    if (RayOnSphere(rayPos, rayDir, sphere, &dist))
    {
      if(dist > 0.01)
        if (dist < closestDist)
        {
          closestDist = dist;
          closestColor = sphere.m_color;
          closestSpherePos = sphere.m_pos;
        }
    }
  }

  *hitPos = rayPos + rayDir * closestDist;

  if (closestColor > 0)
  {
    *hitNor = (*hitPos - closestSpherePos).Normalized();
    float lightAmt = ((*hitNor).DotProduct(lightDirection) + 1) * 0.5;

    float b = closestColor & 0xFF;
    float g = (closestColor >> 8) & 0xFF;
    float r = (closestColor >> 16) & 0xFF;
    b *= lightAmt / 255.f;
    g *= lightAmt / 255.f;
    r *= lightAmt / 255.f;
    *hitCol = vec3(r, g, b);
    return true;
  }
  else
  {
    if (rayDir.y < 0)
    {
      float distToFloor = rayPos.y / rayDir.y;
      *hitPos = rayPos - rayDir * distToFloor;
      *hitNor = vec3(0, 1, 0);
      int xpos = 10000 + (rayPos.x - rayDir.x * distToFloor);
      int zpos = 10000 + (rayPos.z - rayDir.z * distToFloor);
      if ((xpos & 1) ^ (zpos & 1))
      {
        distToFloor = -distToFloor;
        distToFloor = sqrt(distToFloor);
        float col = 1.0 / (1 + distToFloor * 2);
        *hitCol = vec3(col, col, col);
        return true;
      }
      *hitCol = vec3(0.02, 0.02, 0.02);
      return true;
    }
  }
  return false;
}

void RecursiveTraceRay(int iterations, const vec3 &rayPos, vec3 *rayDir, const std::vector<ColoredSphere> &spheres, const vec3 &lightDirection, vec3 *outCol)
{
  vec3 hitPos, hitNor, hitCol;
  if (TraceRay(rayPos, *rayDir, spheres, lightDirection, &hitPos, &hitNor, &hitCol))
  {
    if (iterations == 0)
    {
      *outCol = hitCol;
      return;
    }
    else
    {
      *rayDir = Reflect(*rayDir, hitNor);
      vec3 surface = hitPos;
      RecursiveTraceRay(iterations - 1, surface, rayDir, spheres, lightDirection, outCol);
      *outCol = (*outCol + hitCol) * 0.5;
    }
  }
  else
  {
    // Draw recursion step count
    //*outCol = vec3(1.0f - (iterations - 1) / 10.0f, 1.0f - (iterations - 1) / 10.0f, 1.0f - (iterations - 1) / 10.0f);
  }
}

void RayTraceSpheres(const Job &job, vec3 CamPos, vec2 CamDir, const std::vector<ColoredSphere> &spheres, vec3 lightDirection)
{
  const Window &window = *job.window;
  float invWidth = 1.0f / window.width;
  float invHeight = 1.0f / window.height;
  mat4 viewMat;
  //CamDir.x;
  viewMat.Rotate(CamDir.x, 0, 0);
  viewMat.Rotate(0, CamDir.y, 0);
  viewMat.Transpose();

  float aspect = window.height / (window.width + 0.f);
  uint32_t *pixels = window.pixels + job.startX + job.startY * window.width;
  for (int y = job.startY; y < job.startY + job.height; y++)
    for (int x = job.startX; x < job.startX + job.width; x++)
    {
      float nx = (float(x) * 2 - window.width) * invWidth;
      float ny = 0-(float(y) * 2 - window.height) * invHeight * aspect;
      vec3 rayDir(nx, ny, -1);
      rayDir = rayDir.Normalized();
      rayDir = viewMat * vec4(rayDir.x, rayDir.y, rayDir.z, 1);
      vec3 hitCol;

      RecursiveTraceRay(10, CamPos, &rayDir, spheres, lightDirection, &hitCol);

      hitCol = hitCol * 255.f;
      uint32_t finalCol = uint8_t(hitCol.z) | (uint8_t(hitCol.y) << 8) | (uint8_t(hitCol.x) << 16);
      (*pixels) = finalCol;
      pixels++;
    }
}

void RayTracer()
{
  const int threads = std::thread::hardware_concurrency();
  //Window window("RayTracer", false, 1920 / 1.5, 1080 / 1.5, true);
  Window window("RayTracer", false, 1200, 600, false);
  std::thread* threadArray = new std::thread[threads - 1];

  vec3 lightDirection(1, 1, 1);
  lightDirection = lightDirection.Normalized();

  std::vector<ColoredSphere> WorldSpheres;
  WorldSpheres.emplace_back(vec3(0, 3, 0), 3, vec3(1, 1, 0));
  for (int i = 0; i < 7; i++)
    WorldSpheres.emplace_back(vec3(rand() % 64 - 32, 3, rand() % 64 - 32), ((rand() % 90) + 10) * 0.02, vec3((rand() + 0.f) / RAND_MAX, (rand() + 0.f) / RAND_MAX, (rand() + 0.f) / RAND_MAX));

  //   { // Load Point Cloud
  //     const float desiredCount = 2000;
  //     PointCloud cloud;
  //     cloud.Load("D:/Temp/export/model.pcf");
  //     int stepSize = floor(cloud.pointCount / desiredCount);
  //     for (int64_t p = 0; p < cloud.pointCount; p += stepSize)
  //       WorldSpheres.emplace_back(vec3(cloud.points[p].x*0.02, cloud.points[p].y*0.02, cloud.points[p].z*0.02), 1, cloud.points[p].color);
  //     Camera::SetPosition(vec3() - vec3(cloud.points[0].x, cloud.points[0].y, cloud.points[0].z) * 0.02);
  //   }

//      { // Load Point Cloud
//        PointCloud cloud;
//        cloud.Load("D:/Temp/export/model.pcf");
//        for (int64_t p = 0; p < cloud.pointCount; p++)
//          if(cloud.points[p].x < 64 && cloud.points[p].y < 1024 & cloud.points[p].z < 64)
//          WorldSpheres.emplace_back(vec3(cloud.points[p].x, cloud.points[p].y, cloud.points[p].z), 1, cloud.points[p].color);
//        Camera::SetPosition(vec3() - vec3(cloud.points[0].x, cloud.points[0].y, cloud.points[0].z));
//      }

  Controls::SetMouseLock(true);

  while (Controls::Update()) // Main Game Loop
  {
    Camera::Update(1.f);
    vec3 pos = Camera::Position();
    pos.y = Min(pos.y, -0.1);
    Camera::SetPosition(pos);


    // Rotate Planets Around Sun
    for (int p = 1; p < WorldSpheres.size(); p++)
    {
      vec3 pos = WorldSpheres[p].m_pos;
      float rotSpeed = pos.Length();
      mat4 orbit;
      orbit.RotateY(0.02 / rotSpeed);
      WorldSpheres[p].m_pos = orbit * vec4(pos.x, pos.y, pos.z, 1.0f);
    }

    // Flip user if he enters a planet
    for (auto &Planet : WorldSpheres)
    {
      if ((((vec3() - Camera::Position()) - Planet.m_pos).Length() - 0.1) < Planet.m_radius)
      {
        vec3 newDir = Reflect(Camera::Direction(), ((vec3() - Camera::Position()) - Planet.m_pos).Normalized());

        Camera::SetRotation(newDir);

        while ((((vec3() - Camera::Position()) - Planet.m_pos).Length() - 0.1) < Planet.m_radius)
          Camera::SetPosition(Camera::Position() + Camera::Direction() * 0.05);
      }
    }

    const int hiresX = 1920 * 10;
    const int hiresY = 1080 * 10;
    if (Controls::KeyDown(SDL_SCANCODE_0))
    {
      window.width = hiresX;
      window.height = hiresY;
      window.pixels = new uint32_t[hiresX * hiresY];
    }

    for (int i = 0; i < threads - 1; i++)
      threadArray[i] = std::thread(&RayTraceSpheres, Job(&window, 0, window.height / threads * (i + 1), window.width, window.height / threads), vec3() - Camera::Position(), Camera::GetRotation(), WorldSpheres, lightDirection);

    RayTraceSpheres(Job(&window, 0, 0, window.width, window.height / threads), vec3() - Camera::Position(), Camera::GetRotation(), WorldSpheres, lightDirection);

    for (int i = 0; i < threads - 1; i++)
      threadArray[i].join();

    if (Controls::KeyDown(SDL_SCANCODE_0))
    {
      for (int y = 0; y < window.height; y++)
        for (int x = 0; x < window.width; x++)
        {
          uint32_t &c = window.pixels[x + y * window.width];
          uint8_t r, g, b;
          r = c & 0xFF;
          g = (c >> 8) & 0xFF;
          b = (c >> 16) & 0xFF;
          c = b | (g << 8) | (r << 16) | (0xFF << 24);
        }
      ImageFile::WriteImagePNG("Render.png", window.pixels, window.width, window.height);
      system("Render.png");
      exit(0);
    }

    window.Swap(); // Swap Window
    //window2.Swap(); // Swap Window
    FrameRate::AddDraw(WorldSpheres.size());
    FrameRate::Update();
  }
}

