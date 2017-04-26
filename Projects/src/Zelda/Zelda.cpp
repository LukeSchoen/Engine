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

void Zelda()
{
  Threads::SetFastMode();  // Work in real time mode while loading

#ifdef _DEBUG
  Window window("Game", true, 640, 480, false); // Create Debug Game Window
#else
  Window window("Game", true, 1920, 1080, true); // Create Game Window
#endif


  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 1.0f, 50000.0f);

  PolyModel skybox;
  skybox.LoadModel(ASSETDIR "skybox/skybox.obj");

  window.Swap();
  PolyModel hyruleColorModel, hyruleLightModel;

  hyruleColorModel.LoadModel(ASSETDIR "zelda/Hyrule/Color/Hyrule.obj");
  hyruleLightModel.LoadModel(ASSETDIR "zelda/Hyrule/Light/Hyrule.obj");

  PolyMesh hyruleCollisionMesh;

  RenderObject *meshes;
  int64_t meshCount;
  hyruleColorModel.GetMeshData(&meshCount, &meshes);
  for (int m = 0; m < meshCount; m++)
  {
    int64_t vertexCount;
    const void *posData;
    meshes[m].AccessAttribute("position0", &vertexCount, nullptr, nullptr, &posData);
    vec3 *verts = (vec3*)posData;
    for (int i = 0; i < vertexCount / 3; i++)
      hyruleCollisionMesh.AddTriangle(Triangle(verts[i * 3 + 0], verts[i * 3 + 1], verts[i * 3 + 2]));
  }

  for (int m = 0; m < meshCount; m++)
    meshes[m].AssignShader(SHADERDIR "TexturedRenderObject.vert", ASSETDIR "zelda/shaders/Detail.frag");

  mat4 modelMat;
  Shaders::Report();

  float superSample = 2;

   // Frame buffering
  BufferObject hyruleColorBuffer(window.width * superSample, window.height * superSample);
  hyruleColorBuffer.AddPolyModel(&hyruleColorModel);
  hyruleColorBuffer.AddBuffer("fragColor");

  BufferObject hyruleLightBuffer(window.width  * superSample, window.height * superSample);
  hyruleLightBuffer.AddPolyModel(&hyruleLightModel);
  hyruleLightBuffer.AddBuffer("fragColor");

  RenderObject *finalRenderObject = FullScreenQuad(ASSETDIR "Zelda/shaders/lighting.frag");
  BufferObject finalRenderBuffer(window.width, window.height);
  finalRenderObject->AssignTexture("texture0", hyruleColorBuffer.GetBuffer("fragColor"));
  finalRenderObject->AssignTexture("texture1", hyruleLightBuffer.GetBuffer("fragColor"));
  finalRenderBuffer.AddRenderObject(finalRenderObject);
  finalRenderBuffer.AddBuffer("fragColor");

  Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);

    Camera::Update(40);

    // Skybox
    mat4 skyMVP;
    skyMVP = projectionMat * Camera::RotationMatrix();
    skyMVP.Transpose();
    glDepthMask(GL_FALSE);
    skybox.Render(skyMVP);
    glDepthMask(GL_TRUE);

    mat4 viewMat = Camera::Matrix();
    mat4 MVP;

    modelMat.LoadIdentity();
    MVP = projectionMat * viewMat * modelMat;
    //MVP.Transpose();

    // Create Hyrule color buffer
    float useDetail = 1;
    if (!Controls::KeyDown(SDL_SCANCODE_1)) useDetail = 0;
    hyruleColorModel.GetMeshData(&meshCount, &meshes); for (int m = 0; m < meshCount; m++) meshes[m].AssignUniform("useDetail", UT_1f, &useDetail);
    hyruleColorBuffer.Render(MVP);

    // Create Hyrule light buffer
    modelMat.LoadIdentity();
    modelMat.RotateX(270 * DegsToRads);
    MVP = projectionMat * viewMat * modelMat;
    //MVP.Transpose();
    hyruleLightBuffer.Render(MVP);

    mat4 identity;
    finalRenderBuffer.Render(identity);

    BufferObject::DisplayFullscreenTexture(finalRenderBuffer.GetBuffer("fragColor"));

    // Render Everything
    window.Swap(); // Swap Window
    FrameRate::Update();
  }
}
