#include "Grass.h"
#include "Window.h"
#include "FrameRate.h"
#include "Controls.h"
#include "GPix.h"

GrassWorld grassWorld;

void _UpdateWorld()
{
  std::swap(grassWorld.grid, grassWorld.newgrid);

  // Agent Death
  for (int y = 0; y < grassWorld.gridHeight; y++)
    for (int x = 0; x < grassWorld.gridWidth; x++)
    {
      GPix &gpix = grassWorld.newgrid[x + y * grassWorld.gridWidth];
      if (gpix.energy == 0)
        gpix.red = gpix.green = gpix.blue = 0;
    }

  // Agent Life
  for (int i = 0; i < (grassWorld.gridWidth * grassWorld.gridHeight) / 2; i++)
  {
    //GPix &gpix = grassWorld.grid[(rand() % grassWorld.gridWidth) + (rand() % grassWorld.gridHeight) * grassWorld.gridWidth];
    //if(gpix.energy > 0) gpix.energy = Min(gpix.energy + 1, 255);
  }

  // Agent step
  for (int y = 0; y < grassWorld.gridHeight; y++)
    for (int x = 0; x < grassWorld.gridWidth; x++)
    {
      GPix &gpix = grassWorld.grid[x + y * grassWorld.gridWidth];
      if(gpix.energy == 0) continue;

      gpix.energy = Min(gpix.energy + 1, 255);

      grassWorld.xpos = x;
      grassWorld.ypos = y;
      ActionType action = gpix.brain.Think(gpix);
      switch (action)
      {
      case Detonate: for (int iy = -1; iy <= 1; iy++) for (int ix = -1; ix <= 1; ix++) { GPix &ow = SetNewGPixOffset(0, 0); const GPix &o = GetGPixOffset(ix, iy); ow.energy = 0; ow.red = 255; ow.green = 32; ow.blue = 32; }; break;
      case MoveLeft: {GPix &ow = SetNewGPixOffset(-1, 0); const GPix &o = GetGPixOffset(-1, 0); if (o.useable) { SetNewGPixOffset(0, 0) = gpix; ow = gpix; MutateAgent(&ow), gpix.energy = 0; gpix.red = 0; gpix.green = 0; gpix.blue = 0; } } break;
      case MoveRight: {GPix &ow = SetNewGPixOffset(1, 0); const GPix &o = GetGPixOffset(1, 0); if (o.useable)  { SetNewGPixOffset(0, 0) = gpix; ow = gpix; MutateAgent(&ow), gpix.energy = 0; gpix.red = 0; gpix.green = 0; gpix.blue = 0; } } break;
      case MoveUp: {GPix &ow = SetNewGPixOffset(0, -1); const GPix &o = GetGPixOffset(0, -1); if (o.useable)   { SetNewGPixOffset(0, 0) = gpix; ow = gpix; MutateAgent(&ow), gpix.energy = 0; gpix.red = 0; gpix.green = 0; gpix.blue = 0; } } break;
      case MoveDown: {GPix &ow = SetNewGPixOffset(0, 1); const GPix &o = GetGPixOffset(0, 1); if (o.useable)   { SetNewGPixOffset(0, 0) = gpix; ow = gpix; MutateAgent(&ow), gpix.energy = 0; gpix.red = 0; gpix.green = 0; gpix.blue = 0; } } break;
      case GiveLeft: {GPix &ow = SetNewGPixOffset(-1, 0); const GPix &o = GetGPixOffset(-1, 0); if (o.energy == 0) { gpix.energy /= 2; ow = gpix; } else { ow.energy = Min(o.energy + gpix.energy / 2, 255); gpix.energy /= 2; ow = std::move(o); } } break;
      case GiveRight: {GPix &ow = SetNewGPixOffset(1, 0); const GPix &o = GetGPixOffset(1, 0); if (o.energy == 0) { gpix.energy /= 2; ow = gpix; } else { ow.energy = Min(o.energy + gpix.energy / 2, 255); gpix.energy /= 2; ow = std::move(o); } } break;
      case GiveUp: {GPix &ow = SetNewGPixOffset(0, -1); const GPix &o = GetGPixOffset(0, -1); if (o.energy == 0) { gpix.energy /= 2; ow = gpix; } else { ow.energy = Min(o.energy + gpix.energy / 2, 255); gpix.energy /= 2;  ow = std::move(o); } } break;
      case GiveDown: {GPix &ow = SetNewGPixOffset(0, 1); const GPix &o = GetGPixOffset(0, 1); if (o.energy == 0) { gpix.energy /= 2; ow = gpix; } else { ow.energy = Min(o.energy + gpix.energy / 2, 255); gpix.energy /= 2;  ow = std::move(o); } } break;
      case TakeLeft: {GPix &ow = SetNewGPixOffset(-1, 0); const GPix &o = GetGPixOffset(-1, 0); if (o.energy > 0) { gpix.energy = Min(gpix.energy + o.energy / 2, 255); ow.energy /= 2;ow = std::move(o); } } break;
      case TakeRight: {GPix &ow = SetNewGPixOffset(1, 0); const GPix &o = GetGPixOffset(1, 0); if (o.energy > 0) { gpix.energy = Min(gpix.energy + o.energy / 2, 255); ow.energy /= 2; ow = std::move(o);} } break;
      case TakeUp: {GPix &ow = SetNewGPixOffset(0, -1); const GPix &o = GetGPixOffset(0, -1); if (o.energy > 0) { gpix.energy = Min(gpix.energy + o.energy / 2, 255); ow.energy /= 2; ow = std::move(o);} } break;
      case TakeDown: {GPix &ow = SetNewGPixOffset(0, 1); const GPix &o = GetGPixOffset(0, 1); if (o.energy > 0) { gpix.energy = Min(gpix.energy + o.energy / 2, 255); ow.energy /= 2; ow = std::move(o);} } break;
      default: SetNewGPixOffset(0, 0) = std::move(GetGPixOffset(0, 0)); break;
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
//       if (gpix.energy == 0)
//         gpix.energy = 255;
      //uint8_t r = gpix.red * 255 / gpix.energy;
      //uint8_t g = gpix.green * 255 / gpix.energy;
      //uint8_t b = gpix.blue * 255 / gpix.energy;

      uint8_t r = 0;
      uint8_t g = 0;
      uint8_t b = 0;

      r += gpix.red;
      g += gpix.green;
      b += gpix.blue;

      //r >>= 1;
      //g >>= 1;
      //b >>= 1;

      r = Min((uint64_t)r + (uint64_t)gpix.red * gpix.energy / 256, 255);
      g = Min((uint64_t)g + (uint64_t)gpix.green * gpix.energy / 256, 255);
      b = Min((uint64_t)b + (uint64_t)gpix.blue * gpix.energy / 256, 255);

      r >>= 1;
      g >>= 1;
      b >>= 1;

      uint32_t c = b + (g << 8) + (r << 16);
      if (gpix.energy != 0)
        for (int iy = 0; iy < zoom; iy++)
          for (int ix = 0; ix < zoom; ix++)
            window.pixels[x * zoom + ix + (y * zoom + iy) * window.width] = c;
    }
}

void Grass()
{
  Window window("Grass", false, 640, 480, false); // Create Game Window

  // Create game World
  const int zoom = 8;
  grassWorld.gridWidth = window.width / zoom;
  grassWorld.gridHeight = window.height / zoom;
  grassWorld.grid = new GPix[grassWorld.gridWidth * grassWorld.gridHeight];
  grassWorld.newgrid = new GPix[grassWorld.gridWidth * grassWorld.gridHeight];
  grassWorld.RandomizeBuffer();
  
  for (int y = 0; y < grassWorld.gridHeight; y++)
    for (int x = 0; x < grassWorld.gridWidth; x++)
    {
      grassWorld.grid[x + y * grassWorld.gridWidth].useable = true;
      grassWorld.newgrid[x + y * grassWorld.gridWidth].useable = true;
    }

  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 0, 0);

    //if (rand() % 2 == 0)
      
    for (int i = 0; i < 16; i++)
      _AddRandomAgent();

    _UpdateWorld();

    GrassWorld *grassWorldTemp;


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
