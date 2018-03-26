#ifndef MDSRegion_h__
#define MDSRegion_h__

#include "MDSParticle.h"

#include "stdint.h"
#include <thread>
#include <vector>

class MDSRegion
{
public:
  MDSRegion();
  MDSRegion(int64_t a_regionx, int64_t a_regiony, int64_t a_regionz, int64_t a_regionMomX, int64_t a_regionMomY, int64_t a_regionMomZ);

  int64_t xPos;
  int64_t yPos;
  int64_t zPos;
  int64_t xMom;
  int64_t yMom;
  int64_t zMom;

  uint8_t accelerationMode = 0;
  bool upToDate = true;

  std::vector<std::vector<int>> xLanes;
  std::vector<std::vector<int>> yLanes;
  std::vector<std::vector<int>> zLanes;

  const static int regionWidth = 256;
  const static int regionHeight = 256;
  const static int regionDepth = 256;

  void Update();

  void Process();

  void Optimise();

  void GetParticles(uint8_t x, uint8_t y, uint8_t z, uint8_t radius, std::vector<int> *pIds);

  void AddPoint(MDSOldParticle p);

  void GetExitingParticles(std::vector<MDSOldParticle> *pParticles);

  //Private
  std::vector<MDSOldParticle> particles;
};

#endif // MDSRegion_h__
