#ifndef MDSTask_h__
#define MDSTask_h__

#include <thread>
#include <mutex>
#include <vector>

#include "MDSRegion.h"
#include "MDSParticle.h"

class MDSTask
{
public:
  MDSTask();
  MDSTask(int64_t a_regionx, int64_t a_regiony, int64_t a_regionz, int64_t a_regionMomX, int64_t a_regionMomY, int64_t a_regionMomZ);

  std::vector<MDSParticle> m_newParticles;
  MDSRegion m_region;

  void Process();

  void AddPoint(MDSParticle particle);
};

#endif // MDSTask_h__
