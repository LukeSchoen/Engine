#include "ParticleSimulator.h"
#include "Assets.h"
#include <vector>
#include "GLtypes.h"

ParticleSimulator::ParticleSimulator()
{
  glEnable(GL_PROGRAM_POINT_SIZE);
  mesh.AssignShader(ASSETDIR "Substance/shaders/Substance.vert", ASSETDIR "Substance/shaders/Substance.frag");
  debugLineMesh.AssignShader(ASSETDIR "Substance/shaders/Substance.vert", ASSETDIR "Substance/shaders/Substance.frag");
}

void DrawRegionLines(std::vector<vec3> *posData, std::vector<vec3> *colData, vec3 pos)
{
  vec3 p = vec3(MDSRegion::regionWidth, MDSRegion::regionHeight, MDSRegion::regionDepth);
  posData->push_back(pos); posData->push_back(pos + vec3(p.x, 0, 0));
  posData->push_back(pos + vec3(0, p.y, 0)); posData->push_back(pos + vec3(p.x, p.y, 0));
  posData->push_back(pos + vec3(0, 0, p.z)); posData->push_back(pos + vec3(p.x, 0, p.z));
  posData->push_back(pos + vec3(0, p.y, p.z)); posData->push_back(pos + vec3(p.x, p.y, p.z));

  posData->push_back(pos); posData->push_back(pos + vec3(0, 0, p.z));
  posData->push_back(pos + vec3(p.x, 0, 0)); posData->push_back(pos + vec3(p.x, 0, p.z));
  posData->push_back(pos + vec3(0, p.y, 0)); posData->push_back(pos + vec3(0, p.y, p.z));
  posData->push_back(pos + vec3(p.x, p.y, 0)); posData->push_back(pos + vec3(p.x, p.y, p.z));

  posData->push_back(pos); posData->push_back(pos + vec3(0, p.y, 0));
  posData->push_back(pos + vec3(p.x, 0, 0)); posData->push_back(pos + vec3(p.x, p.y, 0));
  posData->push_back(pos + vec3(0, 0, p.y)); posData->push_back(pos + vec3(0, p.y, p.z));
  posData->push_back(pos + vec3(p.x, 0, p.y)); posData->push_back(pos + vec3(p.x, p.y, p.z));

  for (int i = 0; i < 2 * 12; i++) colData->push_back(vec3(0, 1, 0));
}

void ParticleSimulator::Update(int steps /*= 1*/)
{
  for (int i = 0; i < steps; i++) world.Process();
  static std::vector<vec3> posData; posData.clear();
  static std::vector<vec3> colData; colData.clear();
  for (auto & p : world.RequestPoints())
  {
    posData.push_back(std::get<0>(p));
    colData.push_back(s_unpackRGB(std::get<1>(p)));
  }
  mesh.AssignAttribute("position", GLAttributeType::AT_FLOAT, posData.data(), 3, posData.size());
  mesh.AssignAttribute("color", GLAttributeType::AT_FLOAT, colData.data(), 3, colData.size());
  posData.clear();
  colData.clear();
  for (auto & task : world.m_tasks) DrawRegionLines(&posData, &colData, vec3(task.m_region.xPos, task.m_region.yPos, task.m_region.zPos));
  debugLineMesh.AssignAttribute("position", GLAttributeType::AT_FLOAT, posData.data(), 3, posData.size());
  debugLineMesh.AssignAttribute("color", GLAttributeType::AT_FLOAT, colData.data(), 3, colData.size());
}

void ParticleSimulator::Render(const mat4 & MVP)
{
  float pointSize = 0.8;
  mesh.AssignUniform("size", UT_1f, &pointSize);
  mesh.RenderPoints(MVP);
  debugLineMesh.RenderLines(MVP);
}

void ParticleSimulator::AddPoint(vec3 pos, vec3 col)
{
  world.AddPoint({ pos, s_packRGB(col) });
}
