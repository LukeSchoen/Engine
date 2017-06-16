#include "JamesBot.h"
#include "Window.h"
#include "Controls.h"

void JamesBot()
{
  Window window;

  window.pixels;

  while (Controls::Update())
  {



    window.Swap();
  }

}
