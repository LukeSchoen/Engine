#include "MDSParticle.h"
#include <math.h>

void MDSParticle::TimeStep(int64_t rPosX, int64_t rPosY, int64_t rPosZ)
{
  // Step
  int64_t x = rPosX * 256 + xPos + xNew + xMom;
  int64_t y = rPosY * 256 + yPos + yNew + yMom;
  int64_t z = rPosZ * 256 + zPos + zNew + zMom;

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
  int16_t xDir = o.xPos - xPos;
  int16_t yDir = o.yPos - yPos;
  int16_t zDir = o.zPos - zPos;
  int64_t dist = sqrt( (xDir * xDir) + (yDir * yDir) + (zDir * zDir));

  if (dist < fixedOne + bounceRange)
  {
    int64_t multiplier;
    if (dist < fixedOne + bondRange)
      multiplier = int64_t(dist - fixedOne) * (int64_t(strength)) >> 8; // bond
    else
      multiplier = 0 - (int64_t((fixedOne + bounceRange) - dist) * (int64_t(strength) * int64_t(bounce) >> 8) >> 8); // bounce
    if (multiplier)
    {
      int64_t xoff = int64_t(xDir) * multiplier >> 8;
      int64_t yoff = int64_t(yDir) * multiplier >> 8;
      int64_t zoff = int64_t(zDir) * multiplier >> 8;
      int64_t xMomOff = xoff * momentum >> 8;
      int64_t yMomOff = yoff * momentum >> 8;
      int64_t zMomOff = zoff * momentum >> 8;
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
  xNew = yNew = zNew = 0;
}

void MDSOldParticle::Interact(MDSOldParticle &o)
{
  if (mass == 0 && o.mass == 0) return;
  float xDir = o.xPos - xPos;
  float yDir = o.yPos - yPos;
  float zDir = o.zPos - zPos;
  float dist = sqrt((xDir * xDir) + (yDir * yDir) + (zDir * zDir));

  if (dist < s_bounceRange + 1.0)
  {
    float multiplier;
    if (dist < s_bondRange + 1.0)
      multiplier = int64_t(dist - 1) * (int64_t(s_strength)); // bond
    else
      multiplier = 0 - (int64_t((1 + s_bounceRange) - dist) * (int64_t(s_strength) * int64_t(s_bounce))); // bounce
    if (multiplier)
    {
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
  }
}
