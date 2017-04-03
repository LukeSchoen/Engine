#ifndef Warthog_h__
#define Warthog_h__
#include "Maths.h"
#include "PolyModel.h"

struct Wheel
{
  Wheel();
  vec3 position;
  //vec3 velocity;
};

struct Warthog
{
  float motorStrength = 0.01;
  vec3 position;
  vec3 velocity;
  vec3 rotation = vec3(0, 0, 0);
  PolyModel carModel;
  PolyModel wheelModel;
  Wheel wheels[4];
  Warthog(vec3 _position);

  void update(const PolyMesh& worldMesh);

  void draw(const mat4 &VP);
};

#endif // Warthog_h__
