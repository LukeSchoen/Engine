#ifndef Controls_h__
#define Controls_h__
#include <SDL.h>
#include "Maths.h"
#include "GamePads.h"

struct Controls
{
  static bool Update();
  
  static bool KeyDown(SDL_Scancode key);

  static vec2i GetMouse();

  static vec2 GetMouseRelative();

  static bool GetLeftClick();

  static bool GetRightClick();

  static void SetMouseLock(bool lock);

  static int GetMouseScroll();

  static vec2 GetController2DAxis(int axisID = 0, int gameController = 0);

  static bool GetControllerButton(int button = 0, int gameController = 0);
};

#endif // Controls_h__
