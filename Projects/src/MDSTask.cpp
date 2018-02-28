#include "MDSTask.h"

MDSTask::MDSTask()
{

}

MDSTask::MDSTask(int64_t a_regionx, int64_t a_regiony, int64_t a_regionz, int64_t a_regionMomX, int64_t a_regionMomY, int64_t a_regionMomZ)
: m_region(a_regionx, a_regiony, a_regionz, a_regionMomX, a_regionMomY, a_regionMomZ)
{
}

void MDSTask::Process()
{
  // Take new points
  //m_lock.lock();
  for (auto & particle : m_newParticles)
    m_region.AddPoint(particle);
  m_newParticles.clear();
  //m_lock.unlock();

  // Process Point Interactions
  m_region.Process();
}

void MDSTask::AddPoint(MDSParticle particle)
{
  //m_lock.lock();
  m_newParticles.push_back(particle);
  //m_lock.unlock();
}
