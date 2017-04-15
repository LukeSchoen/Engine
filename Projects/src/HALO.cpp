#include "HALO.h"
#include "Window.h"
#include "Threads.h"
#include "FrameRate.h"
#include "Controls.h"
#include "Camera.h"
#include <stdlib.h>
#include "Maths.h"
#include "PolyModel.h"
#include "Textures.h"
#include "Shaders.h"
#include <vector>
#include "Physics.h"
#include "Warthog.h"
#include "Audio.h"
#include "Assets.h"

extern bool camFollowCar;

void HALO()
{
  //Threads::SetFastMode(); // Work in real time mode while loading
  Threads::SetSlowMode(); // Don't starve OpenGLs driver while rendering
  Window window("Game", true, 1024, 768, false); // Create Game Window

  if (SDL_GL_SetSwapInterval(-1) == -1)
    SDL_GL_SetSwapInterval(1);

  Audio::PlayMP3(ASSETDIR "HALO/Blood Gulch Ambient Noise.mp3", 500, "BloodGulch", true);
  Audio::PlayMP3(ASSETDIR "HALO/Engine.mp3", 0, "Engine", true);

  PolyModel map;
  map.LoadModel(ASSETDIR "HALO/BloodGulge.dae");

  PolyMesh mapCollider;
  int64_t meshCount = 0;
  RenderObject *meshs;
  map.GetMeshData(&meshCount, &meshs);
  for (int m = 0; m < meshCount; m++)
  {
    int64_t vertexCount;
    const void *posData;
    meshs[m].AccessAttribute("position0", &vertexCount, nullptr, nullptr, &posData);
    vec3 *verts = (vec3*)posData;
    for (int i = 0; i < vertexCount / 3; i++)
      mapCollider.AddTriangle(Triangle(verts[i * 3 + 0], verts[i * 3 + 1], verts[i * 3 + 2]));
  }

  PolyModel pistol;
  pistol.LoadModel(ASSETDIR "HALO/Pistol.dae");


  PolyModel dirtShrapnelModel;
  dirtShrapnelModel.LoadModel("../Assets/HALO/Dirt.dae",true);


  Warthog warthog(vec3(10, 0, -10));

  map.GetMeshData(&meshCount, &meshs);
  for (uint32_t i = 0; i < meshCount; i++)
    meshs[i].AssignShader(SHADERDIR "FoggedTexturedRenderObject.vert", SHADERDIR "FoggedTexturedRenderObject.frag");

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.1, 10000.0f);

  PolyModel skybox;
  skybox.LoadModel("../Assets/skybox/skybox.obj");


  struct dirtShrapnel
  {
    vec3 position, velocity, rotation, angVel;
    float life;
  };
  std::vector<dirtShrapnel> Shrapnel;

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);

    // Update Camera & World
    Camera::Update();

    warthog.update(mapCollider);

    // Skybox
    Textures::SetTextureFilterMode(false);
    mat4 skyMVP;
    skyMVP = projectionMat * Camera::RotationMatrix();
    skyMVP.Transpose();
    glDepthMask(GL_FALSE);
    skybox.Render(skyMVP);
    glDepthMask(GL_TRUE);

    // Map
    mat4 MVP;
    mat4 modelMat;
    mat4 viewMat = Camera::Matrix();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    Textures::SetTextureFilterMode(true);
    map.Render(MVP);


    // Shrapnel
    for (int i = 0; i < Shrapnel.size(); i++)
    {
      Shrapnel[i].position = Shrapnel[i].position + Shrapnel[i].velocity * 0.2;
      Shrapnel[i].velocity = Shrapnel[i].velocity + vec3(0, -0.2, 0);
      Shrapnel[i].rotation = Shrapnel[i].rotation + Shrapnel[i].angVel * 0.2;
      Shrapnel[i].life++;
      if (Shrapnel[i].life > 100)
      {
        Shrapnel.erase(Shrapnel.begin() + i);
        i--;
        continue;
      }

      modelMat.LoadIdentity();
      modelMat.Translate(Shrapnel[i].position);
      modelMat.Rotate(Shrapnel[i].rotation);
      MVP = projectionMat * viewMat * modelMat;
      MVP.Transpose();
      dirtShrapnelModel.Render(MVP);
    }

    if (!camFollowCar)
    {
      // Gun
      static vec2 interCamRot;
      modelMat.LoadIdentity();
      modelMat.Translate(vec3() - Camera::Position());

      static float firing = 0;
      if (Controls::GetLeftClick())
      {
        if (firing < 15 * DegsToRads)
        {
          firing = 120 * DegsToRads;
          static char buff[256];
          sprintf(buff, "../Assets/HALO/Pistol/Shot %d.mp3", (rand() % 5) + 1);
          Audio::PlayMP3(buff, 500);
          float SinY = sin(interCamRot.y);
          float CosY = cos(interCamRot.y);
          float SinX = sin(interCamRot.x);
          float CosX = cos(interCamRot.x);
          vec3 gunDirection = vec3(SinY * CosX, SinX, CosY * CosX).Normalized();
          vec3 BulletPath = (vec3() - vec3(gunDirection)) * 1000000;


          float t = mapCollider.OnRay(Ray(vec3() - Camera::Position(), BulletPath));
          //float t = mapCollider.OnSweepSphere(Sphere(vec3() - Camera::Position(), 1.0f), BulletPath);
          if (t < 1.0f)
            for (int i = 0; i < 32; i++)
            {
              //Shrapnel.push_back({ vec3() - Camera::Position() + BulletPath * t, vec3(rand() / (RAND_MAX + 0.0f), 1.0f + rand() / (RAND_MAX + 0.0f),rand() / (RAND_MAX + 0.0f))});
              vec3 pos = vec3() - Camera::Position() + BulletPath * t;
              vec3 vel = vec3(rand() / (RAND_MAX + 0.0f) - 0.5, 1.0f + rand() / (RAND_MAX + 0.0f), rand() / (RAND_MAX + 0.0f) - 0.5);
              vec3 rot = vec3(rand() / 1000.0f, rand() / 1000.0f, rand() / 1000.0f);
              vec3 angvel = vec3(rand() / (RAND_MAX + 0.0f) - 0.5, rand() / (RAND_MAX + 0.0f) - 0.5, rand() / (RAND_MAX + 0.0f) - 0.5);
              Shrapnel.push_back({ pos, vel, rot, angvel}); 
            }
        }
      }
      firing = firing * 0.85;

      static float t = 0;
      t += 0.04;

      interCamRot = ((interCamRot * 9) + (Camera::Rotation())) * 0.1;

      vec2 gunRot = interCamRot;
      gunRot.x = gunRot.x + sin(t)*0.005;
      gunRot.y = gunRot.y + sin(t - 1)*0.003;
      mat4 camRot;
      camRot.RotateZ(gunRot.x - 0 * DegsToRads);
      camRot.RotateY(gunRot.y - 270 * DegsToRads);

      modelMat.Translate(vec4(0.268276423 ,-0.186435908, 0.0413618796) * camRot);
      gunRot.x = gunRot.x - firing;
      modelMat.RotateY(0 - gunRot.y - 90 * DegsToRads);
      modelMat.RotateZ(0 - gunRot.x - 0 * DegsToRads);
      MVP = projectionMat * viewMat * modelMat;
      MVP.Transpose();
      //pistol.Render(MVP);
    }

    // Car
    warthog.draw(projectionMat * viewMat);

    window.Swap(); // Swap Window
    FrameRate::Update();
  }
}

