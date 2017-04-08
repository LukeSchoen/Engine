#include <stdlib.h>
#include <crtdbg.h>  

#include "NovaCosm.h"
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
#include "NovaCosmModel.h"
#include "Convertor.h"

void NovaCosm()
{
  //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  //Threads::SetFastMode(); // Work in real time mode while loading
  Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering
  //Convertor::ExportPCFtoNovaCosm("D:/Luke/Programming/Visual Studio/pixelly/Software Tracer/CarrickHill.pcf", "D:/temp/CarrickHillFull.ncs", "D:/temp/export/", 255);
  //Convertor::ExportPCFtoNovaCosm("D:/Luke/Programming/Visual Studio/pixelly/Software Tracer/sorrento.pcf", "D:/temp/sorrento.ncs", "D:/temp/export/", 2048);
  //Convertor::ExportPCFtoNovaCosm("D:/Luke/Programming/Visual Studio/pixelly/Software Tracer/Expressway.pcf", "D:/temp/Expressway.ncs", "D:/temp/export/", 8192);
  //Convertor::ExportPCFtoNovaCosm("D:/Luke/Programming/Visual Studio/pixelly/Software Tracer/CarrickHill.pcf", "D:/temp/CarrickHill.ncs", "D:/temp/export/", 4096);
  //exit(0);

  Window window("Game", true, 1920, 1080, true); // Create Game Window

  //NovaCosmModel model("D:/temp/CarrickHill.ncs");
  //NovaCosmModel model("C:/temp/map.ncs");
  NovaCosmModel model("D:/temp/Colledge.ncs");

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.02, 8000.0f);

  //PolyModel skybox;
  //skybox.LoadModel("../Assets/skybox/skybox.obj");

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);
    // Update Camera & World
    Camera::Update(1);

    // Skybox
    Textures::SetTextureFilterMode(false);
    mat4 skyMVP;
    skyMVP = projectionMat * Camera::RotationMatrix();
    skyMVP.Transpose();
    glDepthMask(GL_FALSE);
    //skybox.Render(skyMVP);
    glDepthMask(GL_TRUE);

    // Cosmic Model
    mat4 MVP;
    mat4 modelMat;
    mat4 viewMat = Camera::Matrix();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();

    model.Render(MVP);

    window.Swap(); // Swap Window

    if(Controls::KeyDown(SDL_SCANCODE_F))
      model.Stream();

    if (Controls::GetControllerButton(11)) model.AddPoint(vec3() - Camera::Position(), 0x0000FF);
    if (Controls::GetControllerButton(10)) model.AddPoint(vec3() - Camera::Position(), 0x00FF00);
    if (Controls::GetControllerButton(12)) model.AddPoint(vec3() - Camera::Position(), 0xFF0000);
    if (Controls::GetControllerButton(13)) model.AddPoint(vec3() - Camera::Position(), 0xFFFFFF);

    if (Controls::KeyDown(SDL_SCANCODE_1)) model.AddPoint(vec3() - Camera::Position(), 0x0000FF);
    if (Controls::KeyDown(SDL_SCANCODE_2)) model.AddPoint(vec3() - Camera::Position(), 0x00FF00);
    if (Controls::KeyDown(SDL_SCANCODE_3)) model.AddPoint(vec3() - Camera::Position(), 0xFF0000);
    if (Controls::KeyDown(SDL_SCANCODE_4)) model.AddPoint(vec3() - Camera::Position(), 0xFFFFFF);
    if (Controls::KeyDown(SDL_SCANCODE_5)) model.AddPoint(vec3() - Camera::Position(), 0x000000);

    FrameRate::Update();
  }

}