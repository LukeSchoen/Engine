#include <windows.h>
#include "Threads.h"
#include "stdint.h"

static uint64_t processMask;
static uint64_t systemMask;

void Threads::SetFastMode()
{
  SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
  SetThreadAffinity(1);
}

void Threads::SetSlowMode()
{
  SetPriorityClass(GetCurrentProcess(), THREAD_PRIORITY_NORMAL);
  SetThreadAffinityToAll();
}

void Threads::SetThreadAffinity(int core)
{
  SetProcessAffinityMask(GetCurrentProcess(), 1 << core);
}

void Threads::SetThreadAffinityToAll()
{
  GetProcessAffinityMask(GetCurrentProcess(), (PDWORD_PTR)&processMask, (PDWORD_PTR)&systemMask);
  SetProcessAffinityMask(GetCurrentProcess(), systemMask);
}
