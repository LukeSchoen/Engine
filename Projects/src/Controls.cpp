#include "Controls.h"

static uint8_t const* keyboardState;
static int mouseX = 0;
static int mouseY = 0; 
static int mouseScroll = 0;
static GamePads controllers;
static bool leftClick = false;
static bool rightClick = false;

bool Controls::Update()
{
  SDL_PumpEvents(); // request fresh mouse position
  keyboardState = SDL_GetKeyboardState(NULL);
  uint32_t state = SDL_GetRelativeMouseState(&mouseX, &mouseY);
  leftClick = (state & SDL_BUTTON_LMASK) > 0;
  rightClick = (state & SDL_BUTTON_RMASK) > 0;

  SDL_Event e;
  while (SDL_PollEvent(&e))
    switch (e.type)
    {
      case SDL_QUIT: return false;
      case SDL_MOUSEWHEEL: mouseScroll += e.wheel.y; break;
      default: controllers.HandleEvent(e);
    }
  if (KeyDown(SDL_SCANCODE_ESCAPE)) return false;
  return true;
}

bool Controls::KeyDown(SDL_Scancode key)
{
  return keyboardState[key];
}

vec2i Controls::GetMouse()
{
  SDL_GetMouseState(&mouseX, &mouseY);
  return vec2i(mouseX, mouseY);
}

vec2 Controls::GetMouseRelative()
{
  vec2 ret = vec2(mouseX, mouseY);
  mouseX = 0;
  mouseY = 0;
  return ret;
}

bool Controls::GetLeftClick()
{
  return leftClick;
}

bool Controls::GetRightClick()
{
  return rightClick;
}

void Controls::SetMouseLock(bool lock)
{
  SDL_SetRelativeMouseMode(lock ? SDL_TRUE : SDL_FALSE);
}

vec2 Controls::GetController2DAxis(int axisID /*= 0*/, int gameController /*= 0*/)
{
  if (gameController >= controllers.gamePadCount) return vec2(); // Make sure controller is attached
  return vec2(controllers.gamePads[gameController].analogs[axisID * 2 + 0], controllers.gamePads[gameController].analogs[axisID * 2 + 1]);
}

bool Controls::GetControllerButton(int buttonID /*= 0*/, int gameController /*= 0*/)
{
  if (gameController >= controllers.gamePadCount) return false; // Make sure controller is attached
  return controllers.gamePads[gameController].buttons[buttonID];
}