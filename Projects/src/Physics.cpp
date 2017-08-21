#include "Physics.h"
#include "btBulletDynamicsCommon.h"

btVector3 ToBT(const vec3 &vec) { return btVector3(vec.x, vec.y, vec.z); }
vec3 FromBT(const btVector3 &vec) { return vec3(vec.x(), vec.y(), vec.z()); }

btTransform GetWorldTransform(const btCollisionObject *pObject)
{
  btTransform trans;
  const btRigidBody* body = btRigidBody::upcast(pObject);
  body && body->getMotionState() ? body->getMotionState()->getWorldTransform(trans) : trans = pObject->getWorldTransform();
  return trans;
}

PhysicsObject::PhysicsObject(btCollisionObject *a_pObject)
{
  pObject = a_pObject;
}

vec3 PhysicsObject::GetPos() const
{
  btTransform t = GetWorldTransform(pObject);
  return FromBT(t.getOrigin());
}

void PhysicsObject::SetPos(vec3 pos)
{
  btTransform t = GetWorldTransform(pObject);
  t.setOrigin(ToBT(pos));
  pObject->setWorldTransform(t);
}

vec3 PhysicsObject::GetSpeed() const
{
  const btRigidBody* body = btRigidBody::upcast(pObject);
  return FromBT(body->getLinearVelocity());
}

Quat PhysicsObject::GetOri() const
{
  btQuaternion ori = GetWorldTransform(pObject).getRotation();
  return Quat(ori.x(), ori.y(), ori.z(), ori.w());
}

mat4 PhysicsObject::GetModelMat() const
{
  mat4 mat, rot;
  GetOri().GetMatrix(&rot);
  rot.Transpose();
  mat.Translate(GetPos());
  mat *= rot;
  return mat;
}

void PhysicsObject::ApplyForce(vec3 force, vec3 offset)
{
  btRigidBody *body = btRigidBody::upcast(pObject);
  body->getLinearDamping();
  body->activate();
  body->applyForce(ToBT(force), ToBT(offset));
}

void PhysicsObject::SetCanSleep(bool canSleep)
{
  btRigidBody *body = btRigidBody::upcast(pObject);
  if (canSleep)
    body->setActivationState(ACTIVE_TAG);
  else
    body->setActivationState(DISABLE_DEACTIVATION);
}

PhysicsWorld::PhysicsWorld()
{
  btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
  btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
  btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
  btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
  world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
  world->setGravity(btVector3(0, -200, 0));
}

void PhysicsWorld::UpdateWorld(float StepSize, int subSteps)
{
  world->stepSimulation(StepSize, subSteps);
}

PhysicsObject PhysicsWorld::AddMesh(vec3 position, vec3 *vertData, int64_t vertCount, float mass)
{
  btTransform trans;
  trans.setIdentity();
  trans.setOrigin(ToBT(position));
  btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray();
  btIndexedMesh part;
  int *indicies = new int[vertCount];
  for (size_t v = 0; v < vertCount; v++) indicies[v] = v;
  part.m_vertexBase = (const unsigned char*)vertData;
  part.m_vertexStride = sizeof(vec3);
  part.m_numVertices = vertCount;
  part.m_triangleIndexBase = (const unsigned char*)indicies;
  part.m_triangleIndexStride = sizeof(int) * 3;
  part.m_numTriangles = vertCount / 3;
  part.m_indexType = PHY_INTEGER;
  meshInterface->addIndexedMesh(part, PHY_INTEGER);
  btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(meshInterface, true);
  btTransform worldTransform;
  worldTransform.setIdentity();
  btVector3 localInertia(0, 0, 0);
  btDefaultMotionState *myMotionState = new btDefaultMotionState(worldTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
  btRigidBody *body = new btRigidBody(rbInfo);
  world->addRigidBody(body);
  return PhysicsObject(body);
}

bool PhysicsWorld::RayCast(vec3 start, vec3 end, vec3 *hitPos)
{
  btVector3 btFrom(start.x, start.y, start.z);
  btVector3 btTo(end.x, end.y, end.z);
  btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);
  world->rayTest(btFrom, btTo, res);
  if (res.hasHit())
  {
    if (hitPos)
      *hitPos = FromBT(res.m_hitPointWorld);
    return true;
  }
  return false;
}

PhysicsObject PhysicsWorld::AddCube(vec3 position, vec3 dimensions, float mass)
{
  btCollisionShape *shape = new btBoxShape(ToBT(dimensions * 0.5f));
  btTransform worldTransform;
  worldTransform.setIdentity();
  worldTransform.setOrigin(ToBT(position));
  btVector3 localInertia(0, 0, 0);
  if (mass > 0.0f) shape->calculateLocalInertia(mass, localInertia);
  btDefaultMotionState *myMotionState = new btDefaultMotionState(worldTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
  btRigidBody* body = new btRigidBody(rbInfo);
  world->addRigidBody(body);
  return PhysicsObject(body);
}

PhysicsObject PhysicsWorld::AddSphere(vec3 position, float radius, float mass)
{
  btCollisionShape *shape = new btSphereShape(radius);
  btTransform worldTransform;
  worldTransform.setIdentity();
  worldTransform.setOrigin(ToBT(position));
  btVector3 localInertia(0, 0, 0);
  if (mass > 0.0f) shape->calculateLocalInertia(mass, localInertia);
  btDefaultMotionState *myMotionState = new btDefaultMotionState(worldTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
  btRigidBody* body = new btRigidBody(rbInfo);
  world->addRigidBody(body);
  return PhysicsObject(body);
}