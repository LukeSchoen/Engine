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

  RenderObject *meshes;
  int64_t meshCount;

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 1.0f, 50000.0f);

  //PolyModel skybox;
  //skybox.LoadModel("../Assets/skybox/skybox.obj");

  window.Swap();
  PolyModel model, model2, lightModel;

  //model.LoadModel("../Assets/Hyrule Map/Over_World.obj");
  model.LoadModel(ASSETDIR "zelda/Hyrule/Diffuse/Hyrule.obj");
  model2.LoadModel(ASSETDIR "zelda/Hyrule/Diffuse/Hyrule.obj");
  lightModel.LoadModel(ASSETDIR "zelda/Hyrule/light2/Hyrule.obj");

  model2.GetMeshData(&meshCount, &meshes);
  for (int m = 0; m < meshCount; m++)
    meshes[m].AssignShader(SHADERDIR "TexturedRenderObject.vert", ASSETDIR "zelda/shaders/Detail.frag");

  mat4 modelMat;
  Shaders::Report();

  float superSample = 2;

   // Frame buffering
  BufferObject worldDiffuseDetail(window.width * superSample, window.height * superSample);
  model2.GetMeshData(&meshCount, &meshes); for (int m = 0; m < meshCount; m++) worldDiffuseDetail.AddRenderObject(&meshes[m]);
  worldDiffuseDetail.AddBuffer("fragColor");

  BufferObject worldDiffuse(window.width * superSample, window.height * superSample);
  model.GetMeshData(&meshCount, &meshes); for (int m = 0; m < meshCount; m++) worldDiffuse.AddRenderObject(&meshes[m]);
  worldDiffuse.AddBuffer("fragColor");

  BufferObject worldLight(window.width  * superSample, window.height * superSample);
  lightModel.GetMeshData(&meshCount, &meshes); for (int m = 0; m < meshCount; m++) worldLight.AddRenderObject(&meshes[m]);
  worldLight.AddBuffer("fragColor");

  RenderObject *finalRenderObject = FullScreenQuad(ASSETDIR "Zelda/shaders/lighting.frag");
  BufferObject finalRenderBuffer(window.width, window.height);
  finalRenderObject->AssignTexture("texture0", worldDiffuseDetail.GetBuffer("fragColor"));
  finalRenderObject->AssignTexture("texture1", worldLight.GetBuffer("fragColor"));
  finalRenderBuffer.AddRenderObject(finalRenderObject);
  finalRenderBuffer.AddBuffer("fragColor");


  Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);
    Camera::Update(60);

    // Skybox
    //Textures::SetTextureMode(false);
    mat4 skyMVP;
    skyMVP = projectionMat * Camera::RotationMatrix();
    skyMVP.Transpose();
    glDepthMask(GL_FALSE);
    //skybox.Render(skyMVP);
    glDepthMask(GL_TRUE);

    mat4 viewMat = Camera::Matrix();
    mat4 MVP;

    modelMat.LoadIdentity();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();

    //superSampler.Render(MVP);

    // Render World
    if (Controls::KeyDown(SDL_SCANCODE_1))
      worldDiffuse.Render(MVP);
    worldDiffuseDetail.Render(MVP);
    modelMat.LoadIdentity();
    modelMat.RotateX(270 * DegsToRads);
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    if (!Controls::KeyDown(SDL_SCANCODE_1))
      worldLight.Render(MVP);

    //BufferObject::DisplayFullscreenTexture(worldDiffuseDetail.GetBuffer("fragColor"));
    mat4 identity;
    finalRenderBuffer.Render(identity);

    if (Controls::KeyDown(SDL_SCANCODE_1))
      BufferObject::DisplayFullscreenTexture(worldDiffuse.GetBuffer("fragColor"));
    if (Controls::KeyDown(SDL_SCANCODE_2))
      BufferObject::DisplayFullscreenTexture(worldLight.GetBuffer("fragColor"));
    else
      BufferObject::DisplayFullscreenTexture(finalRenderBuffer.GetBuffer("fragColor"));

    // Render Everything
    window.Swap(); // Swap Window
    FrameRate::Update();
  }

  //model.Destroy();
}
