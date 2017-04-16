#include "Physics.h"
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
#include "Audio.h"

void Physics()
{
  Threads::SetFastMode(); 

#ifdef _DEBUG
  Window window("Physics", true, 640, 480, false);
#else
  Window window("Physics", true, 1920, 1080, true);
#endif

  Audio::PlayMP3(ASSETDIR "Physics/Music/Kakariko.mp3", 500, "music", true);

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, .4f, 4000.0f);

  PolyModel skybox;
  skybox.LoadModel(ASSETDIR "skybox/skybox.obj");

  window.Swap();
  PolyModel colorModel, lightModel;

  colorModel.LoadModel(ASSETDIR "Physics/Map/Color/test.obj");
  lightModel.LoadModel(ASSETDIR "Physics/Map/Light/test.obj");

  PolyMesh CollisionMesh;

  RenderObject *meshes;
  int64_t meshCount;
  colorModel.GetMeshData(&meshCount, &meshes);
  for (int m = 0; m < meshCount; m++)
  {
    int64_t vertexCount;
    const void *posData;
    meshes[m].AccessAttribute("position0", &vertexCount, nullptr, nullptr, &posData);
    vec3 *verts = (vec3*)posData;
    for (int i = 0; i < vertexCount / 3; i++)
      CollisionMesh.AddTriangle(Triangle(verts[i * 3 + 0], verts[i * 3 + 1], verts[i * 3 + 2]));
  }

  mat4 modelMat;
  Shaders::Report();

  Camera::SetPosition(vec3(0, -4, 0));

  float superSample = 2;

  // Frame buffering
  BufferObject colorBuffer(window.width * superSample, window.height * superSample);
  colorBuffer.AddPolyModel(&colorModel);
  colorBuffer.AddBuffer("fragColor");

  BufferObject lightBuffer(window.width  * superSample, window.height * superSample);
  lightBuffer.AddPolyModel(&lightModel);
  lightBuffer.AddBuffer("fragColor");

  RenderObject *finalRenderObject = FullScreenQuad(ASSETDIR "Physics/shaders/lighting.frag");
  BufferObject finalRenderBuffer(window.width, window.height);
  finalRenderObject->AssignTexture("texture0", colorBuffer.GetBuffer("fragColor"));
  finalRenderObject->AssignTexture("texture1", lightBuffer.GetBuffer("fragColor"));
  finalRenderBuffer.AddRenderObject(finalRenderObject);
  finalRenderBuffer.AddBuffer("fragColor");

  Threads::SetSlowMode();

  float verticalSpeed = 0;

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 190, 255);

    bool spaceDown = Controls::KeyDown(SDL_SCANCODE_SPACE) | Controls::GetControllerButton(10);
    bool ctrlDown = Controls::KeyDown(SDL_SCANCODE_LCTRL) | Controls::GetControllerButton(11);
    bool shiftDown = Controls::KeyDown(SDL_SCANCODE_LSHIFT) | Controls::GetControllerButton(12);

    float speed = 1;
    if (ctrlDown)
      speed /= 5.0f;

    float camspeed = 40;
    if (Controls::GetControllerButton(11))
      camspeed /= 5.0f;
    vec3 oldCamPos = vec3() - Camera::Position();
    Camera::Update(camspeed);
    vec3 newCamPos = vec3() - Camera::Position();
    vec3 diff = newCamPos - oldCamPos;
    if (!shiftDown)
    {
      diff.y = 0;
      vec3 finalPos = CollisionMesh.SlideSweepSphere(Sphere(oldCamPos, 3), diff + vec3(0, verticalSpeed * speed, 0));
      finalPos.y = Max(finalPos.y, -32);
      Camera::SetPosition(vec3() - finalPos);
    }
    newCamPos = vec3() - Camera::Position();

    bool onFloor = CollisionMesh.OnSweepSphere(Sphere(newCamPos, 3), vec3(0, -0.1, 0)) < 1.0;
    if(onFloor)
      verticalSpeed = Max(verticalSpeed, -0.1);
    else
      verticalSpeed -= 0.004 * speed;
    if (spaceDown && onFloor)
      verticalSpeed = 0.3;
    if (shiftDown)
      verticalSpeed = 0;


    // Skybox
    mat4 skyMVP;
    skyMVP = projectionMat * Camera::RotationMatrix();
    skyMVP.Transpose();
    glDepthMask(GL_FALSE);
    skybox.Render(skyMVP);
    glDepthMask(GL_TRUE);

    mat4 viewMat = Camera::Matrix();
    mat4 MVP;

    modelMat.LoadIdentity();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    colorBuffer.Render(MVP);

    modelMat.LoadIdentity();
    modelMat.RotateX(270 * DegsToRads);
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    lightBuffer.Render(MVP);

    mat4 identity;
    finalRenderBuffer.Render(identity);

    BufferObject::DisplayFullscreenTexture(finalRenderBuffer.GetBuffer("fragColor"));

    window.Swap(); // Swap Window
    FrameRate::Update();
  }
}
