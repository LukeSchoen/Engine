#include "StreamTest.h"
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

void StreamTest()
{
  //Threads::SetFastMode(); // Work in real time mode while loading
  Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering
  Window window("Game", true, 1024, 768, false); // Create Game Window

  if (SDL_GL_SetSwapInterval(-1) == -1) SDL_GL_SetSwapInterval(1); // Vsync

  PolyModel Model;
  //Model.LoadModel("C:/Users/lschoen/Desktop/Dinosaur.dae", true);
  //Model.LoadModel("C:/temp/Assimp/Tile.dae", true);
  //Model.LoadModel("T:/4Khan/Updated Street/Street.obj", true);
  Model.LoadModel("C:/temp/Assimp/rocks.dae", true);

  Model.BakeColors();

  Model.DownSample(500);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.01, 2000.0f);
  
  Controls::SetMouseLock(true);

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);

    // Update Camera & World
    Camera::Update(1.0);

    // Model
    mat4 MVP;
    mat4 modelMat;
    mat4 viewMat = Camera::Matrix();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    Textures::SetTextureFilterMode(true);

    // Model Shaders
    int64_t meshCount;
    RenderObject *meshes;
    Model.GetMeshData(&meshCount, &meshes); // Pass in Model Matrix
    modelMat.Transpose();
    for (int meshItr = 0; meshItr < meshCount; meshItr++)
    {
      vec3 camPos = Camera::Position();
      vec3 camDir = Camera::Direction();
      meshes[meshItr].AssignUniform("camPos", UT_3f, camPos.Data());
      meshes[meshItr].AssignUniform("camDir", UT_3f, camDir.Data());
      meshes[meshItr].AssignUniform("modelMat", UT_mat4fv, &modelMat);
    }
    Model.Render(MVP);

    window.Swap(); // Swap Window
    FrameRate::Update();
  }
}

