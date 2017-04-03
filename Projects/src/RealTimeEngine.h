#ifndef RealTimeEngine_h__
#define RealTimeEngine_h__
#include "stdint.h"
#include <time.h>

struct RealTimeEngine
{
  uint32_t m_lastTicks;
  uint32_t m_ticks;
  uint32_t m_coreSpeed;

  RealTimeEngine(int coreSpeed = 100);

  bool m_running = true;

  void Start();

  void Stop();

  int Steps();

};


#endif // RealTimeEngine_h__
