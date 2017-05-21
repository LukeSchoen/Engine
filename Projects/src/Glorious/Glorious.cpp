#include "Glorious.h"
#include "GloriousModel.h"
#include "Window.h"
#include "Threads.h"
#include "FrameRate.h"
#include "Controls.h"
#include "Camera.h"
#include "RenderObject.h"
#include "Polymodel.h"
#include "Textures.h"
#include "ImageFile.h"
#include <stdlib.h>

// Thread synchronizers
static volatile bool running = true;
static volatile bool streaming = true;

GloriousModel* MainModel;

static int GloriousStreamThread(void *ptr)
{
  while (running)
  {
    MainModel->Stream();
    //Sleep(1);
  }
  streaming = false;
  return 0;
}

void Glorious()
{
  
#ifdef _DEBUG
  Window window("Game", true, 640, 480, false); // Create Debug Game Window
#else
  Window window("Game", true, 1920, 1080, true); // Create Game Window
  //Window window("Game", true, 800, 600, false); // Create Game Window
#endif

  Controls::SetMouseLock(true);

  //GloriousModel model("D:/temp/CarrickHillSmall.ncs");
  //GloriousModel model("D:/temp/CarrickHillMid.ncs");
  //GloriousModel model("D:/temp/CarrickHillFull.ncs");
  //GloriousModel model("D:/temp/Colledge.ncs");
  //GloriousModel model("D:/temp/Image.ncs");
  GloriousModel model("C:/Users/lschoen/Desktop/4b/c3.ncs");
  //GloriousModel model("D:/temp/CarrickHill.ncs");
  //GloriousModel model("D:/temp/Colledge.ncs");
  //GloriousModel model("D:/temp/Expressway.ncs");

  MainModel = &model;

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 1.0 / 32.0, 8192.0);
  mat4 modelMat;

  Textures::SetTextureFilterMode(false);

  SDL_CreateThread(GloriousStreamThread, "streamer", nullptr);

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Camera::Update(5);

    // Glorious Model
    mat4 MVP;
    mat4 modelMat;
    mat4 viewMat = Camera::Matrix();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();

    // Wire frame render
    static bool wireDown = false;
    static bool wire = false;
    if (Controls::KeyDown(SDL_SCANCODE_Q) || Controls::GetControllerButton(11))
    {
      if (!wireDown)
        wire = !wire;
      wireDown = true;
    }
    else wireDown = false;

    //model.Stream();

    if (wire)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glPolygonOffset(0, 1024);
      glEnable(GL_POLYGON_OFFSET_FILL);
      model.Render(MVP);
      glDisable(GL_POLYGON_OFFSET_FILL);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else
    {
      // Normal render
      model.Render(MVP);
    }


    if (Controls::KeyDown(SDL_SCANCODE_0))
    {
      for (int i = 0; i < model.atlasFreeTiles.UseCount; i++)
      {
        static char imgFile[1024];
        sprintf(imgFile, "C:/Users/Luke/Desktop/Atlas%d.png", i);
        ImageFile::WriteImagePNG(imgFile, model.atlas.image + (i * model.atlas.width * model.atlas.height), model.atlas.width, model.atlas.height);
      }

      system("start C:/Users/Luke/Desktop/Atlas0.png");
      break;
    }

    window.Swap(); // Swap Window
    FrameRate::Update();
    //printf("tiles used: %d\n", model.atlasFreeTiles.UseCount);
  }

  running = false; while (streaming) Sleep(10); // Wait for streamer to end before exiting

}
