#include "Warthog.h"
#include "Controls.h"
#include "Camera.h"
#include "Audio.h"

bool camFollowCar = true;

Wheel::Wheel()
{

}

Warthog::Warthog(vec3 _position)
{
  position = _position;
  carModel.LoadModel("../Assets/HALO/WartHog.dae");
  //wheelModel.LoadModel("C:\\Users\\Administrator\\Desktop\\ball.dae", true);
}

void Warthog::update(const PolyMesh& worldMesh)
{
  mat4 modelMat;
  modelMat.Rotate(rotation);
  modelMat.Transpose();

  // Controls
  //if (Controls::KeyDown(SDL_SCANCODE_UP)) rotation.x += 0.02;
  //if (Controls::KeyDown(SDL_SCANCODE_DOWN)) rotation.x -= 0.02;

  static bool camDown = false;
  if (Controls::KeyDown(SDL_SCANCODE_C))
  {
    if (!camDown)
      camFollowCar = !camFollowCar;
    camDown = true;
  }
  else
  {
    camDown = false;
  }

  if (Controls::KeyDown(SDL_SCANCODE_LEFT)) rotation.y -= 0.025;
  if (Controls::KeyDown(SDL_SCANCODE_RIGHT)) rotation.y += 0.025;

  if (Controls::KeyDown(SDL_SCANCODE_N)) rotation.z -= 0.02;
  if (Controls::KeyDown(SDL_SCANCODE_M)) rotation.z += 0.02;

  if (Controls::KeyDown(SDL_SCANCODE_P)) position.y += 0.5;
  if (Controls::KeyDown(SDL_SCANCODE_L)) position.y -= 0.5;

  float yvel = velocity.y;
  // If on ground
  bool moving = false;
  if (Controls::KeyDown(SDL_SCANCODE_UP))
  {
    moving = true;
    velocity = velocity + (modelMat * vec4(0, 0, -1)) * motorStrength;
  }
  if (Controls::KeyDown(SDL_SCANCODE_DOWN))
  {
    moving = true;
    velocity = velocity + (modelMat * vec4(0, 0, 0.6)) * motorStrength;
  }
  if (!moving)
    velocity = velocity *0.95;
  else
    velocity = velocity *0.98;

  velocity.y = yvel;

  float loudness = Min(Max(velocity.Length() * 750 - 50,0),500) * 200;

  loudness /= ((vec3() - Camera::Position()) - position).LengthSquared();

  Audio::ChangeVolume("Engine", int(loudness));



  position = position + velocity;

  // wheel positioning
  vec4 centreToFrontAxel = vec4(0, 0, -2.6);
  vec4 centreToBackAxel = vec4(0, 0, 3.5);
  vec4 centreToWheel = vec4(1.5, 0, 0);
  vec4 centreToGround = vec4(0, -0.9, 0);

  centreToFrontAxel = modelMat * centreToFrontAxel;
  centreToBackAxel = modelMat * centreToBackAxel;
  centreToWheel = modelMat * centreToWheel;
  centreToGround = modelMat * centreToGround;

  vec3 targetWheels[4];
  targetWheels[0] = position + centreToFrontAxel + centreToWheel + centreToGround;
  targetWheels[1] = position + centreToFrontAxel + vec3() - centreToWheel + centreToGround;
  targetWheels[2] = position + centreToBackAxel + centreToWheel + centreToGround;
  targetWheels[3] = position + centreToBackAxel + vec3() - centreToWheel + centreToGround;

  // Move Wheels
  for (int i = 0; i < 4; i++)
    wheels[i].position = worldMesh.SlideSweepSphere(Sphere(wheels[i].position), targetWheels[i] - wheels[i].position);

  vec3 averageFront = (wheels[0].position + wheels[1].position) / 2.0f;
  vec3 averageBack = (wheels[2].position + wheels[3].position) / 2.0f;

  vec3 averageLeft = (wheels[0].position + wheels[2].position) / 2.0f;
  vec3 averageRight = (wheels[1].position + wheels[3].position) / 2.0f;

  float frontBackHeightDiff = averageFront.y - averageBack.y;
  float frontBackLatDiff = (vec2(averageFront.x, averageFront.z) - vec2(averageBack.x, averageBack.z)).Length();

  float rightLeftHeightDiff = averageRight.y - averageLeft.y;
  float rightLeftLatDiff = (vec2(averageRight.x, averageRight.z) - vec2(averageLeft.x, averageLeft.z)).Length();

  float pitch = atan2(frontBackHeightDiff, frontBackLatDiff);
  float roll = atan2(rightLeftHeightDiff, rightLeftLatDiff);

  // Move car to wheels
  targetWheels[0] = wheels[0].position - centreToFrontAxel - centreToWheel - centreToGround;
  targetWheels[1] = wheels[1].position - centreToFrontAxel - (vec3() - centreToWheel) - centreToGround;
  targetWheels[2] = wheels[2].position - centreToBackAxel - centreToWheel - centreToGround;
  targetWheels[3] = wheels[3].position - centreToBackAxel - (vec3() - centreToWheel) - centreToGround;

  rotation.x = pitch;
  //rotation.z = roll;

  // Move car down for gravity
  position = (targetWheels[0] + targetWheels[1] + targetWheels[2] + targetWheels[3]) / 4.0f;
  position = position + vec3(0, -0.2, 0);


  modelMat.LoadIdentity();
  modelMat.RotateY(rotation.y);
  modelMat.Transpose();

  vec3 oldCamPos = vec3() - Camera::Position();
  static float oldCamRot = Camera::Direction().y;

  vec3 newCamPos = ((oldCamPos * 49) + ((position + (modelMat * vec4(0, 0, 15) + vec4(0, 3, 0))))) * 0.02;
  newCamPos = worldMesh.SlideSweepSphere(Sphere(oldCamPos, 2), newCamPos - oldCamPos);

  if (camFollowCar)
  {
    oldCamRot = ((oldCamRot * 49) + (0 - rotation.y)) * 0.02;
    Camera::SetPosition(vec3() - newCamPos);
    Camera::SetRotation(vec2(0, oldCamRot));
  }
}

void Warthog::draw(const mat4 &VP)
{
  // Draw Car Body
  mat4 modelMat;
  mat4 MVP;
  modelMat.Translate(position);
  modelMat.Rotate(rotation);
  MVP = VP * modelMat;
  MVP.Transpose();
  carModel.Render(MVP);

  // Draw Wheels
  for (int i = 0; i < 4; i++)
  {
    modelMat.LoadIdentity();
    modelMat.Translate(wheels[i].position);

    MVP = VP * modelMat;
    MVP.Transpose();
    //wheelModel.Render(MVP);
  }
}
