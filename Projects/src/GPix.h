#include <vector>

enum ActionType : uint8_t
{
  Detonate,
  Idle,
  MoveLeft,
  MoveRight,
  MoveUp,
  MoveDown,
  GiveLeft,
  GiveRight,
  GiveUp,
  GiveDown,
  TakeLeft,
  TakeRight,
  TakeUp,
  TakeDown,
  ActionNum,
};

enum ConditionType : uint8_t
{
  HealthLow,
  HealthMid,
  HealthHigh,
  FreeLeft,
  FreeRight,
  FreeUp,
  FreeDown,
  NonFreeLeft,
  NonFreeRight,
  NonFreeUp,
  NonFreeDown,
  KinLeft,
  KinRight,
  KinUp,
  KinDown,
  NonKinLeft,
  NonKinRight,
  NonKinUp,
  NonKinDown,
  ConditionNum,
};

struct GPix
{
  bool useable = false;
  uint8_t energy = 0;
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;

  static bool isFree(const GPix &o);

  bool isKin(const GPix &o) const;

  bool ConditionMet(const ConditionType &condition) const;

  struct Brain
  {
    struct Thought
    {
      struct Ideas
      {
        std::vector<ConditionType> conditions;
        ActionType action;
      };

      ActionType Think(const GPix& gPix);

      std::vector<Ideas> ideas;
    };

    ActionType Think(const GPix& gPix);

    std::vector<Thought> thoughts;
  };

  Brain brain;
};

struct GrassWorld
{
  GPix emptyGPix;
  GPix *grid = &emptyGPix;
  uint32_t gridWidth = 1;
  uint32_t gridHeight = 1;
  uint16_t xpos = 0;
  uint16_t ypos = 0;
};

GPix &GetGPix(uint16_t x, uint16_t y);

GPix &GetGPixOffset(int16_t x, int16_t y);
