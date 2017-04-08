#include "RayTracer.h"
#include "Camera.h"
#include <synchapi.h>

RayTracer::RayTracer(Window *a_window)
{
  window = a_window;

  // Create World
  WorldSpheres.emplace_back(vec3(0, 3, 0), 3, vec3(1, 1, 0));
  for (int i = 0; i < 7; i++)
    WorldSpheres.emplace_back(vec3(rand() % 64 - 32, 3, rand() % 64 - 32), ((rand() % 90) + 10) * 0.02, vec3((rand() + 0.f) / RAND_MAX, (rand() + 0.f) / RAND_MAX, (rand() + 0.f) / RAND_MAX));

  // Create Light
  lightDirection = vec3(1, 1, 1).Normalized();

  // Setup Threads
  threadCount = std::thread::hardware_concurrency();
  threadArray = new std::thread[threadCount];

  lockArray = new std::mutex[threadCount];
  workingArray = new bool[threadCount];
  doneArray = new bool[threadCount];
  closeArray = new bool[threadCount];

  for (int c = 0; c < threadCount; c++)
  {
    workingArray[c] = false;
    doneArray[c] = false;
    closeArray[c] = false;
  }

  for (int i = 0; i < threadCount; i++)
    threadArray[i] = std::thread(&_StartDrawSection, this, Job(i, 0, window->height / threadCount * i, window->width, window->height / threadCount), lightDirection);
}

RayTracer::~RayTracer()
{
  for (int t = 0; t < threadCount; t++) lockArray[t].lock();
  for (int t = 0; t < threadCount; t++) closeArray[t] = true;
  for (int t = 0; t < threadCount; t++) lockArray[t].unlock();
  while (true)
  {
    bool ready = true;
    for (int t = 0; t < threadCount; t++) if (closeArray[t]) ready = false;
    if (ready) break;
    Sleep(0);
  }
}

void RayTracer::Draw()
{
  for (int t = 0; t < threadCount; t++) lockArray[t].lock();
  vec3 p = vec3() - Camera::Position();
  vec2 r = Camera::GetRotation();

  camPos.x = p.x;
  camPos.y = p.y;
  camPos.z = p.z;

  camRot.x = r.x;
  camRot.y = r.y;

  for (int t = 0; t < threadCount; t++) doneArray[t] = false;
  for (int t = 0; t < threadCount; t++) workingArray[t] = true;
  for (int t = 0; t < threadCount; t++) lockArray[t].unlock();

  while (true)
  {
    bool ready = true;
    for (int t = 0; t < threadCount; t++) if (!doneArray[t]) ready = false;
    if (ready) break;
    //Sleep(0);
  }
}

void RayTracer::_StartDrawSection(RayTracer *tracer, const Job &job, vec3 lightDirection)
{
  tracer->DrawSection(job, lightDirection);
}

void RayTracer::DrawSection(const Job &job, vec3 lightDirection)
{
  while (true)
  {
    lockArray[job.jobNumber].lock();
    if (closeArray[job.jobNumber])
    {
      closeArray[job.jobNumber] = false;
      return;
     }
    
    if (!workingArray[job.jobNumber])
    {
      lockArray[job.jobNumber].unlock();
      Sleep(0);
      continue;
    }

    float invWidth = 1.0f / window->width;
    float invHeight = 1.0f / window->height;

    mat4 viewMat;

    viewMat.Rotate(camRot.x, 0, 0);
    viewMat.Rotate(0, camRot.y, 0);
    viewMat.Transpose();

    vec3 rayPos(camPos.x, camPos.y, camPos.z);

    float aspect = window->height / (window->width + 0.f);
    uint32_t *pixels = window->pixels + job.startX + job.startY * window->width;
    for (int y = job.startY; y < job.startY + job.height; y++)
      for (int x = job.startX; x < job.startX + job.width; x++)
      {
        float nx = (float(x) * 2 - window->width) * invWidth;
        float ny = 0 - (float(y) * 2 - window->height) * invHeight * aspect;
        vec3 rayDir(nx, ny, -1);
        rayDir = rayDir.Normalized();
        rayDir = viewMat * vec4(rayDir.x, rayDir.y, rayDir.z, 1);
        vec3 hitCol;

        RecursiveTraceRay(10, rayPos, &rayDir, lightDirection, &hitCol);

        hitCol = hitCol * 255.f;
        uint32_t finalCol = uint8_t(hitCol.z) | (uint8_t(hitCol.y) << 8) | (uint8_t(hitCol.x) << 16);
        (*pixels) = finalCol;
        pixels++;
      }

    workingArray[job.jobNumber] = false;
    doneArray[job.jobNumber] = true;
    lockArray[job.jobNumber].unlock();
  }

}

void RayTracer::RecursiveTraceRay(int iterations, const vec3 &rayPos, vec3 *rayDir, const vec3 &lightDirection, vec3 *outCol)
{
  vec3 hitPos, hitNor, hitCol;
  if (RayTrace(rayPos, *rayDir, lightDirection, &hitPos, &hitNor, &hitCol))
  {
    if (iterations == 0)
    {
      *outCol = hitCol;
      return;
    }
    else
    {
      *rayDir = ReflectRay(*rayDir, hitNor);
      vec3 surface = hitPos;
      RecursiveTraceRay(iterations - 1, surface, rayDir, lightDirection, outCol);
      *outCol = (*outCol + hitCol) * 0.5;
    }
  }
  else
  {
    // Draw recursion step count
    //*outCol = vec3(1.0f - (iterations - 1) / 10.0f, 1.0f - (iterations - 1) / 10.0f, 1.0f - (iterations - 1) / 10.0f);
  }
}

bool RayTracer::RayTrace(const vec3 &rayPos, const vec3 &rayDir, const vec3 &lightDirection, vec3 *hitPos, vec3 *hitNor, vec3 *hitCol)
{
  float closestDist = FLT_MAX;
  uint32_t closestColor = 0;
  vec3 closestSpherePos;
  for (auto &sphere : WorldSpheres)
  {
    float dist;
    if (sphere.RayOnSphere(rayPos, rayDir, &dist))
    {
      if (dist > 0.01)
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

