#ifndef Entities_h__
#define Entities_h__
#include <stdint.h>
#include "BlockWorld.h"

struct Entity
{
  Entity(BlockWorld *_world)
  {
    world = _world;
  }

  bool Smash()
  {
    vec3i hitPos;
    if (world->RayTrace(position, direction, range, &hitPos))
    {
      world->SetBlock(hitPos, 0);
      return true;
    }
    else
      return false;
  }

  bool Place(uint8_t blockID)
  {
    vec3i lastAir;
    if (world->RayTrace(position, direction, range, nullptr, &lastAir))
    {
      world->SetBlock(lastAir, blockID);
      return true;
    }
    else
      return false;
  }

  vec3 position;
  vec3 direction;
  float range = 10.0f;
  BlockWorld *world;

};


#endif // Entities_h__
