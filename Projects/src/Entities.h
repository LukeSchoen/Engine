#ifndef Entities_h__
#define Entities_h__
#include <stdint.h>
#include "BlockWorld.h"
#include "Assets.h"
#include "PI.h"

static struct
{
  bool loaded = false;
  PolyModel Head;
  PolyModel Torso;
  PolyModel RightArm;
  PolyModel LeftArm;
  PolyModel RightLeg;
  PolyModel LeftLeg;
  void LoadModels()
  {
    if (loaded) return;
    loaded = true;
    Head.LoadModel(ASSETDIR "Minecraft/Mobs/Biped/head.obj", true);
    Torso.LoadModel(ASSETDIR "Minecraft/Mobs/Biped/Torso.obj");
    RightArm.LoadModel(ASSETDIR "Minecraft/Mobs/Biped/ArmR.obj");
    LeftArm.LoadModel(ASSETDIR "Minecraft/Mobs/Biped/ArmL.obj");
    RightLeg.LoadModel(ASSETDIR "Minecraft/Mobs/Biped/LegR.obj");
    LeftLeg.LoadModel(ASSETDIR "Minecraft/Mobs/Biped/LegL.obj");
  }
} BipedModel;

struct Entity
{
  Entity(BlockWorld *_world)
  {
    world = _world;
    BipedModel.LoadModels();
  }

  void Draw(mat4 VP)
  {
    float armOut = 0.474;
    float armDwn = -0.135;

    float legOut = 0.118;
    float legDwn = -0.7095;

    mat4 model;
    model.Translate(position);

    vec3 capPos = vec3() - Camera::Position();
    float headYaw = -atan2(position.x - capPos.x, position.z - capPos.z);
    float headPitch = DegsToRads * 270 + atan2(vec2(position.x - capPos.x, position.z - capPos.z).Length(), position.y - capPos.y);
    model.RotateY(headYaw);
    model.RotateX(headPitch);
    mat4 headMVP = VP * model;
    model.RotateX(-headPitch);
    model.RotateY(-headYaw);
    headMVP.Transpose();
    BipedModel.Head.Render(headMVP);

    mat4 torsoMVP = VP * model;
    torsoMVP.Transpose();
    BipedModel.Torso.Render(torsoMVP);

    model.Translate(vec3(-armOut, armDwn, 0));
    mat4 armR = VP * model;
    armR.Transpose();
    BipedModel.RightArm.Render(armR);

    model.Translate(vec3(armOut * 2, 0, 0));
    mat4 armL = VP * model;
    armL.Transpose();
    BipedModel.LeftArm.Render(armL);

    model.Translate(vec3(-armOut, -armDwn, 0));
    model.Translate(vec3(-legOut, legDwn, 0));
    mat4 legR = VP * model;
    legR.Transpose();
    BipedModel.RightLeg.Render(legR);

    model.Translate(vec3(legOut * 2, 0, 0));
    mat4 legL = VP * model;
    legL.Transpose();
    BipedModel.LeftLeg.Render(legL);
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
