#include "Cosmic.h"
#include "Window.h"
#include "Threads.h"
#include "FrameRate.h"
#include "Controls.h"
#include "Camera.h"
#include <stdlib.h>
#include "RenderObject.h"
#include "Maths.h"
#include "PolyModel.h"
#include "Textures.h"
#include "CosmicModel.h"
#include "Assets.h"

void Cosmic()
{
  //Threads::SetFastMode(); // Work in real time mode while loading
  Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering
  Window window("Game", true, 800, 600, false); // Create Game Window

  CosmicModel model("C:/temp/Colledge.cvs");

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 1.0f, 80000.0f);

  PolyModel skybox;
  skybox.LoadModel(ASSETDIR "skybox/skybox.obj");


  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);
    // Update Camera & World
    Camera::Update(500);

    // Skybox
    Textures::SetTextureFilterMode(false);
    mat4 skyMVP;
    skyMVP = projectionMat * Camera::RotationMatrix();
    skyMVP.Transpose();
    glDepthMask(GL_FALSE);
    skybox.Render(skyMVP);
    glDepthMask(GL_TRUE);


    // Cosmic Model
    mat4 MVP;
    mat4 modelMat;
    mat4 viewMat = Camera::Matrix();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();

    model.Render(MVP);

    window.Swap(); // Swap Window

    FrameRate::Update();
    model.Stream();
  }
}