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
    if (pos.x >= region.xPos && pos.x < region.xPos + 255 && pos.y >= region.yPos && pos.y < region.yPos + 255 && pos.z >= region.zPos && pos.z < region.zPos + 255)
    {
      MDSOldParticle newParticle;
      newParticle.mass = 10;
      newParticle.xPos = (pos.x - region.xPos) * 256.f;
      newParticle.yPos = (pos.y - region.yPos) * 256.f;
      newParticle.zPos = (pos.z - region.zPos) * 256.f;
      newParticle.color = col;
      task.AddPoint(newParticle);
      return;
    }
  }

  // no region found, create a new region for this point
  int64_t regionX = pos.x - 128;
  int64_t regionY = pos.y - 128;
  int64_t regionZ = pos.z - 128;
  MDSTask task((int64_t)regionX, (int64_t)regionY, (int64_t)regionZ, (int64_t)0, (int64_t)0, (int64_t)0);
  m_tasks.push_back(task);
  return AddPoint(point);
}

std::vector<std::tuple<vec3, uint32_t>> MolecularDynamicsSimulator::RequestPoints()
{
  std::vector<std::tuple<vec3, uint32_t>> ret;
  for (auto & task : m_tasks)
    for (auto & particle : task.m_region.particles)
      ret.emplace_back(vec3(float(particle.xPos) / 256 + task.m_region.xPos, float(particle.yPos) / 256 + task.m_region.yPos, float(particle.zPos) / 256 + task.m_region.zPos), 255);
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
//   while (m_running)
//   {
//     pTast->region.particles;
//     SDL_Delay(1);
//   }
}

void MolecularDynamicsSimulator::Update()
{
  for (auto & task : m_tasks)
    task.Process();
}

MolecularDynamicsSimulator::MolecularDynamicsSimulator()
{
  //m_tasks.emplace_back();
  // Setup Threads
  //int64_t threadCount = std::thread::hardware_concurrency();
  //m_tasks.resize(threadCount);
}
