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
#include "Audio.h"

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

  Audio::PlayMP3(ASSETDIR "Halo/Journal.mp3", 1000, "Journal", true);

#ifdef _DEBUG
  Window window("Physics", true, 640, 480, false);
#else
  //Window window("Physics", true, 800, 600, false);
  Window window("Physics", true, 1920, 1080, true);
#endif
  float superSample = 1;

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 10, 20000.0f);
  window.Swap();

  RenderObject *cubeMesh = MakeCube();

  PolyModel colorModel, lightModel;

  PolyModel pistol;
  pistol.LoadModel(ASSETDIR "Halo/Pistol/Pistol.obj");

  PolyModel grunt;
  grunt.LoadModel(ASSETDIR "Halo/Characters/Grunt.obj");

  colorModel.LoadModel(ASSETDIR "Halo/Bridge/Color/Color.obj");
  lightModel.LoadModel(ASSETDIR "Halo/Bridge/Light/Light.obj");

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

  auto gruntObject = world.AddSphere(vec3(), 35, 1);
  
  Camera::SetPosition(vec3(-296.536285, -13606.2158, -5291.29980));

  PhysicsObject CamSphere = world.AddSphere(vec3() - Camera::Position(), 40, 1);
  CamSphere.SetCanSleep(false);

  const float boxSize = 20.0f;

  while (Controls::Update())
  {
    window.Clear(0, 190, 255);
    mat4 viewMat = Camera::Matrix();

    actorColorBuffer.ClearRenderInstances();

    static vec2 lastRot;
    static vec2 WER; // WeaponExtraRotation

    mat4 weaponMat;
    weaponMat.Translate(vec3() - Camera::Position());
    weaponMat.Translate(vec3() - Camera::Direction() * 30);
    weaponMat.RotateY(0 - Camera::Rotation().y);
    weaponMat.RotateX(0 - Camera::Rotation().x);

    weaponMat.RotateZ(DegsToRads * 270);
    weaponMat.RotateY(DegsToRads * 90);
    weaponMat.RotateZ(DegsToRads * 180);

    weaponMat.Translate(vec3(0, -10, -20));

    weaponMat.RotateY(-WER.x * 0.8);
    weaponMat.RotateZ(WER.y*1.1);

    vec2 rotChange = Camera::Rotation() - lastRot;
    WER = WER - rotChange * 0.5;
    lastRot = Camera::Rotation();

    WER = WER* 0.95;

    WER.x = WER.x * (0.9 + Min(abs(rotChange.x * 5000), 1.0f) * 0.1);
    WER.y = WER.y * (0.9 + Min(abs(rotChange.y * 5000), 1.0f) * 0.1);

    actorColorBuffer.AddPolyModel(&pistol, weaponMat);

    mat4 gruntModelMat = gruntObject.GetModelMat();
    gruntModelMat.Translate(vec3(0, -30, 0));
    actorColorBuffer.AddPolyModel(&grunt, gruntModelMat);

    if(Controls::KeyDown(SDL_SCANCODE_4))
    gruntObject.ApplyForce(vec3(10, 0, 0), vec3(0,1.8,0));

    for(const auto &cube : cubes)
    {
      mat4 scaler;
      scaler.Scale(boxSize);
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
    //makeCube++;
    if (makeCube > 4 && cubes.size() < 200)
    {
      makeCube = 0;
      cubes.push_back(world.AddCube(vec3(rand() % 200, 96, 0 - rand() % 200), vec3(10, 10, 10), 1));
      cubes[cubes.size() - 1].ApplyForce(vec3(rand() % 1000 - 500, rand() % 1000, rand() % 1000 - 500));
    }

    static bool heldRight = false;
    if(Controls::GetRightClick())
    {
      if(heldRight == false)
      {
        vec3 hitPoint;
        world.RayCast(CamSphere.GetPos(), CamSphere.GetPos() - Camera::Direction() * 100000, &hitPoint);
        cubes.push_back(world.AddCube(hitPoint + Camera::Direction() * 100, vec3(10, 10, 10), 1));
        cubes[cubes.size() - 1].ApplyForce(vec3(0, 200, 0));
      }
      heldRight = true;
    }
    else
      heldRight = false;


    static bool boxriver = false;

    // box river
    static std::vector<int64_t> boxRiverBoxes;
    if (!boxriver)
    {
      for (int64_t i = 0; i < 100; i++)
      {
        cubes.push_back(world.AddCube(vec3(128, 128, 128), vec3(boxSize, boxSize, boxSize), 0.2));
        boxRiverBoxes.push_back(cubes.size() - 1);
      }
      boxriver = true;
    }

    for (auto & boxid : boxRiverBoxes)
    {
      cubes[boxid].ApplyForce(vec3(0, 0, 40));
      if ((cubes[boxid].GetSpeed().LengthSquared() < 10) || ((cubes[boxid].GetPos() - vec3(128, 128, 128)).LengthSquared() > 100))
        cubes[boxid].SetPos(vec3(128, 128, 128));
    }

    // Shooting
    static bool shooting = false;
    if (Controls::GetControllerButton(12) || Controls::GetLeftClick())
    {
      if (!shooting)
      {
        vec3 camPos = vec3() - Camera::Position();
        vec3 camDir = vec3() - Camera::Direction();

        vec3 weaponDir = weaponMat * vec4(1, 0, 0, 0);

        cubes.push_back(world.AddCube(camPos + weaponDir * (40 + boxSize), vec3(boxSize, boxSize, boxSize), 1));
        cubes[cubes.size() - 1].ApplyForce(weaponDir * 40000);
        WER.x -= 0.4;
      }
      shooting = true;
    }
    else
      shooting = false;

    if (!Controls::KeyDown(SDL_SCANCODE_5))
      for (auto cube : cubes)
        cube.ApplyForce(vec3(0, 0, 100));

    if (Controls::KeyDown(SDL_SCANCODE_2))
      for (auto cube : cubes)
        cube.ApplyForce(vec3(rand() % 1000 - 500, 500, rand() % 1000 - 500));

    // Jumping
    bool FeetNearGround = false;
    for (int x = -40; x < 40; x+=10)
      for (int y = -40; y < 40; y+=10)
        FeetNearGround |= world.RayCast(CamSphere.GetPos() + vec3(x, -35, y), CamSphere.GetPos() + vec3(x, -60, y));

    bool OnGround = FeetNearGround;
    if ((Controls::KeyDown(SDL_SCANCODE_SPACE) || Controls::GetControllerButton(10)) && OnGround)
          CamSphere.ApplyForce(vec3(0, (500 * 1.5 * 2) / 5, 0));

    vec3 lastCam = Camera::Position();
    Camera::Update(140, true);
    vec3 newCam = Camera::Position();
    //newCam.y = lastCam.y;

    vec3 moveDir = vec3() - (newCam - lastCam);
    bool IsMoving = (newCam - lastCam).Length() > 0.005;

    bool HardStopping = moveDir.Normalized().DotProduct(CamSphere.GetSpeed().Normalized()) < -0.01;

    CamSphere.ApplyForce(moveDir * 100);

    vec3 frictionVector = (vec3() - CamSphere.GetSpeed()) * 1;
    frictionVector.y = 0;

    CamSphere.ApplyForce(frictionVector * 2);

    if (Controls::GetControllerButton(11))
      world.UpdateWorld(1.0f / 10.0f);
    else
      world.UpdateWorld(1.0f / 10.0f);

    Camera::SetPosition(vec3() - CamSphere.GetPos());

    Camera::SetPosition(Camera::Position() - vec3(0, 30, 0));

    window.Swap();
    FrameRate::Update();
  }

}