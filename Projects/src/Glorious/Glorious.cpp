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
const bool MultiThreading = true;

GloriousModel* MainModel;

mat4 projectionMat;

static int GloriousStreamThread(void *ptr)
{
  mat4 MVP;
  mat4 viewMat;
  mat4 modelMat;

  while (running)
  {
    viewMat = Camera::Matrix();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    if (!Controls::KeyDown(SDL_SCANCODE_SPACE))
      MainModel->Stream(MVP);
  }
  streaming = false;
  return 0;
}

void Glorious()
{
#ifdef _DEBUG
  Window window("Game", true, 640, 480, false); // Create Debug Game Window
#else
  Window window("Game", true, 1920, 1080,  true); // Create Game Window
  //Window window("Game", true, 800, 600, false); // Create Game Window
#endif

  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 1.0 / 35.0, 515);

  Controls::SetMouseLock(true);

  //GloriousModel model("F:/temp/CarrickHillSmall.ncs");
  //GloriousModel model("F:/temp/CarrickHillMid.ncs");
  //GloriousModel model("F:/temp/CarrickHillFull.ncs");
  //GloriousModel model("F:/temp/Colledge.ncs");
  //GloriousModel model("F:/temp/Image.ncs");
  //GloriousModel model("F:/temp/CarrickHill.ncs");
  GloriousModel model("F:/temp/Colledge.ncs");

  //model.GenGMesh("F:/temp/Colledge.ncs", "F:/temp/Colledge.sgm");
  //exit(0);

  //GloriousModel model("F:/temp/Expressway.ncs");

  MainModel = &model;

  Textures::SetTextureFilterMode(false);

  Controls::Update();
  if(MultiThreading) SDL_CreateThread(GloriousStreamThread, "streamer", nullptr);

  Camera::SetPosition({ -128,-32,-128 });

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

    glFlush();
    window.Swap(); // Swap Window
    FrameRate::Update();
     
    if (!MultiThreading) model.Stream(MVP);
    //printf("tiles used: %d\n", model.atlasFreeTiles.UseCount);
  }
  if (MultiThreading)
  {
    running = false;
    while (streaming) Sleep(10); // Wait for streamer to end before exiting
  }

}
