#include "SciFi.h"
#include "Window.h"
#include "Controls.h"
#include "PolyModel.h"
#include "Camera.h"
#include "CustomRenderObjects.h"
#include <time.h>
#include "Assets.h"
#include "Shaders.h"

void _SetShaders(RenderObject *pModel, std::string vert, std::string frag) { pModel->AssignShader(vert.c_str(), frag.c_str()); }

PolyModel *LoadModel(std::string filePath)
{
  PolyModel *ret = new PolyModel();
  ret->LoadModel(filePath.c_str());

  RenderObject *Pros = nullptr; int64_t meshNum = 0;
  if (false)
  {
    ret->GetMeshData(&meshNum, &Pros); for (int i = 0; i < meshNum; i++)
      _SetShaders(Pros + i, SHADERDIR "TexturedRenderObject.vert", SHADERDIR "TexturedRenderObject.frag");
  }

  if (true)
  {
    ret->GetMeshData(&meshNum, &Pros); for (int i = 0; i < meshNum; i++)
    {
      // Generate Normals
      int64_t vertNum;
      vec3 *posData;
      std::vector<vec3> normalData;
      //normalData.reserve(vertNum);
      Pros[i].AccessAttribute("position0", &vertNum, nullptr, nullptr, (const void**)&posData);
      if (vertNum)
      {
        _SetShaders(Pros + i, ASSETDIR "SciFi/shaders/specularRenderObject.vert", ASSETDIR "SciFi/shaders/specularRenderObject.frag");
        for (int tri = 0; tri < vertNum; tri += 3)
        {
          vec3 normal = ((posData[tri + 1] - posData[tri + 0]).CrossProduct(posData[tri + 2] - posData[tri + 0])).Normalized();
          normalData.push_back(normal);
          normalData.push_back(normal);
          normalData.push_back(normal);
        }
        Pros[i].AssignAttribute("normal0", AT_FLOAT, normalData.data(), 3, normalData.size());
      }
    }
  }

  return ret;
}

void DrawModel(PolyModel * pModel, mat4 viewProjection, vec3 pos = vec3(0, 0, 0), vec3 dir = vec3(0, 0, 0), vec3 scale = vec3(1, 1, 1))
{
  mat4 modelMat;
  modelMat.Translate(pos);
  modelMat.Scale(scale);
  modelMat.Rotate(dir);
  mat4 MVP = viewProjection * modelMat;
  modelMat.inverse();
  MVP.Transpose();
  RenderObject *Pros = nullptr; int64_t meshNum = 0;
  pModel->GetMeshData(&meshNum, &Pros); for (int i = 0; i < meshNum; i++)
  {
    vec3 camPos = Camera::Position();
    vec3 camDir = Camera::Direction();
    Pros[i].AssignUniform("camPos", UT_3f, camPos.Data());
    Pros[i].AssignUniform("camDir", UT_3f, camDir.Data());
    Pros[i].AssignUniform("modelMat", UT_mat4fv, &modelMat);
  }
  pModel->Render(MVP);
}

void SciFi()
{
  Window window("SciFi", true, 1920, 1080, true);
  //Window window("SciFi", true, 800, 600, false);

  mat4 projectionMat;
  projectionMat.Perspective(50.0f * (float)DegsToRads, (float)window.width / window.height, 2.0, 64000);

  glPointSize(2.0f);

  Camera::SetPosition({ -2120.39f, -8.496f, -1750.06f });
  Camera::SetRotation(vec2(0.228000134, 1.3240029));

  // Skybox
  PolyModel skybox;
  skybox.LoadModel(ASSETDIR "SciFi/skybox/Planet.obj");
  
  // Models
  PolyModel manhattanBar(ASSETDIR "SciFi/Docks/Manhattan/Bar/Bar.obj");
  PolyModel manhattanTrdaer(ASSETDIR "SciFi/Docks/Manhattan/CommodityShop/CommodityShop.obj");

  auto manhattan = LoadModel(ASSETDIR "SciFi/Models/Manhattan/Planet.obj");
  auto sol = LoadModel(ASSETDIR "SciFi/Models/sol/Planet.obj");
  auto fireball = LoadModel(ASSETDIR "SciFi/Models/fireball/Planet.obj");
  auto overcast = LoadModel(ASSETDIR "SciFi/Models/overcast/Planet.obj");
  auto pittsburgh = LoadModel(ASSETDIR "SciFi/Models/pittsburgh/Planet.obj");
  auto pittsburgh_moon = LoadModel(ASSETDIR "SciFi/Models/pittsburgh_moon/Planet.obj");
  auto dockingRing = LoadModel(ASSETDIR "SciFi/Models/DockingRing/DockingRing.obj");
  auto shipyard = LoadModel(ASSETDIR "SciFi/Models/Shipyard/Shipyard.obj");
  auto outpost = LoadModel(ASSETDIR "SciFi/Models/Outpost/Outpost.obj");
  auto station = LoadModel(ASSETDIR "SciFi/Models/station/station.obj");
  auto freeport = LoadModel(ASSETDIR "SciFi/Models/FreePort/FreePort.obj");
  auto factory = LoadModel(ASSETDIR "SciFi/Models/Factory/Factory.obj");
  auto sheriff = LoadModel(ASSETDIR "SciFi/Models/sheriff/sheriff.obj");
  auto lane = LoadModel(ASSETDIR "SciFi/Models/Lane/Lane.obj");
  auto battleship = LoadModel(ASSETDIR "SciFi/Models/BattleShip/BattleShip.obj");
  auto dreadnaught = LoadModel(ASSETDIR "SciFi/Models/BattleShip/BattleShip.obj");
  auto starlight = LoadModel(ASSETDIR "SciFi/Models/StarLight/StarLight.obj");
  auto asteroid_1 = LoadModel(ASSETDIR "SciFi/Models/Asteroids/Asteroid_1.obj");
  auto asteroid_2 = LoadModel(ASSETDIR "SciFi/Models/Asteroids/Asteroid_2.obj");
  auto asteroid_3 = LoadModel(ASSETDIR "SciFi/Models/Asteroids/Asteroid_3.obj");
  auto asteroid_4 = LoadModel(ASSETDIR "SciFi/Models/Asteroids/Asteroid_4.obj");

  PolyModel *asteroids[4] = { asteroid_1 ,asteroid_2, asteroid_3, asteroid_4 };

  std::vector<vec3>solRaySpd;
  std::vector<vec3>solRayRot;
  for (int i = 0; i < 35; i++)
  {
    solRaySpd.push_back(vec3(rand() % 256 / 256.f, rand() % 256 / 256.f, rand() % 256 / 256.f));
    solRayRot.push_back(vec3(rand() % 256 / 64.f, rand() % 256 / 64.f, rand() % 256 / 64.f));
  }

  // Stars
  ColouredRenderObjectMaker starMaker;
  for (int i = 0; i < 3000; i++)
  {
    vec4 dVec(1, 0, 0, 1);
    mat4 dMat;
    dMat.RotateX(DegsToRads * (rand() % 360));
    dMat.RotateY(DegsToRads * (rand() % 360));
    dMat.RotateZ(DegsToRads * (rand() % 360));
    float b = 0.5 + (rand() % 255) / 255.0f * 0.5;
    vec3 pos = (dVec * dMat) * rand() * 0.5;
    pos.y *= 0.1;
    starMaker.AddVertex(pos, { b, b, b });
  };
  RenderObject *stars = starMaker.AssembleRenderObject();

  // Asteroids
  std::vector<mat4> asteroidInstances;
  for (int i = 0; i < 200; i++)
  {
    mat4 pos;
    pos.Translate(vec3(2300, -100, -140 - 1500));
    pos.Translate(vec3(rand() % 1500, rand() % 200, rand() % 500));
    pos.RotateX(DegsToRads * (rand() % 360));
    pos.RotateY(DegsToRads * (rand() % 360));
    pos.RotateZ(DegsToRads * (rand() % 360));
    asteroidInstances.push_back(pos);
  }
  for (int i = 0; i < 500; i++)
  {
    mat4 pos;
    pos.Translate(vec3(2300, -100, -140 - 1000));
    pos.Translate(vec3(rand() % 1500, rand() % 200, rand() % 500));
    pos.RotateX(DegsToRads * (rand() % 360));
    pos.RotateY(DegsToRads * (rand() % 360));
    pos.RotateZ(DegsToRads * (rand() % 360));
    asteroidInstances.push_back(pos);
  }
  for (int i = 0; i < 300; i++)
  {
    mat4 pos;
    pos.Translate(vec3(2300, -100, -140 - 500));
    pos.Translate(vec3(rand() % 1500, rand() % 200, rand() % 500));
    pos.RotateX(DegsToRads * (rand() % 360));
    pos.RotateY(DegsToRads * (rand() % 360));
    pos.RotateZ(DegsToRads * (rand() % 360));
    asteroidInstances.push_back(pos);
  }
  for (int i = 0; i < 100; i++)
  {
    mat4 pos;
    pos.Translate(vec3(2300, -100, -140));
    pos.Translate(vec3(rand() % 1500, rand() % 200, rand() % 500));
    pos.RotateX(DegsToRads * (rand() % 360));
    pos.RotateY(DegsToRads * (rand() % 360));
    pos.RotateZ(DegsToRads * (rand() % 360));
    asteroidInstances.push_back(pos);
  }
  for (int i = 0; i < 50; i++)
  {
    mat4 pos;
    pos.Translate(vec3(2300, -100, 440));
    pos.Translate(vec3(rand() % 1500, rand() % 200, rand() % 500));
    pos.RotateX(DegsToRads * (rand() % 360));
    pos.RotateY(DegsToRads * (rand() % 360));
    pos.RotateZ(DegsToRads * (rand() % 360));
    asteroidInstances.push_back(pos);
  }

  Controls::SetMouseLock(true);

  while (Controls::Update())
  {
    window.Clear();

    // Player Ship
    static bool freeCam = true;
    static vec3 playerPos(0, 0, 420);
    static vec3 playerMom(0, 0, 0);
    static vec2 playerDir(0, 180.0 * DegsToRads);

    Camera::Update(60);

    // Player Controls
    if (Controls::KeyDown(SDL_SCANCODE_UP))
    {
      float shipSinY = sin(playerDir.y);
      float shipCosY = cos(playerDir.y);
      float shipSinX = sin(playerDir.x);
      float shipCosX = cos(playerDir.x);
      float speed = 1.0f;
      playerPos.x += shipSinY * shipCosX * speed;
      playerPos.y += shipSinX * speed;
      playerPos.z += shipCosY * shipCosX * speed;
      //playerMom = playerMom + ;
    }

    playerPos = playerPos + playerMom;

    if (!freeCam)
      Camera::SetPosition(vec3() - playerPos);
    if (Controls::KeyPressed(SDL_SCANCODE_1))
      freeCam = !freeCam;

    mat4 MVP;
    mat4 viewMat = Camera::Matrix();

    // Sky box
    mat4 skyMVP;
    skyMVP = projectionMat * Camera::RotationMatrix();
    skyMVP.Transpose();
    glDepthMask(GL_FALSE);
    skybox.Render(skyMVP);
    glDepthMask(GL_TRUE);

    // Stars
    mat4 modelMat;
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    stars->RenderPoints(MVP);

    DrawModel(&manhattanBar, projectionMat * viewMat, { 0, 0, 1800 }, {}, { 4, 4, 4 });
    DrawModel(&manhattanTrdaer, projectionMat * viewMat, { 200, 0, 1800 }, {}, { 4, 4, 4 });

    DrawModel(starlight, projectionMat * viewMat, playerPos, { playerDir.x, playerDir.y }, { 0.1f, 0.1f, 0.1f });

    // Manhattan
    DrawModel(manhattan, projectionMat * viewMat, {}, vec3(0, clock() * -0.000005f, 0), { 2, 2, 2 });
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glDepthMask(false);
    DrawModel(overcast, projectionMat * viewMat, {}, vec3(0, clock() * 0.000002f, 0), { 2.05f, 2.05f, 2.05f});
    glDepthMask(true);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);;

    // Sun
    DrawModel(sol, projectionMat * viewMat, { 0,0, 5000 }, vec3(0, clock() * -0.000005f, 0), { 4, 4, 4 });
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glDepthMask(false);

    float si = 0.05;
    for (int i = 0; i < solRaySpd.size(); i++)
    {
      solRayRot[i] = solRayRot[i] + solRaySpd[i]*0.0005;
      DrawModel(fireball, projectionMat * viewMat, { 0,0, 5000 }, solRayRot[i], { 4 + si, 4 + si, 4 + si });
      si += 0.01;
    }

    glDepthMask(true);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);;

    // PitsBurgh
    modelMat.LoadIdentity();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    DrawModel(pittsburgh, projectionMat * viewMat, { 4300, 0, 200 }, vec3(0, clock() * -0.00001f, 0), { 1.8f, 1.8f, 1.8f });

    DrawModel(dockingRing, projectionMat * viewMat, { 4025, 0, 200 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f }); // DockingRing
    DrawModel(outpost, projectionMat * viewMat, { 3940, 0, 350 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f }); // Outpost

    // PitsBurgh Moon
    modelMat.LoadIdentity();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    DrawModel(pittsburgh_moon, projectionMat * viewMat, { 4700, 0, 700 }, vec3(0, clock() * -0.00001f, 0), { 0.8f, 0.8f, 0.8f });

    DrawModel(dockingRing, projectionMat * viewMat, { 0, 0, 300 }, {}, { 0.1f, 0.1f, 0.1f }); // DockingRing
    DrawModel(freeport, projectionMat * viewMat, { 80, 0, 500 }, { 0, 180.0f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f }); // Freeport
    DrawModel(factory, projectionMat * viewMat, { 150, 0, 320 }, {}, { 0.1f, 0.1f, 0.1f }); // Factory

    // Lanes
    DrawModel(lane, projectionMat * viewMat, { 0, 0, 600 }, {}, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 0, 0, 1100 }, {}, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 0, 0, 1600 }, {}, { 0.1f, 0.1f, 0.1f });
    DrawModel(battleship, projectionMat * viewMat, { -80, 0, 450 }, { 0, 180.f * (float)DegsToRads, 0 }, { 0.2f, 0.2f, 0.2f });

    // Pits burg trade route
    DrawModel(lane, projectionMat * viewMat, { 300, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 800, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 1300, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 1800, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(factory, projectionMat * viewMat, { 2000, 0, 320 }, {}, { 0.1f, 0.1f, 0.1f }); // Factory
    DrawModel(sheriff, projectionMat * viewMat, { 2050, 15, 450 }, {}, { 0.1f, 0.1f, 0.1f }); // Sheriff
    DrawModel(lane, projectionMat * viewMat, { 2300, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 2800, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 3300, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 3800, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });

    // Battleship Yamato
    DrawModel(lane, projectionMat * viewMat, { 2000, 0, 600 }, {}, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 2000, 0, 1100 }, {}, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 2000, 0, 1600 }, {}, { 0.1f, 0.1f, 0.1f });
    DrawModel(battleship, projectionMat * viewMat, { 1980, 0, 1750 }, { 0, 150.f * (float)DegsToRads, 0 }, { 0.2f, 0.2f, 0.2f });
    DrawModel(station, projectionMat * viewMat, { 2050, 0, 1730 }, { 0, 90.f* (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f }); // Station
    DrawModel(shipyard, projectionMat * viewMat, { 2040, -7, 1718 }, {}, { 0.1f, 0.1f, 0.1f }); // Shipyard
    DrawModel(shipyard, projectionMat * viewMat, { 2040, -7, 1748 }, {}, { 0.1f, 0.1f, 0.1f }); // Shipyard

    // Bandit Asteroid base
    DrawModel(outpost, projectionMat * viewMat, { 3000, 0, -1000 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f }); // Outpost
    DrawModel(station, projectionMat * viewMat, { 3030, 0, -1000 }, {}, { 0.1f, 0.1f, 0.1f }); // Station

    // Pits burg asteroid field
    int astID = 0;
    for (auto & asteroidPos : asteroidInstances)
      DrawModel(asteroids[astID++ % 4], projectionMat * viewMat* asteroidPos, { 0, 0, 0 }, { 0, 0, 0 }, { 0.3f, 0.3f, 0.3f });

    window.Swap();
  }

}
