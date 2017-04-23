#include "PhysicsTest.h"
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
#include "Physics.h"

RenderObject *MakeCube()
{
  TexturedRenderObjectMaker cubeMaker;

  // Bottom Face
  cubeMaker.SetTexture(ASSETDIR "physics/map/crate.png");
  cubeMaker.AddVertex(vec3(-0.5, -0.5, -0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(0.5, -0.5, -0.5), vec2(1, 0));
  cubeMaker.AddVertex(vec3(0.5, -0.5, 0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(-0.5, -0.5, -0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(0.5, -0.5, 0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(-0.5, -0.5, 0.5), vec2(0, 1));
  // Top Face
  cubeMaker.AddVertex(vec3(-0.5, 0.5, -0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(0.5, 0.5, -0.5), vec2(1, 0));
  cubeMaker.AddVertex(vec3(0.5, 0.5, 0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(-0.5, 0.5, -0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(0.5, 0.5, 0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(-0.5, 0.5, 0.5), vec2(0, 1));

  // Back
  cubeMaker.AddVertex(vec3(-0.5, -0.5, -0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(0.5, -0.5, -0.5), vec2(1, 0));
  cubeMaker.AddVertex(vec3(0.5, 0.5, -0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(-0.5, -0.5, -0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(0.5, 0.5, -0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(-0.5, 0.5, -0.5), vec2(0, 1));

  // Front
  cubeMaker.AddVertex(vec3(-0.5, -0.5, 0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(0.5, -0.5, 0.5), vec2(1, 0));
  cubeMaker.AddVertex(vec3(0.5, 0.5, 0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(-0.5, -0.5, 0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(0.5, 0.5, 0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(-0.5, 0.5, 0.5), vec2(0, 1));

  // Left
  cubeMaker.AddVertex(vec3(-0.5, -0.5, -0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(-0.5, -0.5, 0.5), vec2(1, 0));
  cubeMaker.AddVertex(vec3(-0.5, 0.5, 0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(-0.5, -0.5, -0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(-0.5, 0.5, 0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(-0.5, 0.5, -0.5), vec2(0, 1));

  // Right
  cubeMaker.AddVertex(vec3(0.5, -0.5, -0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(0.5, -0.5, 0.5), vec2(1, 0));
  cubeMaker.AddVertex(vec3(0.5, 0.5, 0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(0.5, -0.5, -0.5), vec2(0, 0));
  cubeMaker.AddVertex(vec3(0.5, 0.5, 0.5), vec2(1, 1));
  cubeMaker.AddVertex(vec3(0.5, 0.5, -0.5), vec2(0, 1));

  return cubeMaker.AssembleRenderObject();
}

void PhysicsTest()
{
  Threads::SetFastMode();

#ifdef _DEBUG
  Window window("Physics", true, 640, 480, false);
#else
  //Window window("Physics", true, 800, 600, false);
  Window window("Physics", true, 1920, 1080, true);
#endif
  float superSample = 2;

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, .4f, 4000.0f);
  window.Swap();

  RenderObject *cubeMesh = MakeCube();

  PolyModel colorModel, lightModel;

  colorModel.LoadModel(ASSETDIR "Physics/Map/Color/test.obj");
  lightModel.LoadModel(ASSETDIR "Physics/Map/Light/test.obj");

  // Frame buffering
  BufferObject mapColorBuffer(window.width * superSample, window.height * superSample);
  mapColorBuffer.AddPolyModel(&colorModel);
  mapColorBuffer.AddBuffer("fragColor");

  BufferObject mapLightBuffer(window.width  * superSample, window.height * superSample);
  mapLightBuffer.AddPolyModel(&lightModel);
  mapLightBuffer.AddBuffer("fragColor");

  BufferObject actorColorBuffer(window.width  * superSample, window.height * superSample);
  actorColorBuffer.AddBuffer("fragColor");

  RenderObject *finalRenderObject = FullScreenQuad(ASSETDIR "Physics/shaders/lighting.frag");
  BufferObject finalRenderBuffer(window.width, window.height);
  finalRenderObject->AssignTexture("texture0", mapColorBuffer.GetBuffer("fragColor"));
  finalRenderObject->AssignTexture("texture1", mapLightBuffer.GetBuffer("fragColor"));
  finalRenderObject->AssignTexture("texture2", actorColorBuffer.GetBuffer("fragColor"));

  finalRenderObject->AssignTexture("depth0", mapColorBuffer.GetDepth());
  finalRenderObject->AssignTexture("depth1", actorColorBuffer.GetDepth());

  finalRenderBuffer.AddRenderObject(finalRenderObject);
  finalRenderBuffer.AddBuffer("fragColor");

  PhysicsWorld world;
  int64_t meshCount;
  RenderObject *meshes;
  colorModel.GetMeshData(&meshCount, &meshes);
  for (int64_t m = 0; m < meshCount; m++)
  {
    GLAttributeType aType;
    uint32_t aCount;
    int64_t vertCount;
    const void *pData;
    meshes[m].AccessAttribute("position0", &vertCount, &aCount, &aType, &pData);
    if(vertCount)
      world.AddMesh(vec3(0, 0, 0), (vec3*)pData, vertCount, 0);
  }
  std::vector<PhysicsObject> cubes;

  Camera::SetPosition(vec3(0, -30, 0));

  PhysicsObject CamSphere = world.AddSphere(vec3() - Camera::Position(), 5, 1);
  CamSphere.SetCanSleep(false);

  while (Controls::Update())
  {
    window.Clear(0, 190, 255);
    mat4 viewMat = Camera::Matrix();

    actorColorBuffer.ClearRenderInstances();
    for(const auto &cube : cubes)
    {
      mat4 scaler;
      scaler.Scale(10);
      mat4 model = cube.GetModelMat() * scaler;
      actorColorBuffer.AddRenderObject(cubeMesh, model);
    }

    mat4 MVP = projectionMat * viewMat;
    mapColorBuffer.Render(MVP);
    actorColorBuffer.Render(MVP);
    mat4 modelMat;
    modelMat.RotateX(270 * DegsToRads);
    MVP = projectionMat * viewMat * modelMat;
    mapLightBuffer.Render(MVP);
    mat4 identity;
    finalRenderBuffer.Render(identity);
    BufferObject::DisplayFullscreenTexture(finalRenderBuffer.GetBuffer("fragColor"));

    // Add cubes
    static int makeCube = 0;
    makeCube++;
    if (makeCube > 4 && cubes.size() < 200)
    {
      makeCube = 0;
      cubes.push_back(world.AddCube(vec3(rand() % 200, 96, 0 - rand() % 200), vec3(10, 10, 10), 1));
      cubes[cubes.size() - 1].ApplyForce(vec3(rand() % 1000 - 500, rand() % 1000, rand() % 1000 - 500));
    }


    // Shooting
    static bool shooting = false;
    if (Controls::GetControllerButton(12))
    {
      if (!shooting)
      {
        vec3 camPos = vec3() - Camera::Position();
        vec3 camDir = vec3() - Camera::Direction();
        cubes.push_back(world.AddCube(camPos + camDir * 6, vec3(10, 10, 10), 1));
        cubes[cubes.size() - 1].ApplyForce(camDir * 8000);
      }
      shooting = true;
    }
    else
    {
      shooting = false;
    }

    // Jumping
    bool FeetNearGround = false;
    for (int x = -4; x < 4; x++)
      for (int y = -4; y < 4; y++)
        FeetNearGround |= world.RayCast(CamSphere.GetPos() + vec3(x, -4, y), CamSphere.GetPos() + vec3(x, -8, y));

    bool OnGround = FeetNearGround;
    if (Controls::GetControllerButton(10) && OnGround && abs(CamSphere.GetSpeed().y) < 0.1)
          CamSphere.ApplyForce(vec3(0, 500, 0));


    vec3 lastCam = Camera::Position();
    Camera::Update(20, true);
    vec3 newCam = Camera::Position();
    newCam.y = lastCam.y;

    vec3 moveDir = vec3() - (newCam - lastCam);
    bool IsMoving = (newCam - lastCam).Length() > 0.005;

    bool HardStopping = moveDir.Normalized().DotProduct(CamSphere.GetSpeed().Normalized()) < -0.01;

    if (OnGround)
    {
      if (IsMoving)
        if(HardStopping)
          CamSphere.ApplyForce(moveDir * 30);
        else
          CamSphere.ApplyForce(moveDir * 100);
      if (!IsMoving || HardStopping)
      {
        vec3 stopDir = vec3() - CamSphere.GetSpeed() * 5;
        stopDir.y = 0;
        CamSphere.ApplyForce(stopDir);
      }
    }
    else
    {
      CamSphere.ApplyForce(moveDir * 20);
    }

    if (Controls::GetControllerButton(11))
      world.UpdateWorld(1.0f / 300.0f);
    else
      world.UpdateWorld(1.0f / 30.0f);

    Camera::SetPosition(vec3() - CamSphere.GetPos());

    window.Swap();
    FrameRate::Update();
  }

}