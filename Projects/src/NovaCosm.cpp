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
#include <thread>

// Thread synchronizers
static volatile bool running = true;
static volatile bool streaming = true;
static volatile bool runStream = true;

NovaCosmModel *pModel;

void NovaCosmStreamThread()
{
  while (running)
  {
    if (runStream)
      pModel->Stream();
    //Sleep(1);
  }
  streaming = false;
  return;
}

void NovaCosm()
{
  Threads::SetSlowMode(); // Work in real time mode while loading

  //Convertor::PCFtoPNG("F:/temp/ColledgeFull.pcf", "F:/temp/Colledge.png");
  //Convertor::ExportPCFtoNovaCosm("F:/temp/ColledgeFull.pcf", "F:/temp/Colledge.ncs", "F:/temp/export/", 8192);
  //exit(0);

  //Convertor::ExportPCFtoNovaCosm("F:/Luke/Programming/Visual Studio/pixelly/Software Tracer/CarrickHill.pcf", "F:/temp/CarrickHill.ncs", "F:/temp/export/", 2048);

#ifdef _DEBUG
  Window window("Game", true, 800, 600, false); // Create Game Window
#else
  Window window("Game", true, 1920, 1080, true); // Create Game Window
#endif

  //Window window("Game", true, 1920, 1080, true); // Create Game Window

  pModel = new NovaCosmModel("F:/temp/Colledge.ncs");
  //NovaCosmModel model("F:/Luke/Programming/Visual Studio/pixelly/Software Tracer/Expressway.pcf");
  //pModel->ExportPCF("F:/temp/ColledgeFull.pcf");
  //NovaCosmModel model("F:/temp/Colledge.ncs");
  //NovaCosmModel model("C:/temp/map.ncs");
  //exit(0);

  Camera::SetPosition(vec3(-120.317215, -58.7549934, -127.834129));
  Camera::SetRotation(vec2(0.984997272, 0.321998835));

  mat4 projectionMat;
  vec2 nearFar(0.02, 8000);
  projectionMat.Perspective(62.0f * (float)DegsToRads, (float)window.width / window.height, nearFar.x, nearFar.y);

  std::thread streamer(NovaCosmStreamThread);

  while (Controls::Update()) // Main Game Loop
  {
    static bool voxelHovered = false;
    static vec4 mouseWorldPos;
    static bool lastVoxelHovered = false;
    static vec4 lastMouseWorldPos;
    static bool debugging = false;
    if (Controls::KeyPressed(SDL_SCANCODE_Q)) debugging = !debugging;

    if(debugging) window.Clear(0, 0, 0); else window.Clear(0, 190, 255);

    // Mouse Controls
    static bool turning = false;
    turning = Controls::GetRightClick();
    Controls::SetMouseLock(turning);
    Camera::Update(2, !turning, true, turning);
    static vec2i lastMouse;
    static bool wasTurning = false;
    if (!turning && wasTurning)
      SDL_WarpMouseInWindow(window.window, lastMouse.x, lastMouse.y);
    if (!turning)
      lastMouse = Controls::GetMouse();
    wasTurning = turning;

    // Mouse orbiting
    static vec3 orbitPos;
    static vec3 orbitCamPos;
    static vec2 camRot;
    static vec2i orbitStart;
    static bool Orbiting = false;
    if (Controls::GetMiddleClick())
    {
      if (!Orbiting)
      {
        orbitPos = mouseWorldPos;
        camRot = Camera::Rotation();
        orbitStart = Controls::GetMouse();
        orbitCamPos = vec3() - Camera::Position();
      }
      vec3 dir = orbitCamPos - orbitPos;
      mat4 turn;
      float xRot = (Controls::GetMouse().x - orbitStart.x)*0.004;
      float yRot = (Controls::GetMouse().y - orbitStart.y)*0.004;
      turn.RotateY(xRot);
      //turn.RotateX(yRot);
      dir = vec3(vec4(dir.x, dir.y, dir.z, 1.0f) * turn);
      Camera::SetPosition(vec3() - orbitPos - dir);
      Camera::SetRotation(camRot - vec2(0, xRot));
      Orbiting = true;
    }
    else
      Orbiting = false;
    // Mouse scrolling
    static vec3 scrollOffset;
    if (voxelHovered && !turning)
    {
      vec3 campos = vec3() - Camera::Position();
      vec3 moveDir = vec3(mouseWorldPos) - campos;
      int scroll = Controls::GetMouseScroll();
      if (scroll)
      {
        if (scroll > 0) scroll = 1; 
        if (scroll < 0) scroll = -2;
        vec3 diff = vec3() - moveDir * 0.5 * scroll;
        scrollOffset = scrollOffset + diff;
        if (scrollOffset.Length() > diff.Length())
          scrollOffset = scrollOffset * (diff.Length() / scrollOffset.Length());
      }
    }
    Camera::SetPosition(Camera::Position() + scrollOffset * 0.1);
    scrollOffset = scrollOffset * 0.9;
    // Mouse Painting
    if (Controls::GetLeftClick() && voxelHovered && lastVoxelHovered)
    {
      float stepSize = 0.03;
      vec3 sPos = vec3(lastMouseWorldPos) + Camera::Direction() * stepSize;
      vec3 ePos = vec3(mouseWorldPos) + Camera::Direction() * stepSize;
      vec3 dir = (ePos - sPos);
      int dist = dir.Length() / stepSize;
      dir = dir.Normalized();
      for (int s = 0; s < dist; s++)
      {
        pModel->AddPoint(sPos, 0xFFFFFF);
        pModel->AddPoint(sPos + vec3(-stepSize, 0, 0), 0xFFFFFF);
        pModel->AddPoint(sPos + vec3(stepSize, 0, 0), 0xFFFFFF);
        pModel->AddPoint(sPos + vec3(0, -stepSize, 0), 0xFFFFFF);
        pModel->AddPoint(sPos + vec3(0, stepSize, 0), 0xFFFFFF);
        pModel->AddPoint(sPos + vec3(0, 0, -stepSize), 0xFFFFFF);
        pModel->AddPoint(sPos + vec3(0, 0, stepSize), 0xFFFFFF);
        sPos = sPos + dir * stepSize;
      }
    }
    lastVoxelHovered = voxelHovered;
    lastMouseWorldPos = mouseWorldPos;
    
    // Mouse Segmentation Selection
    // if left mouse click
    NovaCosmBlock *block;
    if(!Controls::GetLeftClick())
    {
      static uint32_t *colData = nullptr;
      static int colDataSize = 0;
      int layer;
      block = pModel->GetCamBlock(mouseWorldPos, &layer);
      if (block)
      {
        if(block->voxelCount * 3 > colDataSize)
        {
          colDataSize = block->voxelCount * 3;
          colData = (uint32_t*)realloc(colData, colDataSize);
        }
        memcpy(colData, block->voxelColData, block->voxelCount * 3);

        // find which segment the mouse lies in
        auto seg =(uint8_t*)block->voxelSegmentData;
        uint8_t segR = seg[0]; uint8_t segG = seg[1]; uint8_t segB = seg[2];

        float bestDist = 512;
        for (int64_t vItr = 0; vItr < block->voxelCount; vItr++)
        {
          auto xyz = &((uint8_t*)block->voxelPosData)[vItr * 3];
          auto segData = &((uint8_t*)block->voxelSegmentData)[vItr * 3];
          auto rgb = &((uint8_t*)block->voxelColData)[vItr * 3];

          float layerSize = (1.0 / (1LL << layer));
          vec3 blockPos = (vec3(xyz[0], xyz[1], xyz[2]) + block->position) * layerSize;
          float dist = (blockPos - mouseWorldPos).Length();
          if (dist < bestDist)
          {
            bestDist = dist;
            segR = segData[0];
            segG = segData[1];
            segB = segData[2];
          }
        }
        // Generate cp;pr
        uint8_t cr = 0x1E;
        uint8_t cg = 0x90;
        uint8_t cb = 0xFF;
        float occ = 1-abs(sin(clock() /500.f));
        cr += occ * (255 - cr);
        cg += occ * (255 - cg);
        cb += occ * (255 - cb);
        for (int64_t vItr = 0; vItr < block->voxelCount; vItr++)
        {
          auto xyz = &((uint8_t*)block->voxelPosData)[vItr * 3];
          auto segData = &((uint8_t*)block->voxelSegmentData)[vItr * 3];
          auto rgb = &((uint8_t*)block->voxelColData)[vItr * 3];
          if (segData[0] == segR && segData[1] == segG && segData[2] == segB)
          {
            rgb[0] = (rgb[0] + cr) / 2;
            rgb[1] = (rgb[1] + cg) / 2;
            rgb[2] = (rgb[2] + cb) / 2;
          }
        }
        block->Updated = false;
        block->voxels.AssignAttribute("color", AT_UNSIGNED_BYTE_NORM, block->voxelColData, 3, block->voxelCount);
        memcpy(block->voxelColData, colData, block->voxelCount * 3);
      }
    }


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

    //static int last = 0;
    //printf("%d ms\n\n", clock() - last);
    //last = clock();

    runStream = !Controls::KeyDown(SDL_SCANCODE_F);

    pModel->Render(MVP, debugging);

    if(block) block->Updated = true;

    glFinish();
    window.Swap(false); // Swap Window

    // Retrieve mouse location
    GLfloat depth_comp;
    vec2i mouse = Controls::GetMouse();
    mouse.y = window.height - 1 - mouse.y;
    glReadPixels(mouse.x, mouse.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth_comp);
    voxelHovered = depth_comp < 1;
    if (voxelHovered)
    {
      GLfloat clip_z = (depth_comp - 0.5f) * 2.0f;
      mat4 iMVP = MVP;
      iMVP.Transpose();
      iMVP.inverse();
      vec4 sp = vec4(mouse.x / float(window.width) * 2.0 - 1, mouse.y / float(window.height)  * 2.0 - 1, clip_z, 1);
      mouseWorldPos = iMVP * sp;
      mouseWorldPos = mouseWorldPos / mouseWorldPos.w;
    }

    if (Controls::KeyDown(SDL_SCANCODE_F))
      pModel->Stream();

    FrameRate::Update();
  }

  running = false;
  streamer.join();
}
