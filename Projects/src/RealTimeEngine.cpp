#include "RealTimeEngine.h"
#include "SDL_timer.h"

RealTimeEngine::RealTimeEngine(int coreSpeed /*= 100*/)
  : m_coreSpeed(coreSpeed)
  , m_lastTicks(0)
  , m_ticks(0)
{

}

void RealTimeEngine::Start()
{
  m_ticks = clock();
  m_lastTicks = m_ticks;
}

void RealTimeEngine::Stop()
{
  m_running = false;
}

int RealTimeEngine::Steps()
{
  if (m_running)
  {
    m_ticks = SDL_GetTicks();;
    int steps = (m_ticks - m_lastTicks) / (1000 / m_coreSpeed);
    m_lastTicks += (m_ticks - m_lastTicks) / (1000 / m_coreSpeed) * (1000 / m_coreSpeed);
    return steps;
  }
  return 0;
}
