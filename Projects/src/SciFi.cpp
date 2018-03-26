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
  pModel;
  mat4 modelMat;
  modelMat.Translate(pos);
  modelMat.Scale(scale);
  modelMat.Rotate(dir);
  mat4 MVP = viewProjection * modelMat;
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
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 1.0 / 4.0, 64000);

  glPointSize(2.0f);

  Camera::SetPosition({ 0, 0, -500 });

  // Skybox
  PolyModel skybox;
  skybox.LoadModel(ASSETDIR "SciFi/skybox/Planet.obj");

  // Stars
  ColouredRenderObjectMaker starMaker;
  for (int i = 0; i < 4000; i++)
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

  // Models
  PolyModel manhattan, pittsburgh, pittsburgh_moon;
  manhattan.LoadModel(ASSETDIR "SciFi/Models/Manhattan/Planet.obj");
  pittsburgh.LoadModel(ASSETDIR "SciFi/Models/pittsburgh/Planet.obj");
  pittsburgh_moon.LoadModel(ASSETDIR "SciFi/Models/pittsburgh_moon/Planet.obj");
  auto dockingRing = LoadModel(ASSETDIR "SciFi/Models/DockingRing/DockingRing.obj");
  auto outpost = LoadModel(ASSETDIR "SciFi/Models/Outpost/Outpost.obj");
  auto freeport = LoadModel(ASSETDIR "SciFi/Models/FreePort/FreePort.obj");
  auto factory = LoadModel(ASSETDIR "SciFi/Models/Factory/Factory.obj");
  auto lane = LoadModel(ASSETDIR "SciFi/Models/Lane/Lane.obj");



  Controls::SetMouseLock(true);

  while (Controls::Update())
  {
    window.Clear();

    Camera::Update(60);

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

    // Manhattan
    modelMat.LoadIdentity();
    modelMat.Scale(2);
    modelMat.RotateY(clock() * -0.00002f);
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    manhattan.Render(MVP);


    // PitsBurgh
    modelMat.LoadIdentity();
    modelMat.Translate(4300, 0, 200);
    modelMat.Scale(1.8);
    modelMat.RotateY(clock() * -0.00002f);
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    pittsburgh.Render(MVP);
    DrawModel(dockingRing, projectionMat * viewMat, { 4025, 0, 200 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f }); // DockingRing
    DrawModel(outpost, projectionMat * viewMat, { 3940, 0, 350 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f }); // Outpost

    // PitsBurgh Moon
    modelMat.LoadIdentity();
    modelMat.Translate(4700, 0, 700);
    modelMat.Scale(0.8);
    modelMat.RotateY(clock() * -0.00002f);
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();
    pittsburgh_moon.Render(MVP);

    DrawModel(dockingRing, projectionMat * viewMat, { 0, 0, 300 }, {}, { 0.1f, 0.1f, 0.1f }); // DockingRing
    DrawModel(freeport, projectionMat * viewMat, { 80, 0, 500 }, { 0, 180.0f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f }); // Freeport
    DrawModel(factory, projectionMat * viewMat, { 150, 0, 320 }, {}, { 0.1f, 0.1f, 0.1f }); // Factory

    // Lanes
    DrawModel(lane, projectionMat * viewMat, { 0, 0, 600 }, {}, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 0, 0, 1100 }, {}, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 0, 0, 1600 }, {}, { 0.1f, 0.1f, 0.1f });

    // Pits burg trade route
    DrawModel(lane, projectionMat * viewMat, { 300, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 800, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 1300, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 1800, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(factory, projectionMat * viewMat, { 1900, 0, 320 }, {}, { 0.1f, 0.1f, 0.1f }); // Factory
    DrawModel(lane, projectionMat * viewMat, { 2300, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 2800, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 3300, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });
    DrawModel(lane, projectionMat * viewMat, { 3800, 0, 400 }, { 0, 90.f * (float)DegsToRads, 0 }, { 0.1f, 0.1f, 0.1f });

    window.Swap();
  }


}

