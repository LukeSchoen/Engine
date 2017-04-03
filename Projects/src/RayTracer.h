#ifndef RayTracer_h__
#define RayTracer_h__
#include "RayTracerTypes.h"
#include <thread>
#include <mutex>

class RayTracer
{
private:
  static void _StartDrawSection(RayTracer *tracer, const Job &job, vec3 lightDirection);

  void DrawSection(const Job &job, vec3 lightDirection);

  bool RayTrace(const vec3 &rayPos, const vec3 &rayDir, const vec3 &lightDirection, vec3 *hitPos, vec3 *hitNor, vec3 *hitCol);

  void RecursiveTraceRay(int iterations, const vec3 &rayPos, vec3 *rayDir, const vec3 &lightDirection, vec3 *outCol);

public:

  RayTracer(Window *a_window);

  ~RayTracer();

  void Draw();

  Window *window;
  vec3 lightDirection;
  std::thread *threadArray;
  std::mutex *lockArray;
  volatile bool *workingArray;
  volatile bool *doneArray;
  volatile bool *closeArray;
  volatile vec3 camPos;
  volatile vec2 camRot;
  int threadCount = 1;
  std::vector<ColoredSphere> WorldSpheres;

};

#endif // RayTracer_h__
