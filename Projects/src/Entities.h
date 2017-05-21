#ifndef Entities_h__
#define Entities_h__

#include "maths.h"
#include "GLtypes.h"

struct BlockWorld;

struct Entity
{
  Entity(BlockWorld *_world);

  void Move(vec3 force);

  bool CollidesAt(vec3 pos);

  bool InWater(vec3 pos);

  bool OnFloor();

  bool Smash();

  bool Place(uint8_t blockID);

  void Update();

  void ApplyPhysics();

  void Draw(mat4 VP);

  vec3 position;
  vec3 momentum;
  vec3 direction;
  vec3 dimensions = vec3(0.5, 1.95, 0.5);
  vec3 center = vec3(0, 0.75, 0);
  int64_t uniqueID = -1;
  float range = 5.0f;
  BlockWorld *world;
  bool visible = true;
  bool crouching = false;
  bool running = false;

  void (*Behavior) (Entity*) = nullptr;

  unsigned int textureID = glUndefined;

private:
  vec3 ResolveMomentum(vec3 pos, vec3 momentum, float stepSize);

};


#endif // Entities_h__
