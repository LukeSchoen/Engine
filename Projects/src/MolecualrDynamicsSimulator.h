#ifndef MolecualrDynamicsSimulator_h__
#define MolecualrDynamicsSimulator_h__

#include "maths.h"
#include "MDSTask.h"
#include "stdint.h"
#include <mutex>
#include <vector>
#include <tuple>

class MolecularDynamicsSimulator
{
public:
  MolecularDynamicsSimulator();
  ~MolecularDynamicsSimulator();
  void AddPoint(std::tuple<vec3, uint32_t>);
  std::vector<std::tuple<vec3, uint32_t>> RequestPoints();

  std::vector<MDSTask> m_tasks;
  std::vector<std::thread *> m_workers;
  std::vector<int> m_activeTaskIDs;
  
  bool m_running = true;

  void Clear();

  void MDSwork(MDSTask *pTast);

  void Process();

  void Update();
};

#endif // MolecualrDynamicsSimulator_h__