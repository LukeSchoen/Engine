#include "MolecualrDynamicsSimulator.h"
#include "SDL_timer.h"
#include <vector>
#include <tuple>
#include <array>

MolecularDynamicsSimulator::~MolecularDynamicsSimulator()
{
  m_running = false;
  for (auto & worker : m_workers)
    worker->join();
}

void MolecularDynamicsSimulator::AddPoint(std::tuple<vec3, uint32_t> point)
{
  auto & pos = std::get<0>(point);
  auto & col = std::get<1>(point);
  for (auto & task : m_tasks)
  {
    auto &region = task.m_region;
    if (pos.x >= region.xPos && pos.x < region.xPos + 256 && pos.y >= region.yPos && pos.y < region.yPos + 256 && pos.z >= region.zPos && pos.z < region.zPos + 256)
    {
      MDSOldParticle newParticle;
      newParticle.mass = 10;
      newParticle.xPos = (pos.x);// - region.xPos) * 256.f;
      newParticle.yPos = (pos.y);// - region.yPos) * 256.f;
      newParticle.zPos = (pos.z);// - region.zPos) * 256.f;
      newParticle.color = col;
      task.AddPoint(newParticle);
      return;
    }
  }

  // no region found, create a new region for this point
  int64_t regionX = pos.x - 128;
  int64_t regionY = pos.y - 128;
  int64_t regionZ = pos.z - 128;

  // move away from other tasks
  while (true)
  {
    bool intersect;
    intersect = false;
    int ox, oy, oz;
    for (auto & oTask : m_tasks)
    {
      if ((regionX + MDSRegion::regionWidth <= oTask.m_region.xPos) ||
        (regionY + MDSRegion::regionHeight <= oTask.m_region.yPos) ||
        (regionZ + MDSRegion::regionDepth <= oTask.m_region.zPos) ||
        (regionX >= oTask.m_region.xPos + MDSRegion::regionWidth) ||
        (regionY >= oTask.m_region.yPos + MDSRegion::regionHeight) ||
        (regionZ >= oTask.m_region.zPos + MDSRegion::regionDepth))
        continue;
      intersect = true;
      ox = oTask.m_region.xPos;
      oy = oTask.m_region.yPos;
      oz = oTask.m_region.zPos;
      break;
    }
    // if colliding
    // move away
    if (intersect)
    {
      int distx = abs(ox - regionX);
      int disty = abs(oy - regionY);
      int distz = abs(oz - regionZ);
      if (distx >= disty && distx >= distz) regionX += (regionX - ox > 0) ? 1 : -1;
      if (disty >= distx && disty >= distz) regionY += (regionY - oy > 0) ? 1 : -1;
      if (distz >= distx && distz >= disty) regionZ += (regionZ - oz > 0) ? 1 : -1;
    }

    // unless would lose seed particle, then stop and use original pos
    //task.m_region.xPos oTask.m_region.xPos;
    if (!intersect)
      break;
  }

  MDSTask task((int64_t)regionX, (int64_t)regionY, (int64_t)regionZ, (int64_t)0, (int64_t)0, (int64_t)0);

  m_tasks.push_back(task);
  return AddPoint(point);
}

std::vector<std::tuple<vec3, uint32_t>> MolecularDynamicsSimulator::RequestPoints()
{
  static std::vector<std::tuple<vec3, uint32_t>> ret;
  ret.clear();
  for (auto & task : m_tasks)
    for (auto & particle : task.m_region.particles)
      ret.emplace_back(vec3(particle.xPos, particle.yPos, particle.zPos), particle.color);
      //ret.emplace_back(vec3((particle.xPos / 255.0f) + task.m_region.xPos, (particle.yPos / 255.0f) + task.m_region.yPos, (particle.zPos / 255.0f) + task.m_region.zPos), particle.color);
  //ret.emplace_back(vec3(float(particle.xPos) / 256 + task.m_region.xPos, float(particle.yPos) / 256 + task.m_region.yPos, float(particle.zPos) / 256 + task.m_region.zPos), 255);
  return ret;
}

void MolecularDynamicsSimulator::Clear()
{
  m_tasks.clear();
  m_workers.clear();
  m_activeTaskIDs.clear();
}

void MolecularDynamicsSimulator::MDSwork(MDSTask *pTast)
{
}

void MolecularDynamicsSimulator::Process()
{
  for (auto & task : m_tasks)
  {
    std::vector<MDSOldParticle> transferParticles;
    std::vector<std::tuple<vec3, uint32_t>> tParticles;
    task.GetExitingParticles(&transferParticles);
    for (auto & particle : transferParticles)
      tParticles.emplace_back(vec3(particle.xPos, particle.yPos, particle.zPos), particle.color);
    for (auto & tparticle : tParticles)
      AddPoint(tparticle);
  }
  for (auto & task : m_tasks) task.Update();


  for (auto & task : m_tasks) task.Process();
}

void MolecularDynamicsSimulator::Update()
{
  for (auto & task : m_tasks) task.Update();
}

MolecularDynamicsSimulator::MolecularDynamicsSimulator()
{

}
