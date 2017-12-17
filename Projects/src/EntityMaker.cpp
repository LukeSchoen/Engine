#include "EntityMaker.h"
#include "Textures.h"
#include "Controls.h"
#include "Camera.h"
#include "Assets.h"
#include <map>
#include <algorithm>
#include "BlockWorld.h"

vec3 playerPos;



struct Memory
{
  // I was hurt
  // I was heal
};

class Brain
{
public:
  // history given a context
  // various things happened (i was hurt / heal) last time i was in this context

  // brains job..
  // Determine context
  // Act on history

};

// i saw a tree
// i stood on sand
// i saw a chicken
// i fell in water
// i got hit
// i picked up some food
// 

// object observed at XYZ in the world
// item state changed
// health state changed

// tasks
// Collect wood to build a house

// hard creeper to beat
// attacks from behind
// jumps OVER holes
// builds upward if necessary
// digs threw blocks
// coordinates with other creepers
// Come close only to attack

// has current plan(s)
// Move towards (auto climb / jump)
// hide from 
// strike
// 

// escape
// - hide
// - run
// attack
// - sneak up
// -


static void _PlayerUpdate(Entity *entity)
{
  playerPos = entity->position;

  // Jumping
  bool jumpPressed = Controls::KeyDown(SDL_SCANCODE_SPACE) || Controls::GetControllerButton(10);
  if (jumpPressed && entity->OnFloor() && abs(entity->momentum.y) < 0.05)
    if(entity->running)
      entity->momentum.y = 0.21;
    else
      entity->momentum.y = 0.17;

  if (jumpPressed)
  {
    if (entity->InWater(entity->position)) entity->momentum.y = 0.02;
    if (entity->InWater(entity->position + vec3(0, 0.5, 0))) entity->momentum.y += 0.02;
    if (entity->InWater(entity->position + vec3(0, 1, 0))) entity->momentum.y += 0.02;
    if (entity->InWater(entity->position + vec3(0, 1.5, 0))) entity->momentum.y += 0.02;
  }

  // Gaits
  if (entity->OnFloor() || entity->InWater(entity->position))
  {
    entity->crouching = Controls::KeyDown(SDL_SCANCODE_LCTRL);
    entity->running = Controls::KeyDown(SDL_SCANCODE_LSHIFT) && (!entity->crouching);
  }

  // Walking
  entity->direction = vec3() - Camera::Direction();
  vec3 moveDir = (vec3() - Camera::Position() - entity->position);
  moveDir.y = 0;
  moveDir = moveDir.Normalized();
  float speed = 0.01;
  if (entity->OnFloor())
  {
    speed *= 2;
  }
  if (entity->crouching) speed *= 0.5;
  if (entity->running) speed *= 1.5;

  entity->Move(moveDir * speed);

  entity->ApplyPhysics();

  // Smashing !
  static bool smashDown = false;
  if (Controls::GetLeftClick() || Controls::GetControllerButton(11))
  {
    if (!smashDown)
      entity->Smash();
    smashDown = true;
  }
  else smashDown = false;

  // Placing !
  static bool placeDown = false;
  if (Controls::GetRightClick() || Controls::GetControllerButton(12))
  {
    if (!placeDown)
      entity->Place(12);
    placeDown = true;
  }
  else placeDown = false;
}

struct ZombieBrain
{

};

std::map<int64_t, ZombieBrain> brainStates;

float AngleBetween(float from, float to)
{
  if (from > to)
    return -PI + std::fmod(from - to + PI, PI * 2);
  else
    return PI - std::fmod(to - from + PI, PI * 2);
}

float LerpAngle(float a, float b, float step)
{
  return a - std::max(std::min(AngleBetween(a, b), step), -step);
}

static void _ZombieUpdate(Entity *entity)
{
  //float headPitch = DegsToRads * 270 + atan2(vec2(pos.x - capPos.x, pos.z - capPos.z).Length(), pos.y - capPos.y);
  //brainStates[entity->uniqueID];

  // look at player
  vec3 mobToPMeDir = (playerPos - entity->position).Normalized();
  float oldyaw = -atan2(-entity->direction.x, -entity->direction.z);
  float newyaw = -atan2(-mobToPMeDir.x, -mobToPMeDir.z);
  float yaw = LerpAngle(oldyaw, newyaw, 0.04);

  entity->direction.x = sin(yaw);
  entity->direction.z = -cos(yaw);
  entity->direction.y = mobToPMeDir.y;
  entity->direction = entity->direction.Normalized();

  // Move towards player
  vec3 dir = entity->direction;
  dir.y = 0;
  dir = dir * 0.02;

  if (entity->InWater(entity->position + vec3(0, 1, 0))) entity->momentum.y = 0.02;
  if (entity->InWater(entity->position + vec3(0, 2.5, 0))) entity->momentum.y += 0.02;

  // Jump over gaps
  if (entity->OnFloor() && !entity->CollidesAt(vec3(0, -0.1, 0) + entity->position + dir.Normalized()) && entity->CollidesAt(vec3(0, -0.1, 0) + entity->position + dir.Normalized() * 2))
    entity->momentum.y = 0.1;
  else if (entity->OnFloor() && !entity->CollidesAt(vec3(0, -0.1, 0) + entity->position + dir.Normalized()) && entity->CollidesAt(vec3(0, -0.1, 0) + entity->position + dir.Normalized() * 3))
    entity->momentum.y = 0.15;
  else if (entity->OnFloor() && !entity->CollidesAt(vec3(0, -0.1, 0) + entity->position + dir.Normalized()) && entity->CollidesAt(vec3(0, -0.1, 0) + entity->position + dir.Normalized() * 4))
    entity->momentum.y = 0.21;

  // Get out of water
  if (entity->CollidesAt(entity->position + dir) && entity->InWater(entity->position)) entity->momentum.y = 0.1;

  // Jump over blocks
  if (entity->CollidesAt(entity->position + dir) && entity->OnFloor())
  {
    if (!entity->CollidesAt(entity->position + dir + vec3(0, 1.1, 0)) && !entity->CollidesAt(entity->position + dir + vec3(0, 2.1, 0)))
      entity->momentum.y = 0.17;
    else if (!entity->CollidesAt(entity->position + dir + vec3(0, 2.1, 0)) && !entity->CollidesAt(entity->position + dir + vec3(0, 3.1, 0)))
      entity->momentum.y = 0.21;
    else
      if (rand() % 8 == 1)
      {
        // destroy nearest block
        float nearestDist = 10000;
        vec3 nearestDir = { 1,0,0 };
        for (int64_t r = 0; r < 256; r++)
        {
          vec3 dir = vec3(((float)rand() - RAND_MAX * 0.5) / RAND_MAX, ((float)rand() - RAND_MAX * 0.5) / RAND_MAX, ((float)rand() - RAND_MAX * 0.5) / RAND_MAX);
          vec3i c;
          if (entity->world->RayTrace(entity->position, dir, 5, &c))
          {
            float dist = (vec3(c.x, c.y, c.z) - entity->position).Length();
            if (dist < nearestDist)
            {
              nearestDist = dist;
              nearestDir = dir;
            }
          }
        }
        if (nearestDist != 10000)
        {
          entity->direction = nearestDir;
          entity->Smash();
        }
      }
  }

  // Jump randomly
  //if (entity->OnFloor() && rand() % 96 == 1)
    //entity->momentum.y = 0.21;

  if(fabs(AngleBetween(oldyaw, yaw)) < 0.02)
    entity->Move(dir);
  else
    entity->Move(mobToPMeDir * 0.01);

  entity->ApplyPhysics();
}

struct Agent
{
   
};

Entity EntityMaker::CreateAgentNear(BlockWorld *world, vec3 pos, const char *entityTextureFile, EntityBehaviorType type)
{
  uint32_t entTexture = _LoadTex(entityTextureFile);
  Entity entity(world);
  entity.textureID = entTexture;
  entity.position = pos + vec3(rand() % 16 - 8, 0, rand() % 16 - 8);
  switch (type)
  {
  case EBH_Neutral: entity.Behavior = nullptr; break;
  case EBH_Friendly: entity.Behavior = nullptr; break;
  case EBH_Zombie: entity.Behavior = _ZombieUpdate; break;
  //case EBH_Zombie: entity.Behavior = nullptr; break;
  }

  return entity;
}

Entity EntityMaker::CreatePlayer(BlockWorld *world, vec3 pos /*= vec3()*/)
{
  static unsigned int entTexture = glUndefined; if (entTexture == glUndefined) entTexture = _LoadTex(ASSETDIR "Minecraft/Mobs/textures/Steve.png");
  Entity entity(world);
  entity.textureID = entTexture;
  entity.position = pos;
  entity.Behavior = _PlayerUpdate;
  entity.visible = false;
  return entity;
}

unsigned int EntityMaker::_LoadTex(const char *file)
{
  return Textures::LoadTextureWithScaleup(file, 1);
}
