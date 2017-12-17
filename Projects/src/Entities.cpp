#include "Entities.h"
#include "Assets.h"
#include "PI.h"
#include "PolyModel.h"
#include "BlockWorld.h"
#include "Camera.h"
#include "Biped.h"

Entity::Entity(BlockWorld *_world)
{
  world = _world;
}

void Entity::Move(vec3 force)
{
  momentum += force;
}

bool Entity::CollidesAt(vec3 pos)
{
  return world->BoxCollides(pos - center, dimensions);
}

bool Entity::InWater(vec3 pos)
{
  return world->BoxContains(pos - center, dimensions, 8) || world->BoxContains(pos - center, dimensions, 9);
}

bool Entity::OnFloor()
{
  return CollidesAt(position + vec3(0, momentum.y - 0.02, 0));
}

bool Entity::Smash()
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

bool Entity::Place(uint8_t blockID)
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

void Entity::Update()
{
  if (Behavior)
    Behavior(this); // Custom behavior will call physics step if so required
  else
    ApplyPhysics();
}

void Entity::Draw(mat4 VP)
{
  if (visible) Biped::Draw(VP, position, direction, textureID, false, sin(clock()*0.008) * 1);
}

vec3 Entity::ResolveMomentum(vec3 pos, vec3 momentum, float stepSize)
{
  vec3 dir = momentum.Normalized();
  float dist = momentum.Length();
  int steps = dist / stepSize;
  for (int s = 0; s < steps; s++)
    if (CollidesAt(pos += dir * stepSize))
      return pos - dir * stepSize;
  return pos;
}

void Entity::ApplyPhysics()
{

  // Apply gravity
  momentum.y = OnFloor() ? 0 : momentum.y + -0.01f;

  // Enact movement
  position = CollidesAt(position + vec3(0, momentum.y, 0)) ? ResolveMomentum(position, vec3(0, momentum.y, 0), 0.01) : position + vec3(0, momentum.y, 0);

  vec3 opos = position;
  position = CollidesAt(position + vec3(momentum.x, 0, 0)) ? ResolveMomentum(position, vec3(momentum.x, 0, 0), 0.01) : position + vec3(momentum.x, 0, 0);
  if (crouching && !OnFloor()) position = opos;
  opos = position;
  position = CollidesAt(position + vec3(0, 0, momentum.z)) ? ResolveMomentum(position, vec3(0, 0, momentum.z), 0.01) : position + vec3(0, 0, momentum.z);
  if (crouching && !OnFloor()) position = opos;

  // Apply friction
  vec3 newMomentum = momentum * (OnFloor() ? 0.8f : 0.9f);
  newMomentum.y = momentum.y;
  if (InWater(position)) newMomentum = newMomentum * 0.9;
  momentum = newMomentum;


  // Apply Anti-Stuck
  if (CollidesAt(position)) position += vec3(0, 0.1, 0);
}
