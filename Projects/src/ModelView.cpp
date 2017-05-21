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

void ModelView()
{
#ifdef _DEBUG
  Window window("Game", true, 640, 480, false);
#else
  Window window("Game", true, 1920, 1080, true);
#endif
  Controls::SetMouseLock(true);
  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 1.0f, 500000.0f);

  PolyModel skybox;
  skybox.LoadModel(ASSETDIR "skybox/skybox.obj");

  window.Swap();

  PolyModel model;
  model.LoadModel("C:/Users/lschoen/Desktop/Models/cat.obj");

  while (Controls::Update())
  {
    window.Clear(0, 190, 255);
    Camera::Update(10);
    
    mat4 viewMat = Camera::Matrix();
    mat4 MVP;
    mat4 modelMat;
    modelMat.Scale(1);
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    model.Render(MVP);

    window.Swap();
    FrameRate::Update();
  }
}
