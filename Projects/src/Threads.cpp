#include <windows.h>
#include "Threads.h"
#include "stdint.h"

static uint64_t processMask;
static uint64_t systemMask;

void Threads::SetFastMode()
{
  SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
  SetProcessAffinityMask(GetCurrentProcess(), 1);
}

void Threads::SetSlowMode()
{
  SetPriorityClass(GetCurrentProcess(), THREAD_PRIORITY_NORMAL);
  GetProcessAffinityMask(GetCurrentProcess(), (PDWORD_PTR)&processMask, (PDWORD_PTR)&systemMask);
  SetProcessAffinityMask(GetCurrentProcess(), systemMask);
}
