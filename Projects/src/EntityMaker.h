#ifndef EntityMaker_h__
#define EntityMaker_h__

#include "Entities.h"

struct EntityMaker
{
  static Entity CreatePlayer(BlockWorld *world, vec3 pos = vec3());

  static Entity CreateOrcWarrior(BlockWorld *world, vec3 pos = vec3());

  static Entity CreateOrcBerzerker(BlockWorld *world, vec3 pos = vec3());

private:
  static unsigned int _LoadTex(char *file);
};

#endif // EntityMaker_h__
