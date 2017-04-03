#ifndef JoyStick_h__
#define JoyStick_h__
#include "SDL_joystick.h"
#include "SDL_events.h"
#include "Maths.h"

struct Controller
{
  float analogs[256] = { 0 };
  bool  buttons[256] = { 0 };

  SDL_Joystick *SDLJoystick = nullptr;
};

struct GamePads
{
  Controller *gamePads = nullptr;
  int64_t gamePadCount = 0;
  GamePads();
  ~GamePads();
  void HandleEvent(SDL_Event e);
  void SetButton(uint8_t gamePadID, uint8_t button, bool state);
};

#endif // JoyStick_h__
