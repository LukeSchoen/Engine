#ifndef WireWars_h__
#define WireWars_h__

#include <vector>
#include "Window.h"
#include "Controls.h"

uint32_t tileColors[] = { 0x993d00, 0xff9933, 0xffffff, 0x3399ff };

class WireWarrior
{
public:
  std::vector<uint8_t> cells;
  WireWarrior() { cells.resize(size * size); }

  void ApplyRotation(int *x, int *y)
  {
    if (rot == 1)
    {
      int t = *x;
      *x = *y;
      *y = t;
    }
    if (rot == 2)
    {
      *x = -*x;
      *y = -*y;
    }
    if (rot == 3)
    {
      int t = *x;
      *x = *y;
      *y = t;
      *x = -*x;
      *y = -*y;
    }
  }
  uint8_t GetBlock(int x, int y)
  {
    ApplyRotation(&x, &y);
    return cells[x + y * size];
  }
  void SetBlock(int x, int y, uint8_t block)
  {
    ApplyRotation(&x, &y);
    cells[x + y * size] = block;
  }

  void MutationAdd(uint8_t blockType)
  {
    int64_t segments = (rand() % 8);
    segments *= segments;

    int64_t xpos = rand() % size;
    int64_t ypos = rand() % size;
    int64_t xdir = (rand() % 3) - 1;
    int64_t ydir = (rand() % 3) - 1;
    for (int64_t s = 0; s < segments; s++)
    {
      if (rand() & 1)
        xdir = (rand() % 3) - 1;
      else
        ydir = (rand() % 3) - 1;
      int64_t len = (rand() & 1) + (rand() & 1);
      len *= len + (rand() & 1);
      for (int64_t c = 0; c < len; c++)
      {
        if (xpos < 0) xpos = 0;
        if (xpos >= size) xpos = size - 1;
        if (ypos < 0) ypos = 0;
        if (ypos >= size) ypos = size - 1;
        SetBlock(xpos, ypos, blockType);
        xpos += xdir;
        ypos += ydir;
      }
    }
  }

  void Step()
  {
    std::vector<uint8_t> oldCells = cells;
    for (int y = 0; y < size; y++)
      for (int x = 0; x < size; x++)
      {
        uint8_t oldCell = oldCells[x + y * size];
        uint8_t &newCell = cells[x + y * size];
        if (oldCell == 1) // Conductor to head
        {
          int8_t neighbours = 0;
          for (int ix = -1; ix <= 1; ix++)
            for (int iy = -1; iy <= 1; iy++)
              if (ix + x >= 0 && iy + y >= 0 && ix + x < size && iy + y < size && (ix || iy))
                if(oldCells[(x + ix) + (y + iy) * size] == 2)
                  neighbours++;
          if (neighbours >= 1 && neighbours <= 2)
            newCell = 2;
        }
        if (oldCell == 2) newCell = 3; // Head to tail
        if (oldCell == 3) newCell = 1; // Tail to conductor
      }
  }

  int64_t posx = 0;
  int64_t posy = 0;
  uint8_t rot = 0;
  int64_t size = 64;

private:
};

class WireWar
{
public:
  void Draw(Window *pWindow, int zoom = 1)
  {
    for (auto & warrior : warriors)
    {
      for (int zy = 0; zy < zoom; zy++)
        for (int zx = 0; zx < zoom; zx++)
          for (int y = 0; y < warrior.size; y++)
            for (int x = 0; x < warrior.size; x++)
            {
              int ix = (x + warrior.posx) * zoom + zx;
              int iy = (y + warrior.posy) * zoom + zy;
              if (ix >= 0 && iy >= 0 && ix < pWindow->width && iy < pWindow->height)
                pWindow->pixels[ix + iy * pWindow->width] = tileColors[warrior.cells[x + y * warrior.size]];
            }
      warrior.Step();
    }
  }
  void SpawnWarrior(int x = 0, int y = 0, int size = 64)
  {
    warriors.emplace_back();
  }

  std::vector<WireWarrior> warriors;

private:

};


void WireWars()
{
  Window window("WireWars", false, 800, 600, false);
  WireWar game;
  game.SpawnWarrior();
  while (Controls::Update())
  {
    window.Clear();

    if (Controls::KeyDown(SDL_SCANCODE_1))
      for (auto & warrior : game.warriors)
      {
        memset(warrior.cells.data(), 0, sizeof(warrior.cells[0]) * warrior.cells.size());
        for (int64_t i = 0; i < 10; i++)
          warrior.MutationAdd(1);

        for (int64_t i = 0; i < 2; i++)
          warrior.MutationAdd(2);
      }

    game.Draw(&window, 4);

    window.Swap();
    SDL_Delay(50);
  }

}

#endif // WireWars_h__
