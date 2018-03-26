#include "MDSRegion.h"
#include "Clamp.h"

static int64_t s_optimisationRound = 0;

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

void MDSRegion::Update()
{
  if (!upToDate)
  {
    Optimise();
    upToDate = true;
  }
}

void MDSRegion::Process()
{
  Update();
  std::vector<int> nearIDs;
  for (int a = 0; a < Max(((int64_t)particles.size()) - 1, 0); a++)
  {
    // Discovery
    nearIDs.clear();
    GetParticles(particles[a].xPos, particles[a].yPos, particles[a].zPos, 2, &nearIDs);
    //Interaction
    for (auto & id : nearIDs)
      particles[id].Interact(particles[a]);
  }
  for (auto &particle : particles) particle.TimeStep(xPos, yPos, zPos); // Time step
  upToDate = false;
}

void MDSRegion::Optimise()
{
  accelerationMode = 0; // 0 = N2, 1 = Xlane, 1 = Ylane, 1 = Zlane
  if (particles.size() > 64)
  {
    // Optimize region for processing
    // Calculate approximate operation times for various optimization algorithms
    const int laneSize = 2;
    if (xLanes.size() != regionWidth / laneSize)
      xLanes.resize(regionWidth / laneSize);
    if (yLanes.size() != regionHeight / laneSize)
      yLanes.resize(regionHeight / laneSize);
    if (zLanes.size() != regionDepth / laneSize)
      zLanes.resize(regionDepth / laneSize);

    for (int64_t i = 0; i < regionDepth / laneSize; i++)
    {
      xLanes[i].clear();
      yLanes[i].clear();
      zLanes[i].clear();
    }

    int N2_OPS = particles.size() * particles.size();
    for (int pID = 0; pID < particles.size(); pID++)
    {
      xLanes[int(particles[pID].xPos / laneSize)].push_back(pID);
      yLanes[int(particles[pID].yPos / laneSize)].push_back(pID);
      zLanes[int(particles[pID].zPos / laneSize)].push_back(pID);
    }

    int Xlane_N2_Ops = 0;
    int Ylane_N2_Ops = 0;
    int Zlane_N2_Ops = 0;
    for (auto & lane : xLanes) Xlane_N2_Ops += lane.size() * lane.size();
    for (auto & lane : yLanes) Ylane_N2_Ops += lane.size() * lane.size();
    for (auto & lane : zLanes) Zlane_N2_Ops += lane.size() * lane.size();

    int bestLane = Min(Xlane_N2_Ops, Min(Ylane_N2_Ops, Zlane_N2_Ops));
    if (N2_OPS > bestLane * 2) // if lanes really help then use best lane
    {
      if (Xlane_N2_Ops < Ylane_N2_Ops)
      {
        if (Xlane_N2_Ops < Zlane_N2_Ops)
          accelerationMode = 1;
        else
          accelerationMode = 3;
      }
      else
      {
        if (Ylane_N2_Ops < Zlane_N2_Ops)
          accelerationMode = 2;
        else
          accelerationMode = 3;
      }
    }
  }
  //accelerationMode = 0;
}

void MDSRegion::GetParticles(uint8_t x, uint8_t y, uint8_t z, uint8_t radius, std::vector<int> *pIds)
{
  Update();
  //accelerationMode = 0;
  switch (accelerationMode)
  {

    case 0:
    {
      for (int p = 0; p < particles.size(); p++)
        if (abs(particles[p].xPos - x) <= radius && abs(particles[p].yPos - y) <= radius && abs(particles[p].zPos - z) <= radius)
          pIds->emplace_back(p);
      break;
    }

    case 1:
    {
      int left = floor(x - radius);
      int right = ceil(x + radius);
      left = Max(left, 0);
      right = Min(right, regionWidth - 1);
      for (int ix = left; ix < right; ix++)
        for (int p = 0; p < xLanes[ix].size(); p++)
          if (abs(particles[xLanes[ix][p]].xPos - x) <= radius && abs(particles[xLanes[ix][p]].yPos - y) <= radius && abs(particles[xLanes[ix][p]].zPos - z) <= radius)
            pIds->push_back(xLanes[ix][p]);
      break;
    }

    case 2:
    {
      int bot = floor(y - radius);
      int top = ceil(y + radius);
      bot = Max(bot, 0);
      top = Min(top, regionHeight - 1);
      for (int iy = bot; iy < top; iy++)
        for (int p = 0; p < yLanes[iy].size(); p++)
          if (abs(particles[yLanes[iy][p]].xPos - x) <= radius && abs(particles[yLanes[iy][p]].yPos - y) <= radius && abs(particles[yLanes[iy][p]].zPos - z) <= radius)
            pIds->push_back(yLanes[iy][p]);
      break;
    }

    case 3:
    {
      int back = floor(z - radius);
      int front = ceil(z + radius);
      back = Max(back, 0);
      front = Min(front, regionDepth - 1);
      for (int iz = back; iz < front; iz++)
        for (int p = 0; p < zLanes[iz].size(); p++)
          if (abs(particles[zLanes[iz][p]].xPos - x) <= radius && abs(particles[zLanes[iz][p]].yPos - y) <= radius && abs(particles[zLanes[iz][p]].zPos - z) <= radius)
            pIds->push_back(zLanes[iz][p]);
      break;
    }
  }
}

void MDSRegion::AddPoint(MDSOldParticle p)
{
  particles.push_back(p);
  upToDate = false;
}

void MDSRegion::GetExitingParticles(std::vector<MDSOldParticle> *pParticles)
{
  // Let particles leave regions
  for (int pID = 0; pID < particles.size(); pID++)
  {
    if (particles[pID].xPos < xPos || particles[pID].yPos < yPos || particles[pID].zPos < zPos || particles[pID].xPos >= xPos + regionWidth || particles[pID].yPos >= yPos + regionHeight || particles[pID].zPos >= zPos + regionDepth)
    {
      pParticles->push_back(particles[pID]);
      particles.erase(particles.begin() + pID);
    }
  }
}
