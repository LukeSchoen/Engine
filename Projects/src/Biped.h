#ifndef Biped_h__
#define Biped_h__

#include "maths.h"

class Biped
{
public:
  static void Draw(mat4 VP, vec3 pos, vec3 dir, uint32_t texture, bool use3D = true);

private:
  float walkCycle = 0;

};

#endif // Biped_h__
