#include "Window.h"
#include "Threads.h"
#include "FrameRate.h"
#include "Controls.h"
#include "Camera.h"
#include <stdlib.h>
#include "RenderObject.h"
#include "Polymodel.h"
#include "Maths.h"
#include "Matrix4x4.h"
#include "Shaders.h"
#include "Chunk.h"
#include "Convertor.h"
#include "Textures.h"
#include "Framebuffer.h"
#include "Effect Object.h"
#include "Assets.h"
#include "Rasterizer.h"

void DrawPolyModelCPU(PolyModel &model, const mat4 &MVP, Window &window)
{
  static Rasterizer rasterizer;

  int64_t meshCount;
  RenderObject *meshes;
  model.GetMeshData(&meshCount, &meshes);
  for (int64_t meshID = 0; meshID < meshCount; meshID++)
  {
    int64_t vertCount;
    uint32_t attribCount;
    GLAttributeType vertType;
    const void *PosData;
    meshes[meshID].AccessAttribute("position0", &vertCount, &attribCount, &vertType, &PosData);
    for (int64_t triID = 0; triID < vertCount / 3; triID++)
    {
      // Extract triangle
      uint32_t color = 11 | (134 << 8) | (184 << 16);
      //color = 0xFFFFFFFF;
      //uint32_t color = 255 * 256;
      vec3 v1 = ((vec3*)PosData)[triID * 3 + 0];
      vec3 v2 = ((vec3*)PosData)[triID * 3 + 1];
      vec3 v3 = ((vec3*)PosData)[triID * 3 + 2];

      vec4 v1i(v1.x, v1.y, v1.z, 1);
      vec4 v2i(v2.x, v2.y, v2.z, 1);
      vec4 v3i(v3.x, v3.y, v3.z, 1);

      // Project Triangle
      v1i = MVP * v1i;
      v2i = MVP * v2i;
      v3i = MVP * v3i;

      v1i = (v1i / v1i.w) * 0.5 + 0.5;
      v2i = (v2i / v2i.w) * 0.5 + 0.5;
      v3i = (v3i / v3i.w) * 0.5 + 0.5;

      vec2 v1f(Min(Max(v1i.x * window.width, 0), window.width), Min(Max((1-v1i.y) * window.height, 0), window.height));
      vec2 v2f(Min(Max(v2i.x * window.width, 0), window.width), Min(Max((1-v2i.y) * window.height, 0), window.height));
      vec2 v3f(Min(Max(v3i.x * window.width, 0), window.width), Min(Max((1-v3i.y) * window.height, 0), window.height));

      vec2 off(0, -0.5f);

      // Rasterize
      std::vector<vec2i> pixels = rasterizer.RasterizeTriangle(v1f + off, v2f + off, v3f + off);
      for (auto &pixel : pixels)
      {
        if(pixel.x < 0 || pixel.x >= window.width || pixel.y < 0 || pixel.y >= window.height) continue;
        window.pixels[pixel.x + pixel.y * window.width] = color; // Clip pixel
      }
    }
  }
}

void ModelView()
{
  bool acceleration = true;

#ifdef _DEBUG
  Window window("Game", acceleration, 800, 600, false);
  Window windowSoft("Software", false, 800, 600, false);
#else
  Window window("OpenGL", true, 800, 600, false);
  //Window windowSoft("Software", false, 800, 600, false);
#endif
  Controls::SetMouseLock(true);
  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 1.0f, 500000.0f);

  window.Swap();

  PolyModel model;
  model.LoadModel("C:/Users/Luke/Desktop/map/map.obj", true);
  //model.LoadModel("C:/Users/Luke/Documents/3DReaperDX/Frames/out/Out.obj", true);

  while (Controls::Update())
  {
    window.Clear(0, 0, 0);
    //windowSoft.Clear(0, 0, 0);
    Camera::Update(1000);
    
    mat4 viewMat = Camera::Matrix();
    mat4 MVP;
    mat4 modelMat;
    modelMat.Translate(vec3(0, 0, -30));
    modelMat.Rotate(vec3(DegsToRads * -90, 0, 0));
    MVP = projectionMat * viewMat * modelMat;
    //DrawPolyModelCPU(model, MVP, windowSoft);

    MVP.Transpose();
    model.Render(MVP);

    window.Swap();
    //windowSoft.Swap();
    FrameRate::Update();
  }
}
