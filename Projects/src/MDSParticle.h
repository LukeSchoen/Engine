 #ifndef MDSParticle_h__
#define MDSParticle_h__

#include "stdint.h"

const static float s_gravity = 0.00005f;
const static float s_bounce = 0.5;
const static float s_strength = 0.5;
const static float s_momentum = 0.5;
const static float s_bondRange = 0.2;
const static float s_bounceRange = 0.41;

struct MDSOldParticle
{
  uint32_t color = 0;
  uint8_t mass = 0;
  float xPos = 0, yPos = 0, zPos = 0;
  float xNew = 0, yNew = 0, zNew = 0;
  float xMom = 0, yMom = 0, zMom = 0;

  void TimeStep(int64_t rPosX, int64_t rPosY, int64_t rPosZ);
  void Interact(MDSOldParticle &o);
};

struct MDSParticle
{
  uint32_t color = 0;
  uint8_t mass = 0;
  uint16_t xPos = 0, yPos = 0, zPos = 0;
  int16_t xNew = 0, yNew = 0, zNew = 0;
  int16_t xMom = 0, yMom = 0, zMom = 0;

  const static int16_t fixedOne = 256;
  const static int16_t gravity = 10;
  const static int16_t bounce = 0.5 * fixedOne;
  const static int16_t strength = 0.5 * fixedOne;
  const static int16_t momentum = 0.5 * fixedOne;
  const static int16_t bondRange = 0.2 * fixedOne;
  const static int16_t bounceRange = 0.41 * fixedOne;

  void TimeStep(int64_t rPosX, int64_t rPosY, int64_t rPosZ);
  void Interact(MDSParticle &o);
};

#endif // MDSParticle_h__
