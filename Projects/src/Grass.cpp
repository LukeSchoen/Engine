#include "Grass.h"
#include "Window.h"
#include "FrameRate.h"
#include "Controls.h"
#include "GPix.h"

GrassWorld grassWorld;

void _UpdateWorld()
{
  // Sprinkle food

  // Agent Death
  for (int y = 0; y < grassWorld.gridHeight; y++)
    for (int x = 0; x < grassWorld.gridWidth; x++)
    {
      GPix &gpix = grassWorld.grid[x + y * grassWorld.gridWidth];
      if (gpix.energy == 0)
        gpix.red = gpix.green = gpix.blue = 0;
    }

  // Agent step
  for (int y = 0; y < grassWorld.gridHeight; y++)
    for (int x = 0; x < grassWorld.gridWidth; x++)
    {
      GPix &gpix = grassWorld.grid[x + y * grassWorld.gridWidth];
      if(gpix.energy == 0) continue;
      grassWorld.xpos = x;
      grassWorld.ypos = y;
      ActionType action = gpix.brain.Think(gpix);
      switch (action)
      {
      case Detonate: for (int iy = -1; iy <= 1; iy++) for (int ix = -1; ix <= 1; ix++) { GPix &o = GetGPixOffset(ix, iy); o.energy = 0; o.red = 255; o.green = 32; o.blue = 32; }; break;
      case MoveLeft: {GPix &o = GetGPixOffset(-1, 0); if (o.useable) { o = gpix; gpix.energy = 0; gpix.red = 0; gpix.green = 0; gpix.blue = 0; } } break;
      case MoveRight: {GPix &o = GetGPixOffset(1, 0); if (o.useable) { o = gpix; gpix.energy = 0; gpix.red = 0; gpix.green = 0; gpix.blue = 0; } } break;
      case MoveUp: {GPix &o = GetGPixOffset(0, -1); if (o.useable) { o = gpix; gpix.energy = 0; gpix.red = 0; gpix.green = 0; gpix.blue = 0; } } break;
      case MoveDown: {GPix &o = GetGPixOffset(0, 1); if (o.useable) { o = gpix; gpix.energy = 0; gpix.red = 0; gpix.green = 0; gpix.blue = 0; } } break;
      case GiveLeft: {GPix &o = GetGPixOffset(-1, 0); if (o.energy == 0) { gpix.energy /= 2; o = gpix; } else { o.energy = Min(o.energy + gpix.energy / 2, 255); gpix.energy /= 2; } } break;
      case GiveRight: {GPix &o = GetGPixOffset(1, 0); if (o.energy == 0) { gpix.energy /= 2; o = gpix; } else { o.energy = Min(o.energy + gpix.energy / 2, 255); gpix.energy /= 2; } } break;
      case GiveUp: {GPix &o = GetGPixOffset(0, -1); if (o.energy == 0) { gpix.energy /= 2; o = gpix; } else { o.energy = Min(o.energy + gpix.energy / 2, 255); gpix.energy /= 2; } } break;
      case GiveDown: {GPix &o = GetGPixOffset(0, 1); if (o.energy == 0) { gpix.energy /= 2; o = gpix; } else { o.energy = Min(o.energy + gpix.energy / 2, 255); gpix.energy /= 2; } } break;
      case TakeLeft: {GPix &o = GetGPixOffset(-1, 0); if (o.energy > 0) { gpix.energy = Min(gpix.energy + o.energy / 2, 255); o.energy /= 2; } } break;
      case TakeRight: {GPix &o = GetGPixOffset(1, 0); if (o.energy > 0) { gpix.energy = Min(gpix.energy + o.energy / 2, 255); o.energy /= 2; } } break;
      case TakeUp: {GPix &o = GetGPixOffset(0, -1); if (o.energy > 0) { gpix.energy = Min(gpix.energy + o.energy / 2, 255); o.energy /= 2; } } break;
      case TakeDown: {GPix &o = GetGPixOffset(0, 1); if (o.energy > 0) { gpix.energy = Min(gpix.energy + o.energy / 2, 255); o.energy /= 2; } } break;
      }
    }
}

void _AddRandomAgent()
{
  int xpos = rand() % grassWorld.gridWidth;
  int ypos = rand() % grassWorld.gridHeight;
  GPix &newGras = grassWorld.grid[xpos + ypos * grassWorld.gridWidth];
  // Set energy
  newGras.energy = 128;
  // Set Color
  while (newGras.red + newGras.green + newGras.blue < 128)
  {
    newGras.red = rand() % 255;
    newGras.green = rand() % 255;
    newGras.blue = rand() % 255;
  }
  // Create brain
  newGras.brain.thoughts.resize(16);
  for (auto &thought : newGras.brain.thoughts)
  {
    thought.ideas.resize(4);
    for (auto &idea : thought.ideas)
    {
      idea.conditions.resize(rand() % 3 + 1);
      for (auto &condition : idea.conditions)
        condition = ConditionType(rand() % ConditionNum);
      idea.action = ActionType(rand() % ActionNum);
    }
  }
}

void _DrawWorld(const Window &window, int zoom)
{
  // Draw Grass
  for (int y = 0; y < grassWorld.gridHeight; y++)
    for (int x = 0; x < grassWorld.gridWidth; x++)
    {
      GPix &gpix = grassWorld.grid[x + y * grassWorld.gridWidth];
      uint32_t c = gpix.blue + (gpix.green << 8) + (gpix.red << 16);
      for (int iy = 0; iy < zoom; iy++)
        for (int ix = 0; ix < zoom; ix++)
          window.pixels[x * zoom + ix + (y * zoom + iy) * window.width] = c;
    }
}

void Grass()
{
  Window window("SecondStream", false, 1920, 1080, true); // Create Game Window

  // Create game World
  const int zoom = 4;
  grassWorld.gridWidth = window.width / zoom;
  grassWorld.gridHeight = window.height / zoom;
  grassWorld.grid = new GPix[grassWorld.gridWidth * grassWorld.gridHeight];
  for (int y = 0; y < grassWorld.gridHeight; y++)
    for (int x = 0; x < grassWorld.gridWidth; x++)
      grassWorld.grid[x + y * grassWorld.gridWidth].useable = true;

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 0, 0);

    //if (rand() % 2 == 0)
      
    for (int i = 0; i < 16; i++)
      _AddRandomAgent();

    _UpdateWorld();

    _DrawWorld(window, zoom);

    window.Swap(); // Swap Window
    FrameRate::Update();

    if (Controls::KeyDown(SDL_SCANCODE_1))
      SDL_Delay(50);
    if (Controls::KeyDown(SDL_SCANCODE_2))
      SDL_Delay(100);
    if (Controls::KeyDown(SDL_SCANCODE_3))
      SDL_Delay(200);
  }


}
