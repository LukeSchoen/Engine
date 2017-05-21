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

  //uint32_t w, h;
  //uint32_t *img = ImageFile::ReadImage("C:/Users/Luke/Desktop/Image.jpg", &w, &h);
  //
  //StreamFileWriter stream("D:/temp/Image.pcf");
  //for (int32_t y = 0; y < h; y++)
  //  for (int32_t x = 0; x < w; x++)
  //    for (int32_t iy = 0; iy < 10; iy++)
  //      for (int32_t ix = 0; ix < 10; ix++)
  //  {
  //    int32_t z = 0;
  //    int32_t ox = x + ix * w;
  //    int32_t oy = y + iy * h;
  //    stream.WriteBytes(&ox, sizeof(x));
  //    stream.WriteBytes(&z, sizeof(z));
  //    stream.WriteBytes(&oy, sizeof(y));
  //    uint32_t c = img[x + y * w];
  //    uint8_t r, g, b;
  //    b = c & 255;
  //    g = (c >> 8) & 255;
  //    r = (c >> 16) & 255;
  //    c = r | (g << 8) | (b << 16);
  //    stream.WriteBytes(&c, sizeof(c));
  //  }
  //stream.StopStream();
  //
  //Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering
  //Convertor::ExportPCFtoNovaCosm("D:/temp/Image.pcf", "D:/temp/Image.ncs", "D:/temp/export/");
  //exit(0);
  //Convertor::ExportPCFtoNovaCosm("D:/Luke/Programming/Visual Studio/pixelly/Software Tracer/CarrickHill.pcf", "D:/temp/CarrickHillFull.ncs", "D:/temp/export/", 255);
  //Convertor::ExportPCFtoNovaCosm("D:/Luke/Programming/Visual Studio/pixelly/Software Tracer/sorrento.pcf", "D:/temp/sorrento.ncs", "D:/temp/export/", 2048);
  //Convertor::ExportPCFtoNovaCosm("D:/Luke/Programming/Visual Studio/pixelly/Software Tracer/Expressway.pcf", "D:/temp/Expressway.ncs", "D:/temp/export/", 8192);
  //Convertor::ExportPCFtoNovaCosm("C:/Users/lschoen/Desktop/Quary/Quarry.pcf", "C:/Users/lschoen/Desktop/Quary/Quarry.ncs", "D:/temp/export/", 8192);
  //exit(0);

  //NovaCosmModel model("D:/temp/CarrickHill.ncs");
  Window window("Game", true, 1920, 1080, false); // Create Game Window

  //NovaCosmModel model("C:/temp/map.ncs");

  NovaCosmModel modelFull("C:/Users/lschoen/Desktop/Quary/Quarry.ncs");
  NovaCosmModel model("C:/Users/lschoen/Desktop/Quary/Quarry.ncs");

  //modelFull.ExportPCF("C:/Users/lschoen/Desktop/Quary/Quarry.pcf");
  //exit(0);

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.02, 8000.0f);

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);
    // Update Camera & World
    Camera::Update(10);

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


    static bool fullMode = false;
    static bool fullHeld = false;

    if (Controls::KeyDown(SDL_SCANCODE_L))
    {
      if (!fullHeld)
        fullMode = !fullMode;
      fullHeld = true;
    }
    else
      fullHeld = false;

    if (fullMode)
      modelFull.Render(MVP);
    else
      model.Render(MVP);

    window.Swap(); // Swap Window

    if (Controls::KeyDown(SDL_SCANCODE_L))
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