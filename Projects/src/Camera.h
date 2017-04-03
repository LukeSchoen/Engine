#ifndef Camera_h__
#define Camera_h__
#include "Maths.h"

struct Camera
{
  static void Update(float speed = 10);
  static vec3 Position();
  static vec2 Rotation();
  static vec3 Direction();
  static mat4 Matrix();
  static mat4 RotationMatrix();
  static void SetPosition(const vec3 &newPos);
  static void SetRotation(const vec2 &newRot);
  static void SetRotation(const vec3 &newDir);
  static vec2 GetRotation();
};

#endif // Camera_h__
