#include "MDSTask.h"

MDSTask::MDSTask()
{

}

MDSTask::MDSTask(int64_t a_regionx, int64_t a_regiony, int64_t a_regionz, int64_t a_regionMomX, int64_t a_regionMomY, int64_t a_regionMomZ)
: m_region(a_regionx, a_regiony, a_regionz, a_regionMomX, a_regionMomY, a_regionMomZ)
{
}

void MDSTask::Update()
{
  // Take new points
  //m_lock.lock();
  for (auto & particle : m_newParticles)
    m_region.AddPoint(particle);
  m_newParticles.clear();
  m_region.Update();
  //m_lock.unlock();
}

void MDSTask::Process()
{
  // Process Point Interactions
  //m_lock.lock();
  m_region.Process();
  //m_lock.unlock();
}

void MDSTask::GetExitingParticles(std::vector<MDSOldParticle> *pParticles)
{
  m_region.GetExitingParticles(pParticles);
}

void MDSTask::AddPoint(MDSOldParticle particle)
{
  //m_lock.lock();
  m_newParticles.push_back(particle);
  //m_lock.unlock();
}
