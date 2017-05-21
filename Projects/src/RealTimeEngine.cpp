#include "RealTimeEngine.h"
#include "SDL_timer.h"
#include <math.h>

RealTimeEngine::RealTimeEngine(int coreSpeed /*= 100*/)
  : m_coreSpeed(coreSpeed)
  , m_lastTicks(0)
{

}

void RealTimeEngine::Start()
{
  m_lastTicks = clock();
}

void RealTimeEngine::Stop()
{
  m_running = false;
}

int RealTimeEngine::Steps()
{
  if (m_running)
  {
    float stepSize = (1000.0f / m_coreSpeed);
    int steps = floor((clock() - m_lastTicks) / stepSize);
    m_lastTicks += steps * stepSize;
    return steps;
  }
  return 0;
}
