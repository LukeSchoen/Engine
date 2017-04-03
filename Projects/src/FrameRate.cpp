#include "FrameRate.h"
#include <stdio.h>
#include "SDL_timer.h"
#include "Camera.h"

static int ThisTime = 0;
static int LastTime = 0;
static int frameRate = 0;
static int draws = 0;

void FrameRate::Update()
{
  ThisTime = SDL_GetTicks();
  if (ThisTime - LastTime > 1000)
  {
    LastTime = ThisTime;
    if (frameRate)
    {
      printf("FPS: %d\n", frameRate);
      printf("POLYS: %d\n", draws / frameRate);
      //printf("x %f, y %f, z %f", Camera::Position().x, Camera::Position().y, Camera::Position().z);
    }
    frameRate = 0;
    draws = 0;
  }
  frameRate++;
}

void FrameRate::AddDraw(int amt)
{
  draws += amt;
}
