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
  Threads::SetFastMode(); // Work in real time mode while loading

  //Convertor::ExportPCFtoNovaCosm("F:/Luke/Programming/Visual Studio/pixelly/Software Tracer/Colledge.pcf", "F:/temp/Colledge.ncs", "F:/temp/export/", 4096 * 2);
  //exit(0);

  Window window("Game", true, 1600, 1000, false); // Create Game Window

  //NovaCosmModel model("D:/temp/CarrickHill.ncs");
  //NovaCosmModel model("C:/temp/map.ncs");
  NovaCosmModel model("F:/temp/Colledge.ncs");
  //NovaCosmModel model("F:/Luke/Programming/Visual Studio/pixelly/Software Tracer/Expressway.pcf");

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.02, 8000.0f);


  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);
    // Update Camera & World
    Camera::Update(6);

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

    static int last = 0;
    printf("%d ms\n\n", clock() - last);
    last = clock();

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
