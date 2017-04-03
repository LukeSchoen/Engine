#include "GamePads.h"
#include "SDL.h"

GamePads::GamePads()
{
  SDL_Init(SDL_INIT_JOYSTICK);
  gamePadCount = SDL_NumJoysticks();
  gamePads = new Controller[gamePadCount];
  for (int cItr = 0; cItr < gamePadCount; cItr++)
    gamePads[cItr].SDLJoystick = SDL_JoystickOpen(cItr);
}

GamePads::~GamePads()
{
 // for (int cItr = 0; cItr < gamePadCount; cItr++)
 //    SDL_JoystickClose(gamePads[cItr].SDLJoystick);
}

void GamePads::HandleEvent(SDL_Event e)
{
  switch (e.type)
  {
  case SDL_JOYAXISMOTION:
    gamePads[e.jaxis.which].analogs[e.jaxis.axis] = ((e.jaxis.value + 32768) / 65536.0f) * 2 - 1;
    break;

  case SDL_JOYBUTTONDOWN:
    SetButton(e.jbutton.which, e.jbutton.button, true);
    break;

  case SDL_JOYBUTTONUP:
    SetButton(e.jbutton.which, e.jbutton.button, false);
    break;

  default:
    break;
  }
}

void GamePads::SetButton(uint8_t gamePadID, uint8_t button, bool state)
{
  gamePads[gamePadID].buttons[button] = state;
}
