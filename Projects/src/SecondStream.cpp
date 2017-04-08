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

void RecursiveRenderNode(const Octree &octree, const Node &node, const CPURenderer &renderer, const vec3 &camPos, float nodeSize, vec4 nodePos)
{
  const float detail = 256;

  float newNodeSize = nodeSize * 0.5;

  Point p;
  p.x = nodePos.x + newNodeSize;
  p.y = nodePos.y + newNodeSize;
  p.z = nodePos.z + newNodeSize;
  p.color = octree.GetColor(node);

  if (!octree.IsLeaf(node))
  {
    //if (!renderer.OnScreen(p, nodeSize * 2.8)) return;
    float dist = (vec3(nodePos) - camPos).Length();
    if (dist < nodeSize * detail)
    {
      // Near Split
      for (int i = 0; i < 8; i++)
      {
        Node child = octree.GetChild(node, i);
        //if (child != node)
        {
          if (octree.GetColor(child) == 0) continue;
          // Calculate new node pos here
          int xoff = (i & 1) > 0;
          int yoff = (i & 2) > 0;
          int zoff = (i & 4) > 0;
          RecursiveRenderNode(octree, child, renderer, camPos, newNodeSize, nodePos + vec4(xoff * newNodeSize, yoff * newNodeSize, zoff * newNodeSize, 0));
        }
      }
      return;
    }
  }
  // Far Draw
  renderer.DrawPoint(p, nodeSize * 2);
  FrameRate::AddDraw(1);
}


void SecondStream()
{
  //Threads::SetFastMode(); // Work in real time
  Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering
  Window window("SecondStream", false, 800, 600, false); // Create Game Window

  CPURenderer renderer;

  PointCloud pc("d:/temp/map.pcf"); // Open Point Cloud
  pc.pointCount = 1000000;
  Octree model(pc); // Create Octree
  model.SaveFile("d:/temp/fullmodel.oct");
  //Octree model("c:/temp/fullmodel.oct");

  // Create software depth buffer
  uint32_t *depthBuffer = new uint32_t[window.width * window.height];

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.01, 10000.0f);

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);
    Camera::Update(100);

    // Clear depth
    for (int i = 0; i < window.width * window.height; i++)
      depthBuffer[i] = 2000000000;

    // Model Matrix
    mat4 MVP;
    mat4 modelMat;
    mat4 viewMat = Camera::Matrix();
    MVP = projectionMat * viewMat * modelMat;
    //MVP.Transpose();

    renderer.PrepareFrame(MVP, window, depthBuffer);
    if (!Controls::KeyDown(SDL_SCANCODE_1))
    {
      // Octree render
      RecursiveRenderNode(model, model.RootNode(), renderer, vec3() - Camera::Position(), 4096, vec4(0, 0, 0, 1));
    }
    else
    {
      FrameRate::AddDraw(pc.pointCount);
      for (int64_t pItr = 0; pItr < pc.pointCount; pItr++)
        renderer.DrawPoint(pc.points[pItr]);
    }

    window.Swap(); // Swap Window

    FrameRate::Update();
  }
}

