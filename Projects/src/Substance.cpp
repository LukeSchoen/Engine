#include "Substance.h"
#include "Window.h"
#include "Controls.h"
#include "Camera.h"
#include "FrameRate.h"
#include "RenderObject.h"
#include "Assets.h"
#include "CustomRenderObjects.h"
#include <time.h>
#include <algorithm>

struct Material
{
  Material(vec3 col, bool a_moveable, bool a_gravity, bool a_liquid, float a_mass, float a_bond)
  {
    moveable = a_moveable;
    gravity = a_gravity;
    mass = a_mass;
    bond = a_bond;
    color = col;
    liquid = a_liquid;
  }

  bool moveable = true;
  bool gravity = true;
  float mass = 1.0;
  float bond = 1.0;
  bool liquid = false;
  vec3 color;
};

struct Connection
{
  int p;
  int o;
};

const static Material adminium({ 0.7 ,0.7, 0.7 }, false, false, false, 0.0, 1.0f);
const static Material wood({ 0.7 ,0.5, 0.2 }, true, true, false, 1.0f, 1.0f);
const static Material water({ 0.0 ,0.0, 1.0 }, true, true, true, 1.0f, 0.1f);

std::vector<vec3> posData;
std::vector<vec3> colData;
std::vector<vec3> momData;
std::vector<float> masData;
std::vector<bool> liqData;

std::vector<Connection> connections;
RenderObject mesh;

RenderObject* lines = nullptr;


float strength = 0.5;
float bounce = 0.5;
float momentum = 0.95;
float bondRange = 0.2;
float bounceRange = 0.41;

vec3 gravity(0, -0.00005f, 0);

void RebuildMesh()
{
  mesh.AssignAttribute("position", GLAttributeType::AT_FLOAT, posData.data(), 3, posData.size());
  mesh.AssignAttribute("color", GLAttributeType::AT_FLOAT, colData.data(), 3, colData.size());
}

void AddVoxel(vec3 pos, const Material & mat, vec3 mom = vec3())
{
  posData.push_back(pos);
  colData.push_back(mat.color);
  masData.push_back(mat.mass);
  momData.push_back(mom);
  liqData.push_back(mat.liquid);
}

void AddVoxel(vec3 pos, vec3 col, vec3 mom = vec3(), float mass = 1.0, bool liquid = false)
{
  posData.push_back(pos);
  colData.push_back(col);
  masData.push_back(mass);
  momData.push_back(mom);
  liqData.push_back(liquid);
}

void AddBox(vec3 pos = vec3(0, 0, 0), vec3i size = vec3i(1, 1, 1), const Material & mat = wood)
{
  for (int64_t z = 0; z < size.z; z++)
    for (int64_t y = 0; y < size.y; y++)
      for (int64_t x = 0; x < size.x; x++)
        AddVoxel(pos + vec3(x, y, z), mat);
}

void AddColorCube(vec3 pos = vec3(0, 0, 0))
{
  for (int64_t z = 0; z < 10; z++)
    for (int64_t y = 0; y < 10; y++)
      for (int64_t x = 0; x < 10; x++)
        AddVoxel(pos + vec3(x*1.05, y, z), vec3(x / 10.0f, y / 10.0f, z / 10.0f));
}

void AddWater(vec3 pos, vec3 size, const Material & mat = wood)
{
  for (int64_t z = 0; z < size.z; z++)
    for (int64_t y = 0; y < size.y; y++)
      for (int64_t x = 0; x < size.x; x++)
      {
        if (x & 1 && y & 1 && z & 1)
        {
          float brightness = (rand() / (0.0f + RAND_MAX)) * 0.5 + 0.5;
          AddVoxel(pos + vec3(x, y, z), vec3(0.8, 0.1, 0.2) * brightness, vec3(0, -3, 0) * 0.01, 1.0, true);
          //AddVoxel(pos + vec3(x, y, z), vec3(0.8, 0.1, 0.2) * brightness, (vec3(rand() / (0.0f + RAND_MAX), rand() / (0.0f + RAND_MAX), rand() / (0.0f + RAND_MAX)) - vec3(0.5, 5, 0.5)) * 0.01, 1.0, true);
          //AddVoxel(pos + vec3(x, y, z), vec3(0.2, 0.4, 1.0) * brightness, (vec3(rand() / (0.0f + RAND_MAX), rand() / (0.0f + RAND_MAX), rand() / (0.0f + RAND_MAX)) - vec3(0.5, 15, 0.5)) * 0.01, 1.0, true);
        }
      }
}

void AddPaddle(vec3 pos)
{
  AddBox(pos + vec3(11, 20, 10), { 1,1,10 }, adminium);
  AddBox(pos + vec3(10, 22, 10), { 3,10,10 }, wood);
  AddBox(pos + vec3(10, 9, 10), { 3,10,10 }, wood);
  AddBox(pos + vec3(13, 19, 10), { 10,3,10 }, wood);
  AddBox(pos + vec3(0, 19, 10), { 10,3,10 }, wood);

  for (int64_t z = 0; z < 10; z++)
    for (int64_t y = 0; y < 3; y++)
      for (int64_t x = 0; x < 3; x++)
        if (!(x == 1 && y == 1))
          AddVoxel(pos + vec3(10, 19, 10) + vec3(x, y, z), wood);
}

void AddGear(vec3 pos, float radius, int depth)
{
  AddBox(pos + vec3(0, 0, 0), { 1,1,depth }, adminium);
  for (int y = -10; y < 11; y++)
    for (int x = -10; x < 11; x++)
      if (!(x == 0 && y == 0))
      {
        if ((x*x + y*y) < radius)
          AddBox(pos + vec3(x, y, 0), { 1,1,depth }, wood);
      }
}

void AddCylinder(vec3 pos, float radius, int depth)
{
  for (int y = -32; y < 33; y++)
    for (int x = -32; x < 33; x++)
    {
      float d = sqrt(x*x + y*y);
      if (d < 12 && d > 9.5)
        AddBox(pos + vec3(x, 0, y), { 1,depth ,1 }, wood);
    }
}

void AddDisc(vec3 pos, float radius, int depth)
{
  for (int y = -32; y < 33; y++)
    for (int x = -32; x < 33; x++)
    {
      float d = sqrt(x*x + y*y);
      if (d < 12)
        AddBox(pos + vec3(x, 0, y), { 1,depth ,1 }, wood);
    }
}

void BigGear(vec3 pos, int depth)
{
  AddGear(pos, 20, depth);
}

void AddGearSet(vec3 pos)
{
  AddGear(pos, 10, 3);
  AddGear(pos + vec3(40 - 33.6, 1.2, 0), 10, 3);
}

void AmbientOcclusion()
{
  for (int64_t p = 0; p < posData.size(); p++)
  {
    float occlusion = 0;
    for (int64_t o = 0; o < posData.size(); o++)
      if (p != o)
      {
        float dist = (posData[p] - posData[o]).Length();
        if (dist < 3)
          occlusion += 1.0 / dist;
      }
    float m = std::min(20.0 / occlusion, 1.0);
    colData[p] = colData[p] * m;
  }
}

void CalculateConection(int p, int o)
{
  vec3 dir = posData[p] - posData[o];
  float dist = (dir).Length();

  float pMass = masData[p];
  float oMass = masData[o];

  float averageMass = (pMass + oMass) * 0.5;

  // bouncing
  if (dist < 1 + bounceRange && dist >= 1 + bondRange)
  {
    if (pMass > 0)
    {
      posData[p] = posData[p] + dir * (bounceRange + 1 - dist)*strength * bounce;
      momData[p] = momData[p] + dir * (bounceRange + 1 - dist)*strength * bounce * momentum;
    }
    if (oMass > 0)
    {
      posData[o] = posData[o] - dir * (bounceRange + 1 - dist)*strength * bounce;
      momData[o] = momData[o] - dir * (bounceRange + 1 - dist)*strength * bounce * momentum;
    }
  }

  // bonding
  if (dist < 1 + bondRange)
  {
    if (pMass > 0 && !liqData[p])
    {
      posData[p] = posData[p] + dir * (1 - dist)*strength;
      momData[p] = momData[p] + dir * (1 - dist)*strength * momentum;
    }
    if (oMass > 0 && !liqData[o])
    {
      posData[o] = posData[o] - dir * (1 - dist)*strength;
      momData[o] = momData[o] - dir * (1 - dist)*strength * momentum;
    }
  }
}

void CalculateNextStep()
{
  // Step
  for (int64_t p = 0; p < posData.size(); p++)
  {
    // Update position
    posData[p] = posData[p] + momData[p];

    // Hit ground
    if (posData[p].y < 0)
    {
      posData[p].y = 0;
      momData[p].y = 0;
      momData[p].x *= 0.9;
      momData[p].z *= 0.9;
    }

    // Gravity
    if (masData[p] > 0)
      momData[p] = momData[p] + gravity;
  }
}

void SimulateConnections()
{
  for (auto & connection : connections)
    CalculateConection(connection.p, connection.o);
  CalculateNextStep();
}

void ExtractConnections()
{
  connections.clear();
  ColouredRenderObjectMaker makerMaker;
  for (int p = 0; p < posData.size() - 1; p++)
    for (int o = p + 1; o < posData.size(); o++)
    {
      float maxMom = (momData[p].Length() + momData[o].Length());
      vec3 dir = posData[p] - posData[o];
      float dist = (dir).Length();
      if (dist < 2.0 + maxMom * 200)
      {

        //if (dist > 3.0)
        //{
        //  vec3 newPpos = posData[p] + momData[p];
        //  vec3 newOpos = posData[o] + momData[o];
        //  float newDist = (newPpos - newOpos).Length();
        //  if(newDist < dist)
        //    connections.push_back({ p, o });
        //}
        //else
          connections.push_back({ p, o });

        if (dist < 1.0)
        {
          makerMaker.AddVertex(posData[p], vec3(1, 0, 0));
          makerMaker.AddVertex(posData[o], vec3(1, 0, 0));
        }
        else if (dist < 1.2)
        {
          makerMaker.AddVertex(posData[p], vec3(0, 1, 0));
          makerMaker.AddVertex(posData[o], vec3(0, 1, 0));
        }
        else if (dist < 1.41)
        {
          makerMaker.AddVertex(posData[p], vec3(1, 0, 0));
          makerMaker.AddVertex(posData[o], vec3(1, 0, 0));
        }
        else
        {
          makerMaker.AddVertex(posData[p], vec3(0, 0, 1));
          makerMaker.AddVertex(posData[o], vec3(0, 0, 1));
        }
      }
    }

  delete lines;
  lines = makerMaker.AssembleRenderObject();
}

void Start()
{
  vec3 off(54, 0, 54);

  //AddColorCube(vec3(32, 32, 32));

  // Wine
  if (true)
  {
    AddCylinder(vec3(32, 23, 32), 32, 15);
    AddDisc(vec3(32, 20, 32), 32, 3);
    AddBox(vec3(32 - 3, 5, 32 - 3), vec3(7, 15, 7));
    AddDisc(vec3(32, 0, 32), 32, 5);
    AddWater(vec3(25, 24, 25), vec3(15, 75, 15));
  }


  //Gears
  if (false)
  {
    AddGearSet(vec3(44, 20, 0));
    AddBox(vec3(44 - 1, 20 - 1, 3), { 3,3,7 }, wood);
  }

  // Table
  if (false)
  {
    //legs
    AddBox(off + vec3(0, 0, 0), { 4,20,4 }, wood);
    AddBox(off + vec3(16, 0, 0), { 4,20,4 }, wood);
    AddBox(off + vec3(0, 0, 16), { 4,20,4 }, wood);
    AddBox(off + vec3(16, 0, 16), { 4,20,4 }, wood);
    // Connections
    AddBox(off + vec3(4, 5, 0), { 12,1,1 }, wood);
    AddBox(off + vec3(4, 5, 19), { 12,1,1 }, wood);
    AddBox(off + vec3(0, 5, 4), { 1,1,12 }, wood);
    AddBox(off + vec3(19, 5, 4), { 1,1,12 }, wood);
    // Top
    AddBox(off + vec3(0, 20, 0), { 20,3,20 }, wood);
  }

  // Water
  if (false)
  {
    AddWater(off + vec3(0, 50, 0), vec3(20, 50, 20), water);
  }

  // swinging ball
  if (false)
  {
    for (int i = 0; i < 3; i++)
    {
      vec3 off(i * 10, 0, 0);
      AddVoxel(off + vec3(32 + 5, 42 + 30, 32 + 5), vec3(1.0, 1.0, 1.0), vec3(), 0);
      AddBox(off + vec3(32 + 5, 42 + 5, 32 + 5), vec3(1, 25, 1), wood);
      AddBox(off + vec3(32 + 5, 42, 32 + 5), vec3(5, 5, 5), wood);
    }
  }

  // Water pipe
  if (false)
  {
    AddPaddle({ 4,0,0 });
    AddPaddle({ 33,0,0 });
    AddBox(vec3(38, 50, 10), { 1,30,10 }, adminium);
    AddBox(vec3(20, 50, 10), { 1,30,10 }, adminium);
    AddBox(vec3(20, 50, 9), { 19,30,1 }, adminium);
    AddBox(vec3(20, 50, 20), { 19,30,1 }, adminium);
    AddBox(vec3(20, 80, 10), { 19,1,10 }, adminium);
  }

  AmbientOcclusion();
  ExtractConnections();
}

void SimulateParticles()
{
  for (int p = 0; p < posData.size() - 1; p++)
    for (int o = p + 1; o < posData.size(); o++)
      CalculateConection(p, o);
  CalculateNextStep();
}

void Substance()
{
  //Window window("Game", true, 800, 600, false);
  Window window("Game", true, 1920, 1080, true);

  Start();
  RebuildMesh();
  glEnable(GL_PROGRAM_POINT_SIZE);
  mesh.AssignShader(ASSETDIR "Substance/shaders/Substance.vert", ASSETDIR "Substance/shaders/Substance.frag");

  TexturedRenderObjectMaker makerMaker;
  makerMaker.AddVertex(vec3(-0.5, -0.5, -0.5), vec2(0, 0));
  makerMaker.AddVertex(vec3(128.5, -0.5, -0.5), vec2(1, 0));
  makerMaker.AddVertex(vec3(128.5, -0.5, 128.5), vec2(1, 1));
  makerMaker.AddVertex(vec3(-0.5, -0.5, -0.5), vec2(0, 0));
  makerMaker.AddVertex(vec3(-0.5, -0.5, 128.5), vec2(0, 1));
  makerMaker.AddVertex(vec3(128.5, -0.5, 128.5), vec2(1, 1));
  makerMaker.SetTexture(ASSETDIR "Substance/grid.png");
  RenderObject *floorMesh = makerMaker.AssembleRenderObject();

  mat4 projectionMat;
  Controls::SetMouseLock(true);

  Camera::SetPosition(vec3(51.4112930, -82.1524200, 48.1408844));
  Camera::SetRotation(vec2(0.464999855, -2.35599828));

  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.02, 8000.0f);

  static bool thickMode = true;

  while (Controls::Update())
  {
    window.Clear(0, 0, 0);
    Camera::Update(50);
    mat4 MVP;
    mat4 modelMat;
    mat4 viewMat = Camera::Matrix();
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();

    float pointSize = 0.5;
    if (thickMode)
      pointSize = 1.4;
    mesh.AssignUniform("size", UT_1f, &pointSize);
    if (Controls::KeyDown(SDL_SCANCODE_3))
    {
      lines->RenderLines(MVP);
    }
    else
    {
      mesh.RenderPoints(MVP);
      floorMesh->Render(MVP);
    }

    glDepthFunc(GL_ALWAYS);
    glDepthFunc(GL_LESS);

    window.Swap();
    FrameRate::Update();

    if (Controls::KeyDown(SDL_SCANCODE_R))
    {
      posData.clear();
      colData.clear();
      momData.clear();
      masData.clear();
      liqData.clear();
      connections.clear();
      Start();
    }

    static bool thiockWasDown = false;
    if (Controls::KeyDown(SDL_SCANCODE_1))
    {
      if (!thiockWasDown)
      {
        thickMode = !thickMode;
        thiockWasDown = true;
      }
    }
    else
      thiockWasDown = false;


    static bool cubeWasDown = true;
    if (Controls::KeyDown(SDL_SCANCODE_C))
    {
      if (!cubeWasDown)
      {
        AddColorCube(vec3(0, 0, 0));
        ExtractConnections();
        cubeWasDown = true;
      }
    }
    else
      cubeWasDown = false;


    static bool accelerator = true;
    static bool accellWasDown = false;
    if (Controls::KeyDown(SDL_SCANCODE_KP_MULTIPLY))
    {
      if (!accellWasDown)
      {
        accelerator = !accelerator;
        accellWasDown = true;
      }
    }
    else
      accellWasDown = false;



    static bool skipDiscovery = false;
    static bool skipDiscoveryDown = false;
    if (Controls::KeyDown(SDL_SCANCODE_2))
    {
      if (!skipDiscoveryDown)
      {
        skipDiscovery = !skipDiscovery;
        skipDiscoveryDown = true;
      }
    }
    else
      skipDiscoveryDown = false;


    if (Controls::KeyDown(SDL_SCANCODE_KP_MINUS))
      continue;

    if (!Controls::KeyDown(SDL_SCANCODE_KP_PLUS))
    {
      int startTime = clock();
      int timeTaken = 0;
      while (clock() - startTime + timeTaken < 10)
      {
        int before = clock();

        if (Controls::GetLeftClick())
        {
          static int shot = 0;
          if (shot++ > 10000);
          {
            shot = 0;
            AddVoxel(vec3() - Camera::Position() - Camera::Direction() * 4, vec3(1, 1, 0), vec3() - Camera::Direction() * 0.1);
          }
        }

        if (Controls::GetRightClick())
          AddVoxel(vec3() - Camera::Position() - Camera::Direction() * 4, vec3(1, 1, 0), vec3() - Camera::Direction());

        // Spawn water

        static int waterWave = 0;
        if (waterWave++ % 50 == 1 && waterWave / 50 < 35)
        {
          //if (waterWave++ > 15)
          {
            //for (int x = 0; x < 7; x++)
            {
              //for (int y = 0; y < 4; y++)
              {
              //AddWater(vec3(26, 40, 26), vec3(10, 5, 10));
                //float brightness = (rand() / (0.0f + RAND_MAX)) * 0.5 + 0.5;
                //AddVoxel(vec3(22 + x*2, 75, 11 + y*2), vec3(0.2, 0.4, 1.0) * brightness, vec3(0.01 * ((rand() / (0.0f + RAND_MAX)) * 2 - 1), -0.1, 0.01 * ((rand() / (0.0f + RAND_MAX)) * 2 - 1)), 1.0, true);
              }
            }
            //waterWave = 0;
          }
        }

        if (true)
        {
          if (skipDiscovery)
            SimulateConnections();
          else
            if (!accelerator)
            {
              SimulateParticles();
            }
            else
            {
              int fastSteps = 200;
              static int step = 0;
              if (step++ < fastSteps)
                SimulateConnections();
              else
              {
                step = 0;
                SimulateParticles();
                ExtractConnections();
              }
            }
        }
        timeTaken = clock() - before;
      }
    }
    else
    {
      SimulateParticles();
    }
    RebuildMesh();
  }
}
