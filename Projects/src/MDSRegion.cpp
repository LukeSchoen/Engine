#include "MDSRegion.h"

MDSRegion::MDSRegion()
  : xPos(0)
  , yPos(0)
  , zPos(0)
  , xMom(0)
  , yMom(0)
  , zMom(0)
{

}

MDSRegion::MDSRegion(int64_t a_regionx, int64_t a_regiony, int64_t a_regionz, int64_t a_regionMomX, int64_t a_regionMomY, int64_t a_regionMomZ)
  : xPos(a_regionx)
  , yPos(a_regiony)
  , zPos(a_regionz)
  , xMom(a_regionMomX)
  , yMom(a_regionMomY)
  , zMom(a_regionMomZ)
{

}

void MDSRegion::Process()
{
  for (int a = 0; a < particles.size() - 1; a++)
    for (int b = a + 1; b < particles.size(); b++) // Discovery
      particles[a].Interact(particles[b]); //Interaction
  for (auto &particle : particles) particle.TimeStep(xPos, yPos, zPos); // Time step
}

void MDSRegion::AddPoint(MDSOldParticle p)
{
  particles.push_back(p);
}