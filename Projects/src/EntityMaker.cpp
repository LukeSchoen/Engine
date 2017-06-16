#include "EntityMaker.h"
#include "Textures.h"
#include "Controls.h"
#include "Camera.h"
#include "Assets.h"

static void _PlayerUpdate(Entity *entity)
{
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




struct Agent
{
  
};


Entity EntityMaker::CreateAgentNear(BlockWorld *world, vec3 pos, const char *entityTextureFile)
{
  uint32_t entTexture = _LoadTex(entityTextureFile);
  Entity entity(world);
  entity.textureID = entTexture;
  entity.position = pos + vec3(rand() % 16 - 8, 0, rand() % 16 - 8);
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

Entity EntityMaker::CreateOrcWarrior(BlockWorld *world, vec3 pos /*= vec3()*/)
{
  static unsigned int entTexture = glUndefined; if (entTexture == glUndefined) entTexture = _LoadTex(ASSETDIR "Minecraft/Mobs/textures/orc1.png");
  Entity entity(world);
  entity.textureID = entTexture;
  entity.position = pos;
  return entity;
}

Entity EntityMaker::CreateOrcBerzerker(BlockWorld *world, vec3 pos /*= vec3()*/)
{
  static unsigned int entTexture = glUndefined; if (entTexture == glUndefined) entTexture = _LoadTex(ASSETDIR "Minecraft/Mobs/textures/orc2.png");
  Entity entity(world);
  entity.textureID = entTexture;
  entity.position = pos;
  return entity;
}

unsigned int EntityMaker::_LoadTex(const char *file)
{
  return Textures::LoadTextureWithScaleup(file, 1);
}
