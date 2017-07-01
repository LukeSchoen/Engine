#include "SecondStream.h"
#include "Window.h"
#include "Threads.h"
#include "Maths.h"
#include "FrameRate.h"
#include "Controls.h"
#include "Camera.h"
#include "StreamFile.h"
#include "File.h"
#include <stdlib.h>
#include "PointCloud.h"
#include "Octree.h"
#include "MatVecSIMD.h"
#include "CPUDraw.h"

#include "C:\Luke\Programming\Engine\Projects\Accelerator\Accelerator.h"

void SecondStream()
{
  //Threads::SetFastMode(); // Work in real time
  Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering
  Window window("SecondStream", false, 800, 600, false); // Create Game Window

  PointCloud pc("f:/temp/CarrickHill.pcf"); // Open Point Cloud
  //pc.pointCount = 1000;

  vec3 *in = (vec3*)_mm_malloc(pc.pointCount * sizeof(vec3), 16);
  vec3 *out = (vec3*)_mm_malloc(pc.pointCount * sizeof(vec3), 16);


  vec4 *positions = new vec4[pc.pointCount];
  vec4 *positions2 = new vec4[pc.pointCount];
  for (int64_t pItr = 0; pItr < pc.pointCount; pItr++)
  {
    positions[pItr] = vec4(pc.points[pItr].x, pc.points[pItr].y, pc.points[pItr].z, 1);
    in[pItr] = vec3(pc.points[pItr].x, pc.points[pItr].y, pc.points[pItr].z);
  }

  uint32_t *depthBuffer = new uint32_t[window.width * window.height];

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.01, 10000.0f);

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 0, 0);
    Camera::Update(50000);

    // Model Matrix
    mat4 MVP;
    mat4 modelMat;
    mat4 viewMat = Camera::Matrix();
    MVP = projectionMat * viewMat * modelMat;
    //MVP.Transpose();
    FrameRate::AddDraw(pc.pointCount);
    if(Controls::KeyDown(SDL_SCANCODE_1))
    {
      for (int64_t pItr = 0; pItr < pc.pointCount; pItr++)
      {
        vec4 pos = positions[pItr] * MVP;
        int xpos = (0.5f + (pos.x / pos.w * 0.5f)) * window.width;
        int ypos = (0.5f - (pos.y / pos.w * 0.5f)) * window.height;
        if (xpos >= 0 && xpos < window.width)
          if (ypos >= 0 && ypos < window.height)
            window.pixels[xpos + ypos * window.width] = pc.points[pItr].color;
      }
    }
    else
    {
      MatVec(out, MVP, in, pc.pointCount);
      //for (int64_t pItr = 0; pItr < pc.pointCount; pItr++)
      //{
      //  positions2[pItr] = positions[pItr] * MVP;
      //}
    }

    window.Swap(); // Swap Window

    FrameRate::Update();
  }
}

