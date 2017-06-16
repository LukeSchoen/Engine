#ifndef EntityMaker_h__
#define EntityMaker_h__

#include "Entities.h"

struct EntityMaker
{
  static Entity CreateAgentNear(BlockWorld *world, vec3 pos, const char *entityTextureFile);

  static Entity CreatePlayer(BlockWorld *world, vec3 pos = vec3());

  static Entity CreateOrcWarrior(BlockWorld *world, vec3 pos = vec3());

  static Entity CreateOrcBerzerker(BlockWorld *world, vec3 pos = vec3());

private:
  static unsigned int _LoadTex(const char *file);
};

#endif // EntityMaker_h__
