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
#include "Textures.h"
#include "Framebuffer.h"
#include "CustomRenderObjects.h"
#include "Assets.h"

void Pokemon()
{
  Threads::SetFastMode();  // Work in real time mode while loading

#ifdef _DEBUG
  Window window("Game", true, 640, 480, false); // Create Debug Game Window
#else
                           //Window window("Game", true, 1920, 1080, true); // Create Game Window
  Window window("Game", true, 1920, 1080, false); // Create Game Window
#endif
  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 50.0f, 50000.0f);

  PolyModel skybox;
  skybox.LoadModel("../Assets/skybox/skybox.obj");

  window.Swap();
  PolyModel model;
  //model.LoadModel("../Assets/Hyrule Map/Over_World.obj");

  mat4 modelMat;
  Shaders::Report();

  TexturedRenderObjectMaker maker;
  maker.AddVertex(vec3(-1, -1, -1), vec2(0, 0));
  maker.AddVertex(vec3(1, -1, -1), vec2(1, 0));
  maker.AddVertex(vec3(1, 1, -1), vec2(1, 1));
  maker.AddVertex(vec3(1, 1, -1), vec2(1, 1));
  maker.AddVertex(vec3(-1, 1, -1), vec2(0, 1));
  maker.AddVertex(vec3(-1, -1, -1), vec2(0, 0));
  maker.SetTexture("C:/Luke/Programming/Engine/Assets/HALO/BloodGulge/Grass.jpg");
  RenderObject *mesh = maker.AssembleRenderObject();

  mesh->AssignShader(ASSETDIR "Pokemon/Shaders/pokemon.vert", ASSETDIR "Pokemon/Shaders/pokemon.frag", ASSETDIR "Pokemon/Shaders/pokemon.geom");

  Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);

    Camera::Update(60);

    // Render Everything

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
    modelMat.Scale(100);
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //model.Render(MVP);
    //mesh->Render(MVP);
    mesh->RenderPoints(MVP);

    window.Swap(); // Swap Window
    FrameRate::Update();
  }

  //model.Destroy();
}
