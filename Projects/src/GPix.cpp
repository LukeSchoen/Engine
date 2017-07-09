#include "GPix.h"
#include <basetsd.h>

int RandBuff[65536];

extern GrassWorld grassWorld;

int FastRand()
{
  static uint16_t r = 0;
  return RandBuff[r++];
}

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
  if (!o.useable)
    return false;

  int error = abs(o.red - red) + abs(o.green - green) + abs(o.blue - blue);
  return error / 5;
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

const GPix &GetGPix(uint16_t x, uint16_t y)
{
  if (x < 0 || y < 0 || x >= grassWorld.gridWidth || y >= grassWorld.gridHeight)
    return grassWorld.emptyGPix;
  return grassWorld.grid[x + y * grassWorld.gridWidth];
}

GPix &SetNewGPix(uint16_t x, uint16_t y)
{
  if (x < 0 || y < 0 || x >= grassWorld.gridWidth || y >= grassWorld.gridHeight)
    return grassWorld.emptyGPix;
  return grassWorld.newgrid[x + y * grassWorld.gridWidth];
}

const GPix &GetGPixOffset(int16_t x, int16_t y)
{
  return GetGPix(grassWorld.xpos + x, grassWorld.ypos + y);
}

GPix &SetNewGPixOffset(int16_t x, int16_t y)
{
  return SetNewGPix(grassWorld.xpos + x, grassWorld.ypos + y);
}

void MutateAgent(GPix *agent)
{
  int64_t colorMutationRate = 5;
  int64_t brainMutationRate = 5;

  // mutate color
  agent->red = agent->red += (rand() % 3 - 1) * (rand() % colorMutationRate);
  agent->green = agent->green += (rand() % 3 - 1) * (rand() % colorMutationRate);
  agent->blue = agent->blue += (rand() % 3 - 1) * (rand() % colorMutationRate);

  float mutationSpeed = 1 - (((float)rand() / RAND_MAX * 128) * ((float)rand() / RAND_MAX * 128) * ((float)rand() / RAND_MAX * 128));

  // mutate brain
  for (int64_t i = 0; i < brainMutationRate; i++)
    for (auto & thought : agent->brain.thoughts)
      for (auto & idea : thought.ideas)
        for (auto & condition : idea.conditions)
        {
          if ((FastRand() % brainMutationRate / mutationSpeed) < 1.0f)
            condition = ConditionType(FastRand() % ConditionNum);
          if ((FastRand() % brainMutationRate * idea.conditions.size() / mutationSpeed) < 1.0f)
            idea.action = ActionType(FastRand() % ActionNum);
        }
        std::vector<ConditionType> conditions;
        ActionType action;
}

void GrassWorld::RandomizeBuffer()
{
  for (uint16_t i = 0; i < ((UINT16)~((UINT16)0)); i++)
  {
    RandBuff[i] = rand();
  }
  
}
