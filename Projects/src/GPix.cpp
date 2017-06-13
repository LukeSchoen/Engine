#include "GPix.h"

extern GrassWorld grassWorld;

ActionType GPix::Brain::Think(const GPix& gPix)
{
  static int32_t mainvotes[ActionNum];
  memset(mainvotes, 0, sizeof(mainvotes));
  for (auto &thought : thoughts)
    mainvotes[thought.Think(gPix)]++;
  ActionType bestMainAction = Detonate;
  int32_t bestMainActionValue = 0;
  for (uint8_t mvID = 0; mvID < ActionNum; mvID++)
    if (mainvotes[mvID] > bestMainAction)
    {
      mainvotes[mvID] = bestMainAction;
      bestMainAction = (ActionType)mvID;
    }
  return bestMainAction;
}

ActionType GPix::Brain::Thought::Think(const GPix& gPix)
{
  static int32_t votes[ActionNum];
  memset(votes, 0, sizeof(votes));
  for (auto &idea : ideas)
  {
    bool conditionMet = true;
    for (auto &condition : idea.conditions)
      conditionMet &= gPix.ConditionMet(condition);
    if (conditionMet)
      votes[idea.action]++;
  }
  ActionType bestAction = Detonate;
  int32_t bestActionValue = 0;
  for (uint8_t vID = 0; vID < ActionNum; vID++)
    if (votes[vID] > bestActionValue)
    {
      votes[vID] = bestActionValue;
      bestAction = (ActionType)vID;
    }
  return bestAction;
}

bool GPix::isFree(const GPix &o)
{
  return o.useable && o.energy == 0;
}

bool GPix::isKin(const GPix &o) const
{
  return o.useable && o.red == red && o.green == green && o.blue == blue;
}

bool GPix::ConditionMet(const ConditionType &condition) const
{
  switch (condition)
  {
  case HealthLow: return energy < 64;
  case HealthMid: return energy >= 64 && energy < 192;
  case HealthHigh: return energy >= 192;
  case FreeLeft: return isFree(GetGPixOffset(-1, 0));
  case FreeRight: return isFree(GetGPixOffset(1, 0));
  case FreeUp: return isFree(GetGPixOffset(0, -1));
  case FreeDown: return isFree(GetGPixOffset(0, 1));
  case NonFreeLeft: return !isFree(GetGPixOffset(-1, 0));
  case NonFreeRight: return !isFree(GetGPixOffset(1, 0));
  case NonFreeUp: return !isFree(GetGPixOffset(0, -1));
  case NonFreeDown: return !isFree(GetGPixOffset(0, 1));
  case KinLeft: return isKin(GetGPixOffset(-1, 0));
  case KinRight: return isKin(GetGPixOffset(1, 0));
  case KinUp: return isKin(GetGPixOffset(0,  -1));
  case KinDown: return isKin(GetGPixOffset(0, 1));
  case NonKinLeft: return !isKin(GetGPixOffset(-1, 0));
  case NonKinRight: return !isKin(GetGPixOffset(1, 0));
  case NonKinUp: return !isKin(GetGPixOffset(0, -1));
  case NonKinDown: return !isKin(GetGPixOffset(0, 1));
  }
  return true;
}

GPix &GetGPix(uint16_t x, uint16_t y)
{
  if (x < 0 || y < 0 || x >= grassWorld.gridWidth || y >= grassWorld.gridHeight)
    return grassWorld.emptyGPix;
  return grassWorld.grid[x + y * grassWorld.gridWidth];
}

GPix &GetGPixOffset(int16_t x, int16_t y)
{
  return GetGPix(grassWorld.xpos + x, grassWorld.ypos + y);
}