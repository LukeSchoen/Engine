#include "Minecraft.h"
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
#include "BlockWorld.h"
#include "Entities.h"
#include "Text.h"
#include "Assets.h"
#include "ImageFile.h"

static bool MultiThread = true;

// Thread synchronizers
static volatile bool running = true;
static volatile bool streaming = true;

// ViewDistance
#ifdef _DEBUG
int viewDist = 4;
#else
int viewDist = 20;
#endif

BlockWorld *world = nullptr;

static int updateExchangeData(void *ptr)
{
  while (running)
  {
    world->Stream(vec3() - Camera::Position());
    //Sleep(1);
  }
  streaming = false;
  return 0;
}

void Minecraft()
{
  //Threads::SetFastMode(); // Work in real time mode while loading

  Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering
#ifdef _DEBUG
  Window window("Game", true, 640, 480, false); // Create Debug Game Window
#else
  Window window("Game", true, 1920, 1080, true); // Create Game Window
#endif

  //if (SDL_GL_SetSwapInterval(-1) == -1) SDL_GL_SetSwapInterval(1); // VSYNC
  //SDL_GL_SetSwapInterval(1);


  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.1f, 4096.0f);

  PolyModel skybox;
  skybox.LoadModel(ASSETDIR "/skybox/skybox.obj");
  //Textures::SetTextureFilterMode(false);

  // Create World
  //world = new BlockWorld("C:/Users/Luke/Desktop/region/", viewDist);
  world = new BlockWorld("C:/Users/Luke/Desktop/kanto/", viewDist);

  // Stream chunks (on another thread)
  if (MultiThread)
    SDL_CreateThread(updateExchangeData, "streamer", nullptr);

  Entity player(world);

  // Create Cross Hair Model
  AlphaTexturedRenderObjectMaker cursorMaker;
  cursorMaker.SetTexture(ASSETDIR "Minecraft/Cursor.png");
  float cursorSize = 0.025f;
  cursorSize = -1.0f / cursorSize;
  cursorMaker.AddVertex(vec3(-1, -1, cursorSize), vec2(0, 0));
  cursorMaker.AddVertex(vec3(1, -1, cursorSize), vec2(1, 0));
  cursorMaker.AddVertex(vec3(-1, 1, cursorSize), vec2(0, 1));
  cursorMaker.AddVertex(vec3(1, -1, cursorSize), vec2(1, 0));
  cursorMaker.AddVertex(vec3(1, 1, cursorSize), vec2(1, 1));
  cursorMaker.AddVertex(vec3(-1, 1, cursorSize), vec2(0, 1));
  RenderObject *crossHairModel = cursorMaker.AssembleRenderObject();

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);
    // Update Camera & World
    Camera::Update(6);

    // Smash blocks !
    static bool smashDown = false;
    if (Controls::GetLeftClick() || Controls::GetControllerButton(10))
    {
      if (!smashDown)
      {
        player.position = vec3() - Camera::Position();
        player.direction = vec3() - Camera::Direction();
        player.Smash();
      }
      smashDown = true;
    }
    else smashDown = false;

    // Place blocks !
    static bool placeDown = false;
    if (Controls::GetRightClick() || Controls::GetControllerButton(12))
    {
      if (!placeDown)
      {
        player.position = vec3() - Camera::Position();
        player.direction = vec3() - Camera::Direction();
        player.Place(12);
      }
      placeDown = true;
    }
    else placeDown = false;



    //Textures::SetTextureFilterMode(false);
    // Skybox
    glDepthMask(GL_FALSE);
    mat4 skyMVP;
    skyMVP = projectionMat * Camera::RotationMatrix();
    skyMVP.Transpose();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    skybox.Render(skyMVP);
    glDepthMask(GL_TRUE);

    // World
    mat4 MVP;
    mat4 modelMat;
    mat4 viewMat = Camera::Matrix();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    world->Render(MVP);


    // Draw Cross hair
    glDepthMask(GL_FALSE);
    mat4 uiMVP = projectionMat;
    uiMVP.Transpose();
    glDisable(GL_DEPTH_TEST);
    crossHairModel->Render(uiMVP);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);


    if (Controls::KeyDown(SDL_SCANCODE_0))
    {
      ImageFile::WriteImagePNG("C:/Users/Luke/Desktop/Atlas.png", world->lightingAtlas.image, world->lightingAtlas.width, world->lightingAtlas.height);
      system("start C:/Users/Luke/Desktop/Atlas.png");
      break;
    }

    if (!MultiThread)
    {
      world->Stream(vec3() - Camera::Position());
    }

    window.Swap(); // Swap Window

    FrameRate::Update();
  }

  if (MultiThread)
  {
    running = false; while (streaming) Sleep(10); // Wait for streamer to end before exiting
  }
}