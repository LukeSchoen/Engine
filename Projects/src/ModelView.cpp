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
  static Texture *textures;

  int64_t meshCount;
  RenderObject *meshes;
  model.GetMeshData(&meshCount, &meshes);

  static bool started = false;
  if (!started)
  {
    textures = new Texture[meshCount];
    for (int64_t meshID = 0; meshID < meshCount; meshID++)
    {
      const char *path;
      if (meshes[meshID].textureCount > 0)
      {
        meshes[meshID].AccessTexture("texture0", &path);
        textures[meshID].CreateFromImage(path);
      }
    }
  }

  for (int64_t meshID = 0; meshID < meshCount; meshID++)
  {
    int64_t vertCount;
    uint32_t attribCount;
    GLAttributeType vertType;
    const void *PosData;
    const void *UVData;
    const void *ColData;
    meshes[meshID].AccessAttribute("position0", &vertCount, &attribCount, &vertType, &PosData);
    meshes[meshID].AccessAttribute("texcoord0", &vertCount, &attribCount, &vertType, &UVData);
    meshes[meshID].AccessAttribute("colour0", &vertCount, &attribCount, &vertType, &ColData);
    for (int64_t triID = 0; triID < vertCount / 3; triID++)
    {
      // Extract triangle
      uint32_t color = 11 | (134 << 8) | (184 << 16);
      //color = 0xFFFFFFFF;
      //uint32_t color = 255 * 256;
      vec3 v1 = ((vec3*)PosData)[triID * 3 + 0];
      vec3 v2 = ((vec3*)PosData)[triID * 3 + 1];
      vec3 v3 = ((vec3*)PosData)[triID * 3 + 2];

      vec3 c1 = ((vec3*)ColData)[triID * 3 + 0];
      vec3 c2 = ((vec3*)ColData)[triID * 3 + 1];
      vec3 c3 = ((vec3*)ColData)[triID * 3 + 2];

      vec2 uv1 = ((vec2*)UVData)[triID * 3 + 0];
      vec2 uv2 = ((vec2*)UVData)[triID * 3 + 1];
      vec2 uv3 = ((vec2*)UVData)[triID * 3 + 2];

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

      vec2 v1f(Min(Max(v1i.x * window.width, 0), window.width), Min(Max((1 - v1i.y) * window.height, 0), window.height));
      vec2 v2f(Min(Max(v2i.x * window.width, 0), window.width), Min(Max((1 - v2i.y) * window.height, 0), window.height));
      vec2 v3f(Min(Max(v3i.x * window.width, 0), window.width), Min(Max((1 - v3i.y) * window.height, 0), window.height));

      std::vector<Rasterizer::Vertex> pixels = rasterizer.RasterizeTriangle(v1f, v2f, v3f);
      for (auto &pixel : pixels)
      {
        int xPox = pixel.pos.x;
        int yPox = pixel.pos.y;
        if (xPox < 0 || xPox >= window.width || yPox < 0 || yPox >= window.height) continue; // Clip pixel
        vec3 c = (c1 * pixel.inf.x) + (c2 * pixel.inf.y) + (c3 * pixel.inf.z);
        vec2 uv = (uv1 * pixel.inf.x) + (uv2 * pixel.inf.y) + (uv3 * pixel.inf.z);

        c = { 1, 1, 1 };

        c = textures[meshID].BiLinearSample(uv, 0);

        color = (uint32_t) Max(c.x * 255, 0) | (uint32_t) Max(c.y * 255, 0) << 8 | (uint32_t) Max(c.z * 255, 0) << 16;

        window.pixels[xPox + yPox * window.width] = color;
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

  SDL_SetWindowPosition(window.window, 96, 240);
  //SDL_SetWindowPosition(windowSoft.window, 96 + window.width + 32, 240);

#endif
  Controls::SetMouseLock(true);
  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.1f, 1024.f * 32);

  window.Swap();

  ColouredRenderObjectMaker maker;
  maker.AddVertex(vec3(0, 0, 0), vec3(1, 0, 0));
  maker.AddVertex(vec3(1, 0, 0), vec3(0, 1, 0));
  maker.AddVertex(vec3(0, 0, 1), vec3(0, 0, 1));

  PolyModel model;
  //model.meshCount = 1;
  //model.pMeshes = maker.AssembleRenderObject();
  model.LoadModel("C:/Users/Luke/Desktop/Box/Box.obj", true);
  //model.LoadModel("C:/Users/lschoen/Desktop/Cat/Cat.obj");
  //model.LoadModel(ASSETDIR "Raster/Teapot.dae", true);

  while (Controls::Update())
  {
    window.Clear(0, 0, 0);
    //windowSoft.Clear(0, 0, 0);
    Camera::Update(10);

    mat4 viewMat = Camera::Matrix();
    mat4 MVP;
    mat4 modelMat;
    modelMat.Translate(vec3(0, 0, -3));
    modelMat.Rotate(vec3(DegsToRads * -90, 0, 0));
    MVP = projectionMat;
    //DrawPolyModelCPU(model, MVP, windowSoft);

    MVP.Transpose();
    model.Render(MVP);

    window.Swap();
    //windowSoft.Swap();
    FrameRate::Update();
  }
}
