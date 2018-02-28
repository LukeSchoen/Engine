#include "ParticleSimulator.h"
#include "MagicaVoxels.h"
#include "RenderObject.h"
#include "ParticleSim.h"
#include "ImageFile.h"
#include "FrameRate.h"
#include "Controls.h"
#include "Window.h"
#include "Assets.h"
#include "Camera.h"
#include <string>

void AddCube(vec3 pos, vec3 size, std::vector<std::tuple<vec3, uint32_t>> *pPoints)
{
  for (int z = 0; z < size.z; z++) for (int y = 0; y < size.y; y++) for (int x = 0; x < size.x; x++) pPoints->emplace_back(vec3(pos.x + x, pos.y + y, pos.z + z), s_packRGB(vec3(x / (size.x - 1), y / (size.y - 1), z / (size.z - 1))));
}

void AddImage(std::string file, std::vector<std::tuple<vec3, uint32_t>> *pPoints)
{
  uint32_t w, h;
  uint32_t *pImg = ImageFile::ReadImage(file.c_str(), &w, &h);
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++)
      pPoints->emplace_back(vec3(x, 0, y), pImg[x + y * w]);
  delete[] pImg;
}

void SpawnParticles(MolecularDynamicsSimulator *pCreation)
{
  pCreation->Clear();
  std::vector<std::tuple<vec3, uint32_t>> points;

  AddCube(vec3(0, 0, 0), vec3(10, 10, 10), &points);
  //AddImage(ASSETDIR "Substance/field.png", &points);
  //points = ReadVoxFile("C:/Luke/Programming/Engine/Assets/Substance/model/drop2.vox");
  //AddCube(vec3(11, 5, 10), vec3(150, 3, 3), &points);
  //AddCube(vec3(7, 10, 10), vec3(5, 5, 5), &points);
  //AddCube(vec3(11, 15, 10), vec3(5, 5, 5), &points);
  for (auto & point : points) pCreation->AddPoint(point);
  pCreation->Update();
}

void ParticleSim()
{
  Window window("Particle Sim", true, 900, 600, false);
  ParticleSimulator creation;
  mat4 projectionMat; projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.02, 8000.0f);
  Controls::SetMouseLock(true);

  SpawnParticles(&creation.world);
  creation.Update();
  while (Controls::Update())
  {
    Camera::Update(20);
    mat4 MVP = projectionMat * Camera::Matrix(); MVP.Transpose();
    creation.Render(MVP);
    window.Swap();
    if (Controls::KeyDown(SDL_SCANCODE_R)) SpawnParticles(&creation.world);
    creation.Update();
    window.Clear();
    FrameRate::Update();
    SDL_Delay(14);
  }
}
