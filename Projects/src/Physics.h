#ifndef Bullet_h__
#define Bullet_h__

#include "Maths.h"

class btCollisionObject;
class btDiscreteDynamicsWorld;

class PhysicsObject
{
public:
  PhysicsObject(btCollisionObject *a_pObject);
  vec3 GetPos() const;
  void SetPos(vec3 pos);
  Quat GetOri() const;
  vec3 GetSpeed() const;
  mat4 GetModelMat() const;
  void ApplyForce(vec3 force, vec3 offset = vec3());
  void SetCanSleep(bool canSleep = true);
private:
  btCollisionObject *pObject;
};

class PhysicsWorld
{
public:
  PhysicsWorld();

  void UpdateWorld(float StepSize = 1.f / 60.f, int subSteps = 10);

  PhysicsObject AddSphere(vec3 position, float radius, float mass);
  PhysicsObject AddCube(vec3 position, vec3 dimensions, float mass);
  PhysicsObject AddMesh(vec3 position, vec3 *vertData, int64_t vertCount, float mass);

  bool RayCast(vec3 start, vec3 end, vec3 *hitPos = nullptr);

private:
  btDiscreteDynamicsWorld *world;
};

#endif // Bullet_h__
