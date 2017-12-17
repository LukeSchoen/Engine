#ifndef EntityMaker_h__
#define EntityMaker_h__

#include "Entities.h"

enum EntityBehaviorType
{
  EBH_Neutral,
  EBH_Friendly,
  EBH_Zombie,
  EBH_Archer,
};

struct EntityMaker
{
  static Entity CreateAgentNear(BlockWorld *world, vec3 pos, const char *entityTextureFile, EntityBehaviorType type);

  static Entity CreatePlayer(BlockWorld *world, vec3 pos = vec3());

private:
  static unsigned int _LoadTex(const char *file);
};

#endif // EntityMaker_h__
