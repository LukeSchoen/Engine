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

  const static int regionWidth = 256;
  const static int regionHeight = 256;
  const static int regionDepth = 256;

  void Process();

  void AddPoint(MDSOldParticle p);

  //Private
  std::vector<MDSOldParticle> particles;
};

#endif // MDSRegion_h__
