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
#include "EntityMaker.h"
#include "RealTimeEngine.h"
#include "Audio.h"

static bool MultiThread = true;

// Thread synchronizers
static volatile bool running = true;
static volatile bool streaming = true;

// ViewDistance
#ifdef _DEBUG
int viewDist = 8;
#else
int viewDist = 10;
#endif

BlockWorld *world = nullptr;

static int updateWorldThread(void *ptr)
{
  while (running)
  {
    world->Stream(vec3() - Camera::Position());
    //Sleep(1);
  }
  streaming = false;
  return 0;
}

static void _AddLine(ColouredRenderObjectMaker *maker, vec3 s, vec3 e, vec3 c = vec3())
{
  maker->AddVertex(s, c);
  maker->AddVertex(e, c);
  maker->AddVertex(e, c);
}

static void _DrawPlayerHoverBlock(vec3 pos, mat4 VP)
{
  static RenderObject *hoverBlock = nullptr;
  if (!hoverBlock)
  {
    float d = 0.005;
    float l = 0 - d;
    float h = 1 + d;
    ColouredRenderObjectMaker maker;
    _AddLine(&maker, { l, l, l }, { h, l, l });
    _AddLine(&maker, { l, l, h }, { h, l, h });
    _AddLine(&maker, { l, l, l }, { l, l, h });
    _AddLine(&maker, { h, l, l }, { h, l, h });
    _AddLine(&maker, { l, h, l }, { h, h, l });
    _AddLine(&maker, { l, h, h }, { h, h, h });
    _AddLine(&maker, { l, h, l }, { l, h, h });
    _AddLine(&maker, { h, h, l }, { h, h, h });
    _AddLine(&maker, { l, l, l }, { l, h, l });
    _AddLine(&maker, { h, l, l }, { h, h, l });
    _AddLine(&maker, { l, l, h }, { l, h, h });
    _AddLine(&maker, { h, l, h }, { h, h, h });
    hoverBlock = maker.AssembleRenderObject();
  }
  mat4 model;
  model.Translate(pos);
  mat4 MVP = VP * model;
  MVP.Transpose();
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glLineWidth(2);
  hoverBlock->Render(MVP);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Minecraft()
{
  //Threads::SetFastMode(); // Work in real time mode while loading

  //Audio::PlayMP3(ASSETDIR "minecraft/music/great sea.mp3", 1000, nullptr, true);

  Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering
#ifdef _DEBUG
  Window window("Game", true, 640, 480, false); // Create Debug Game Window
#else
  vec2i fullRes = Window::GetScreenRes();
  //Window window("Game", true, fullRes.x, fullRes.y, true); // Create Game Window
  Window window("Game", true, 1920, 1080, true); // Create Game Window
#endif

  //if (SDL_GL_SetSwapInterval(-1) == -1) SDL_GL_SetSwapInterval(1); // VSYNC
  //SDL_GL_SetSwapInterval(1);

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(70.0f * (float)DegsToRads, (float)window.width / window.height, 0.1f, 4096.0f);

  PolyModel skybox;
  skybox.LoadModel(ASSETDIR "/skybox/skybox.obj");
  //Textures::SetTextureFilterMode(false);

  // Create World
  //world = new BlockWorld(ASSETDIR "/minecraft/maps/island/", viewDist);
  world = new BlockWorld("C:/Users/Luke/AppData/Roaming/.technic/modpacks/vanilla/saves/roulette/region/", viewDist);

  vec3 playerPos = vec3(-56, 74, -1300);
  int64_t playerID = world->entities.Add(EntityMaker::CreatePlayer(world, playerPos));
  Camera::SetPosition(vec3()-playerPos);

  // Stream chunks (on another thread)
  if (MultiThread)
    SDL_CreateThread(updateWorldThread, "streamer", nullptr);

  Textures::SetTextureFilterMode(false); 
  //for (int i = 0; i < 3; i++) world->entities.Add(EntityMaker::CreateOrcWarrior(world, playerPos + vec3(rand() % 50 - 25, 50, rand() % 50 - 25)));
  //for (int i = 0; i < 3; i++) world->entities.Add(EntityMaker::CreateOrcBerzerker(world, playerPos + vec3(rand() % 50 - 25, 50, rand() % 50 - 25)));

  for (int64_t i = 0; i < 1; i++)
  {
    world->entities.Add(EntityMaker::CreateAgentNear(world, playerPos, ASSETDIR "Minecraft/Mobs/Zombie.png", EBH_Zombie));
    world->entities.Add(EntityMaker::CreateAgentNear(world, playerPos, ASSETDIR "Minecraft/Mobs/Skeleton.png", EBH_Zombie));

    //world->entities.Add(EntityMaker::CreateAgentNear(world, playerPos, ASSETDIR "Minecraft/Mobs/Villager2.png", EBH_Friendly));
    //world->entities.Add(EntityMaker::CreateAgentNear(world, playerPos, ASSETDIR "Minecraft/Mobs/Villager3.png", EBH_Friendly));
    //world->entities.Add(EntityMaker::CreateAgentNear(world, playerPos, ASSETDIR "Minecraft/Mobs/Villager4.png", EBH_Friendly));
    //world->entities.Add(EntityMaker::CreateAgentNear(world, playerPos, ASSETDIR "Minecraft/Mobs/Villager5.png", EBH_Friendly));
    //world->entities.Add(EntityMaker::CreateAgentNear(world, playerPos, ASSETDIR "Minecraft/Mobs/Villager6.png", EBH_Friendly));
    //world->entities.Add(EntityMaker::CreateAgentNear(world, playerPos, ASSETDIR "Minecraft/Mobs/Villager7.png", EBH_Friendly));
  }

  Textures::SetTextureFilterMode(true);

  // Create Cross Hair Model
  AlphaTexturedRenderObjectMaker cursorMaker;
  cursorMaker.SetTexture(ASSETDIR "Minecraft/Cursor.png");
  //float cursorSize = 0.025f;
  float cursorSize = 0.01f;
  cursorSize = -1.0f / cursorSize;
  cursorMaker.AddVertex(vec3(-1, -1, cursorSize), vec2(0, 0));
  cursorMaker.AddVertex(vec3(1, -1, cursorSize), vec2(1, 0));
  cursorMaker.AddVertex(vec3(-1, 1, cursorSize), vec2(0, 1));
  cursorMaker.AddVertex(vec3(1, -1, cursorSize), vec2(1, 0));
  cursorMaker.AddVertex(vec3(1, 1, cursorSize), vec2(1, 1));
  cursorMaker.AddVertex(vec3(-1, 1, cursorSize), vec2(0, 1));
  RenderObject *crossHairModel = cursorMaker.AssembleRenderObject();

  Controls::Update();
  RealTimeEngine RTE(60);
  RTE.Start();
  bool run = true;
  while (run) // Main Game Loop
  {

    // Toggle Fly Mode !
    static bool flyMode = false;
    static bool flyDown = false;
    if (Controls::KeyDown(SDL_SCANCODE_F) || Controls::GetControllerButton(6))
    {
      if (!flyDown)
        flyMode = !flyMode;
      flyDown = true;
    }
    else flyDown = false;

    // Update Camera & World
    int steps = RTE.Steps();
    for (int step = 0; step < steps; step++)
    {
      run &= Controls::Update();
      Camera::Update(10, false, flyMode);
      world->entities.Update();
      if (flyMode)
        world->entities.Get(playerID)->position = vec3() - Camera::Position();
      else
        Camera::SetPosition(vec3() - world->entities.Get(playerID)->position);
    }

    // Draw world
    window.Clear(0, 190, 255);

    // Skybox
    glDepthMask(GL_FALSE);
    mat4 skyMVP;
    skyMVP = projectionMat * Camera::RotationMatrix();
    skyMVP.Transpose();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    skybox.Render(skyMVP);
    glDepthMask(GL_TRUE);

    // World
    mat4 VP;
    mat4 viewMat = Camera::Matrix();
    VP = projectionMat * viewMat;

    // Draw players hovered cube
    Entity *player = world->entities.Get(playerID);
    vec3i hoverPos;
    if (world->RayTrace(player->position, player->direction, player->range, &hoverPos))
      _DrawPlayerHoverBlock(hoverPos, VP);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    world->Render(VP);

    // Draw Cross hair
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    mat4 uiMVP = projectionMat;
    uiMVP.Transpose();
    glDisable(GL_DEPTH_TEST);
    crossHairModel->Render(uiMVP);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    if (!MultiThread)
      world->Stream(vec3() - Camera::Position());

    window.Swap(false); // Swap Window
    glFinish();

    FrameRate::Update();
  }

  if (MultiThread)
  {
    running = false; while (streaming) Sleep(10); // Wait for streamer to end before exiting
  }
}