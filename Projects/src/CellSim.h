#ifndef Cells_h__
#define Cells_h__

#include <vector>
#include "Window.h"
#include "Controls.h"
#include "FrameRate.h"

const static float requiredGrowthHealth = 25;

class CellSim
{
public:
  enum CellAction : uint8_t
  {
    CA_Stay,
    CA_Heal,
    CA_Grow,
    CA_TurnLeft,
    CA_TurnRight,
    CA_TurnAround,
    CA_Move,
    CA_Smak,
    CA_ValueSetOnA,
    CA_ValueSetOnB,
    CA_ValueSetOnC,
    CA_ValueSetOoffA,
    CA_ValueSetOoffB,
    CA_ValueSetOoffC,
    CA_DoIfFree,
    CA_DoIfNotFree,
    CA_DoIfBro,
    CA_DoIfNotBro,
    CA_DoIfValueIsOnA,
    CA_DoIfValueIsOnB,
    CA_DoIfValueIsOnC,
    CA_DoIfValueIsOffA,
    CA_DoIfValueIsOffB,
    CA_DoIfValueIsOffC,
    CA_NUM
  };

  enum CellCondition : uint8_t
  {

  };

  CellSim(Window *pWindow, int zoomOut = 1) : m_pWindow(pWindow), m_zoomOut(zoomOut)
  {
    m_dimensions = { pWindow->width / zoomOut , pWindow->height / zoomOut };
    m_cells.resize(m_dimensions.x * m_dimensions.y);
    for (auto & pCell : m_cells) { pCell = new Cell(); /*pCell->m_color = rand()*/; }
  }
  ~CellSim() { }

  void Draw()
  {
    for (int y = 0; y < m_dimensions.y; y++) for (int x = 0; x < m_dimensions.x; x++) for (int iy = 0; iy < m_zoomOut; iy++) for (int ix = 0; ix < m_zoomOut; ix++)
      m_pWindow->pixels[(x * m_zoomOut + ix) + (y * m_zoomOut + iy) * m_pWindow->width] = m_cells[x + y * m_dimensions.x]->Draw();
  }

  void Step()
  {
    // Nukes
    static int nuke = 0;
    nuke++;
    if(false)
    if (nuke % 500 == 0)
    {
      int sx = rand() % (m_dimensions.x + 64) - 64;
      int sy = rand() % (m_dimensions.y + 64) - 64;
      for (int y = Max(sy, 0); y < Min(m_dimensions.y, sy + 64); y++)
        for (int x = Max(sx, 0); x < Min(m_dimensions.x, sx + 64); x++)
        {
          auto pcell = m_cells[x + y * m_dimensions.x];
          if (pcell)
            pcell->m_health = 0;
        }
    }

    // Pioneers
    static int pioneer = 0;
    pioneer++;
    if(true)
    if (pioneer % 4000 == 0)
      for (int y = 0; y < m_dimensions.y; y++)
        for (int x = 0; x < m_dimensions.x; x++)
        {
          if(rand() > RAND_MAX  * 0.2)
            m_cells[x + y * m_dimensions.x]->m_health = 0;
          else
            if (m_cells[x + y * m_dimensions.x]->m_health > 0)
              m_cells[x + y * m_dimensions.x]->Mutate();
        }

    // Neutrinos
    if (true)
      for (int y = 0; y < m_dimensions.y; y++)
        for (int x = 0; x < m_dimensions.x; x++)
          if(m_cells[x + y * m_dimensions.x]->m_health > 0)
            if (rand() > RAND_MAX - 15)
              m_cells[x + y * m_dimensions.x]->Mutate();


    // Step All
    //for (int y = 0; y < m_dimensions.y; y++) for (int x = 0; x < m_dimensions.x; x++) m_cells[x + y * m_dimensions.x]->Step();
    //m_cells[x + y * m_dimensions.x]->Step();

    // Add new cells
    if (rand() < 25000)
      for (int i = 0; i < 1; i++)
      {
        vec2ui cellPos = { rand() % m_dimensions.x, rand() % m_dimensions.y };
        auto ppCell = &m_cells[cellPos.x + cellPos.y * m_dimensions.x];
        if ((*ppCell)->m_health == 0)
        {
          (*ppCell)->m_health = 1;
          (*ppCell)->m_brain.Generate();
          uint8_t *rgb = (uint8_t*)(&(*ppCell)->m_fullColor);
          float r = rand() % 255;
          float g = rand() % 255;
          float b = rand() % 255;
          (*ppCell)->m_fullColor = uint32_t(r) + (uint32_t(g) << 8) + (uint32_t(b) << 16);
        }
      }

    // Step one at random
    int pixels = 40000 / (m_pWindow->width / m_zoomOut) * 40000 / (m_pWindow->height / m_zoomOut);
    for (int i = 0; i < pixels; i++)
    {
      vec2ui cellPos = { rand() % m_dimensions.x, rand() % m_dimensions.y };
      auto ppCell = &m_cells[cellPos.x + cellPos.y * m_dimensions.x];
      if ((*ppCell)->m_health > 0)
      {
        auto action = (*ppCell)->Step();
        PerformAction(*ppCell, cellPos, action);
        uint32_t col = (*ppCell)->m_fullColor;
        uint8_t *rgb = (uint8_t*)(&col);
        float r = rgb[0] / 255.f;
        float g = rgb[1] / 255.f;
        float b = rgb[2] / 255.f;
        float bright = ((*ppCell)->m_health / requiredGrowthHealth) * 0.5f + 0.5f;
        r *= bright;
        g *= bright;
        b *= bright;
        col = uint32_t(r * 255.f) + (uint32_t(g * 255.f) << 8) + (uint32_t(b * 255.f) << 16);
        (*ppCell)->m_color = col;
      }
      else
        (*ppCell)->m_color = 0;
    }
  }

  class Cell
  {
  public:
    Cell() { m_direction = rand() % 4; }
    ~Cell() {}

    CellAction Step()
    {
      // Look around at nearby cells
      // color output
      // genome similarity?

      // Listen to the signals of nearby cells
      // Direct messages from touching cells
      // Messages broadcast from distant cells

      // know information about current cell
      // Health
      // Energy
      // Etc..

      // choose action based on said information

      // action = Brain.think()
      // Body.Do(action)

      return m_brain.Think();
    }
    uint32_t Draw() { return m_color; }
    void Mutate()
    {
      bool willMutate = (rand() & 2);
      int len = (rand() % 8);
      if(willMutate)
        for (int i = 0; i < len; i++)
        {
          m_brain.Mutate();
          bool colWillMutate = (rand() & 2) * (rand() & 2);
          colWillMutate = true;
          if (colWillMutate)
          {
            uint8_t *rgb = (uint8_t*)(&m_fullColor);
            float r = rgb[0];
            float g = rgb[1];
            float b = rgb[2];
            r += (1 - rand() % 3) * 8;
            g += (1 - rand() % 3) * 8;
            b += (1 - rand() % 3) * 8;
            r = Min(Max(r, 0), 255);
            g = Min(Max(g, 0), 255);
            b = Min(Max(b, 0), 255);
            m_fullColor = uint32_t(r) + (uint32_t(g) << 8) + (uint32_t(b) << 16);
          }
        }
    }

    class Brain
    {
    public:
      Brain() {}
      ~Brain() {}

      CellAction Think()
      {
        if (ideas.size() == 0) Generate();
        if (activeIdea >= ideas.size()) activeIdea = 0;
        Idea &idea = ideas[activeIdea];
        CellAction ret = CellAction(idea.m_steps[idea.m_step]);
        if (++idea.m_step >= idea.m_steps.size()) { idea.m_step = 0; ++activeIdea; }
        if (activeIdea >= ideas.size()) activeIdea = 0;
        return ret;
      }

      void SkipThought()
      {
        ideas[activeIdea++].m_step = 0;
        if (activeIdea >= ideas.size()) activeIdea = 0;
      }

      void Generate()
      {
        ideas.clear();
        int len = rand() % 32 + 2;
        for (int p = 0; p < len; p++) // Programs
          ideas.push_back(Idea().Generate());
      }

      void Mutate()
      {
        if (rand() % 2) ideas.erase(ideas.begin() + rand() % ideas.size());
        if (rand() % 2) ideas.push_back(Idea().Generate());
        while(ideas.size() < 2) ideas.push_back(Idea().Generate());
      }

    private:

      class Idea
      {
      public:
        Idea() {}
        ~Idea() {}

        Idea& Generate(int length = 32)
        {
          int len = rand() % length + 2;
          for (int i = 0; i < len; i++) // Instructions
            m_steps.push_back(CellAction(rand() % CA_NUM));
          return *this;
        }

        //private:
        std::vector<CellAction> m_steps;
        uint32_t m_step = 0;
      };

      uint32_t activeIdea = 0;
      std::vector<Idea> ideas;
    };

    //private:

    Brain m_brain;
    bool a = false;
    bool b = false;
    bool c = false;

    int8_t m_direction = 0;
    uint32_t m_color = 0;
    uint32_t m_fullColor = 0;
    uint32_t m_health = 0;
    bool m_inBounds = true;
    //uint32_t m_energy = 0;
  };

private:

  void PerformAction(Cell *pCell, const vec2ui &pos, CellAction action)
  {
    static Cell empty;
    empty.m_inBounds = false;
    Cell *pEmpty = &empty;
    Cell **ppUp = pos.y > 0 ? &m_cells[pos.x + (pos.y - 1) * m_dimensions.x] : &pEmpty;
    Cell **ppDn = pos.y < m_dimensions.y - 1 ? &m_cells[pos.x + (pos.y + 1) * m_dimensions.x] : &pEmpty;
    Cell **ppLt = pos.x > 0 ? &m_cells[(pos.x - 1) + pos.y * m_dimensions.x] : &pEmpty;
    Cell **ppRt = pos.x < m_dimensions.x - 1 ? &m_cells[(pos.x + 1) + pos.y * m_dimensions.x] : &pEmpty;
    Cell **ppFr = nullptr;
    switch (pCell->m_direction)
    {
    case 0: ppFr = ppRt; break;
    case 1: ppFr = ppUp; break;
    case 2: ppFr = ppLt; break;
    case 3: ppFr = ppDn; break;
    }

    switch (action)
    {

    case CA_Stay: { /*m_energy += 1;*/ break; }
    case CA_Heal: { pCell->m_health = Min(pCell->m_health + 1, requiredGrowthHealth); break; }
    case CA_TurnLeft: { --pCell->m_direction; if (pCell->m_direction == -1) pCell->m_direction = 3; }
    case CA_TurnRight: { ++pCell->m_direction; if (pCell->m_direction == 4) pCell->m_direction = 0; }
    case CA_TurnAround: { pCell->m_direction += 2; if (pCell->m_direction >= 4) pCell->m_direction -= 4; }
    case CA_Grow: { if ((*ppFr)->m_health == 0 && (*ppFr)->m_inBounds && pCell->m_health >= requiredGrowthHealth) { pCell->m_health = 1; **ppFr = *pCell; (*ppFr)->Mutate(); } break; }
    case CA_Move: { if ((*ppFr)->m_health == 0 && (*ppFr)->m_inBounds) { **ppFr = *pCell; pCell->m_health = 0; } break; }
    case CA_Smak: { if ((*ppFr)->m_health > 0) (*ppFr)->m_health = 0; break; }
    case CA_ValueSetOnA: { pCell->a = true; }
    case CA_ValueSetOnB: { pCell->b = true; }
    case CA_ValueSetOnC: { pCell->c = true; }
    case CA_ValueSetOoffA: { pCell->a = false; }
    case CA_ValueSetOoffB: { pCell->b = false; }
    case CA_ValueSetOoffC: { pCell->c = false; }

    // Continue if true type actions

    case CA_DoIfFree: { if ((*ppFr)->m_health) pCell->m_brain.SkipThought(); break; }
    case CA_DoIfNotFree: { if (!(*ppFr)->m_health) pCell->m_brain.SkipThought(); break; }
    case CA_DoIfBro: { if ((*ppFr)->m_fullColor != pCell->m_fullColor) pCell->m_brain.SkipThought(); break; }
    case CA_DoIfNotBro: { if ((*ppFr)->m_fullColor == pCell->m_fullColor) pCell->m_brain.SkipThought(); break; }

    case CA_DoIfValueIsOnA: { if (!pCell->a) pCell->m_brain.SkipThought(); break; }
    case CA_DoIfValueIsOnB: { if (!pCell->b) pCell->m_brain.SkipThought(); break; }
    case CA_DoIfValueIsOnC: { if (!pCell->c) pCell->m_brain.SkipThought(); break; }
    case CA_DoIfValueIsOffA: { if (pCell->a) pCell->m_brain.SkipThought(); break; }
    case CA_DoIfValueIsOffB: { if (pCell->b) pCell->m_brain.SkipThought(); break; }
    case CA_DoIfValueIsOffC: { if (pCell->c) pCell->m_brain.SkipThought(); break; }

    default:
      break;
    }
  }

  std::vector<Cell*> m_cells;
  vec2ui m_dimensions;
  Window *m_pWindow;
  uint32_t m_zoomOut;

};

void CellSimulator()
{
  int seed = 456;
  for (int i = 0; i < seed; i++) rand();

  Window window("CellSim", false, 1200, 600);
  CellSim simulation(&window, 5);

  while (Controls::Update())
  {
    window.Clear();
    simulation.Draw();
    window.Swap();
    simulation.Step();
    FrameRate::Update();
  }
}

#endif // Cells_h__
