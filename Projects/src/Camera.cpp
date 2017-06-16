#include "Camera.h"
#include "Controls.h"
#include "Window.h"
#include <time.h>

static vec3 pos = vec3(0, -50, 0);
//static vec3 pos = vec3(-3439.62012, -176.580917, -733.853149);
//static vec2 dir = vec2(0.531746447, -5.85340261);
static vec2 dir = vec2(0, DegsToRads * 180);

static int lastClock = 0;

vec3 Camera::Direction()
{
  float camSinY = sin(dir.y);
  float camCosY = cos(dir.y);
  float camSinX = sin(dir.x);
  float camCosX = cos(dir.x);
  return vec3(camSinY * camCosX, camSinX, camCosY * camCosX).Normalized();
}

void Camera::Update(float speed, bool only2D, bool useTimeDelta)
{
  const float MouseTurnSensitivity = 1.0f / 1000.0f;
  const float ControllerTurnSensitivity = 1.0f / 1000.0f * 2000;
  const float controllerDeadZone = 0.2;

  vec2 mouseTurn = Controls::GetMouseRelative();
  dir.y -= mouseTurn.x * MouseTurnSensitivity;
  dir.x += mouseTurn.y * MouseTurnSensitivity;

  int time = clock();
  float timeDelta = (time - lastClock) / 1000.0;
  lastClock = clock();
  if (!useTimeDelta)
    timeDelta = 1;

  vec2 controllerTurn = Controls::GetController2DAxis(1);
  controllerTurn = vec2(controllerTurn.y, -controllerTurn.x);
  if (fabs(controllerTurn.x) < controllerDeadZone) controllerTurn.x = 0;
  if (fabs(controllerTurn.y) < controllerDeadZone) controllerTurn.y = 0;
  dir = dir + Delinerize(controllerTurn, 3.0f) * ControllerTurnSensitivity * timeDelta;

  float camSinY = sin(dir.y);
  float camCosY = cos(dir.y);
  float camSinX = sin(dir.x);
  float camCosX = cos(dir.x);


  speed *= timeDelta;
  if (Controls::KeyDown(SDL_SCANCODE_LCTRL))
    speed *= 0.2f;

  if (Controls::KeyDown(SDL_SCANCODE_LSHIFT))
    speed *= 5.0f;

  // Controller Speed
  vec2 controllerSpeed = Controls::GetController2DAxis(2);
  float cSpeed = (((controllerSpeed.y + 1) / 2.0f) - (((controllerSpeed.x + 1) / 2.0f))) + 1;
  speed *= Delinerize(cSpeed, 3.0f);

  // Controller Movement
  vec2 controllerMove = Controls::GetController2DAxis(0);
  if (fabs(controllerMove.x) < controllerDeadZone) controllerMove.x = 0;
  if (fabs(controllerMove.y) < controllerDeadZone) controllerMove.y = 0;
  controllerMove = Delinerize(controllerMove, 3.0f);
  if (only2D)
  {
    pos.x += camSinY * speed * -controllerMove.y;
    pos.z += camCosY * speed * -controllerMove.y;
    pos.x -= camCosY * speed * controllerMove.x;
    pos.z += camSinY * speed * controllerMove.x;
  }
  else
  {
    pos.x += camSinY * camCosX * speed * -controllerMove.y;
    pos.y += camSinX * speed * -controllerMove.y;
    pos.z += camCosY * camCosX * speed * -controllerMove.y;
    pos.x -= camCosY * speed * controllerMove.x;
    pos.z += camSinY * speed * controllerMove.x;
  }


  if (Controls::KeyDown(SDL_SCANCODE_W))
  {
    if (only2D)
    {
      pos.x += camSinY * speed;
      pos.z += camCosY * speed;
    }
    else
    {
      pos.x += camSinY * camCosX * speed;
      pos.y += camSinX * speed;
      pos.z += camCosY * camCosX * speed;
    }
  }

  if (Controls::KeyDown(SDL_SCANCODE_S))
  {
    if (only2D)
    {
      pos.x -= camSinY * speed;
      pos.z -= camCosY * speed;
    }
    else
    {
      pos.x -= camSinY * camCosX * speed;
      pos.y -= camSinX * speed;
      pos.z -= camCosY * camCosX * speed;
    }
  }

  if (Controls::KeyDown(SDL_SCANCODE_A))
  {
    pos.x += camCosY * speed;
    pos.z -= camSinY * speed;
  }

  if (Controls::KeyDown(SDL_SCANCODE_D))
  {
    pos.x -= camCosY * speed;
    pos.z += camSinY * speed;
  }

  if (Controls::GetControllerButton(8))
    pos.y += speed * 4;

  if (Controls::GetControllerButton(9))
    pos.y -= speed * 4;

  if (Controls::KeyDown(SDL_SCANCODE_X))
  {
    pos.y += speed;
  }

  if (Controls::KeyDown(SDL_SCANCODE_E))
  {
    pos.y -= speed;
  }

}

vec3 Camera::Position()
{
  return pos;
}

vec2 Camera::Rotation()
{
  return dir;
}

mat4 Camera::Matrix()
{
  mat4 viewMat;
  viewMat.LoadIdentity();
  viewMat.Rotate(Camera::Rotation().x, 0, 0);
  viewMat.Rotate(0, Camera::Rotation().y, 0);
  viewMat.Translate(Camera::Position());
  return viewMat;
}

mat4 Camera::RotationMatrix()
{
  mat4 viewMat;
  viewMat.LoadIdentity();
  viewMat.Rotate(Camera::Rotation().x, 0, 0);
  viewMat.Rotate(0, Camera::Rotation().y, 0);
  return viewMat;
}

void Camera::SetPosition(const vec3 &newPos)
{
  pos = newPos;
}

void Camera::SetRotation(const vec2 &newRot)
{
  dir = newRot;
}

void Camera::SetRotation(const vec3 &newDir)
{
  vec3 forward(0, 0, 1);
  float pitch = sin(newDir.y);
  float yaw = atan2(newDir.x, newDir.z);
  dir = vec2(pitch, yaw);
  //dir;
}

vec2 Camera::GetRotation()
{
  return dir;
}
