#include "MDSParticle.h"
#include <math.h>

void MDSParticle::TimeStep(int64_t rPosX, int64_t rPosY, int64_t rPosZ)
{
  // Step
  int64_t x = 256ll * rPosX + ((int64_t)xPos) + xNew + xMom;
  int64_t y = 256ll * rPosY + ((int64_t)yPos) + yNew + yMom;
  int64_t z = 256ll * rPosZ + ((int64_t)zPos) + zNew + zMom;

  // Gravity
  if (mass > 0)
    yMom -= gravity;

  // Hit ground
  if (y < 0)
  {
    y = 0;
    yMom = 0;
    xMom *= 0.9;
    zMom *= 0.9;
  }

  // Write back
  xPos = x - rPosX * 256;
  yPos = y - rPosY * 256;
  zPos = z - rPosZ * 256;

  // Cleanup
  xNew = yNew = zNew = 0;
}

void MDSParticle::Interact(MDSParticle &o)
{
  if (mass == 0 && o.mass == 0) return;

  int16_t xDir = xPos - o.xPos;
  int16_t yDir = yPos - o.yPos;
  int16_t zDir = zPos - o.zPos;
  int64_t dist = sqrt((xDir * xDir) + (yDir * yDir) + (zDir * zDir));

  if (dist > fixedOne + bounceRange) return;

  int64_t multiplier;

  if (dist < fixedOne + bondRange)
    multiplier = int64_t(fixedOne - dist) * (int64_t(strength)) >> 8; // bond
  else
    multiplier = (int64_t((fixedOne + bounceRange) - dist) * (int64_t(strength) * int64_t(bounce) >> 8) >> 8); // bounce

  if (multiplier == 0.f) return;

  int64_t xoff = int64_t(xDir) * multiplier >> 8;
  int64_t yoff = int64_t(yDir) * multiplier >> 8;
  int64_t zoff = int64_t(zDir) * multiplier >> 8;

  int64_t xMomOff = xoff * momentum >> 8;
  int64_t yMomOff = yoff * momentum >> 8;
  int64_t zMomOff = zoff * momentum >> 8;

  if (mass > 0)
  {
    xNew += xoff;
    yNew += yoff;
    zNew += zoff;
    xMom += xMomOff;
    yMom += yMomOff;
    zMom += zMomOff;
  }

  if (o.mass > 0)
  {
    o.xNew -= xoff;
    o.yNew -= yoff;
    o.zNew -= zoff;
    o.xMom -= xMomOff;
    o.yMom -= yMomOff;
    o.zMom -= zMomOff;
  }

}


void MDSOldParticle::TimeStep(int64_t rPosX, int64_t rPosY, int64_t rPosZ)
{
  // Step
  float x = xPos + xNew + xMom;
  float y = yPos + yNew + yMom;
  float z = zPos + zNew + zMom;

  // Gravity
  if (mass > 0)
    yMom -= s_gravity;

  // Hit ground
  if (y < 0)
  {
    y = 0;
    yMom = 0;
    xMom *= 0.9;
    zMom *= 0.9;
  }

  // Write back
  xPos = x;
  yPos = y;
  zPos = z;

  // Cleanup
  xNew = 0;
  yNew = 0;
  zNew = 0;
}

void MDSOldParticle::Interact(MDSOldParticle &o)
{
  if (mass == 0 && o.mass == 0) return;

  float xDir = xPos - o.xPos;
  float yDir = yPos - o.yPos;
  float zDir = zPos - o.zPos;
  float dist = sqrt((xDir * xDir) + (yDir * yDir) + (zDir * zDir));

  if (dist > s_bounceRange + 1.0)
    return;

  float multiplier;
  if (dist < s_bondRange + 1.0)
    multiplier = (1 - dist) * s_strength; // bond
  else
    multiplier = (((1 + s_bounceRange) - dist) * ((s_strength) * (s_bounce))); // bounce

  if (!multiplier)
    return;

  float xoff = xDir * multiplier;
  float yoff = yDir * multiplier;
  float zoff = zDir * multiplier;
  float xMomOff = xoff * s_momentum;
  float yMomOff = yoff * s_momentum;
  float zMomOff = zoff * s_momentum;

  if (mass > 0)
  {
    xPos += xoff;
    yPos += yoff;
    zPos += zoff;
    xMom += xMomOff;
    yMom += yMomOff;
    zMom += zMomOff;
  }

  if (o.mass > 0)
  {
    o.xPos -= xoff;
    o.yPos -= yoff;
    o.zPos -= zoff;
    o.xMom -= xMomOff;
    o.yMom -= yMomOff;
    o.zMom -= zMomOff;
  }
}
