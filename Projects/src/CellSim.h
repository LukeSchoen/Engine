#ifndef Cells_h__
#define Cells_h__

#include <vector>
#include "Window.h"
#include "Controls.h"
#include "FrameRate.h"

class CellSim
{
public:
  enum CellAction : uint8_t
  {
    CA_Stay,
    CA_Heal,
    CA_GrowUp,
    CA_GrowDown,
    CA_GrowLeft,
    CA_GrowRight,
    CA_MoveUp,
    CA_MoveDown,
    CA_MoveLeft,
    CA_MoveRight,
    CA_SmakUp,
    CA_SmakDown,
    CA_SmakLeft,
    CA_SmakRight,
    CA_NUM
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
    static int nuke = 0;
    nuke++;
    if (nuke % 32 == 0)
    {
      int sx = rand() % (m_dimensions.x + 64) - 64;
      int sy = rand() % (m_dimensions.y + 64) - 64;
      for (int y = Max(sy, 0); y < Min(m_dimensions.y - 1, sy + 64); y++)
        for (int x = Max(sx, 0); x < Min(m_dimensions.x - 1, sx + 64); x++)
        {
          auto pcell = m_cells[x + y * m_dimensions.x];
          if (pcell)
            pcell->m_health = 0;
        }
    }

    // Step All
    //for (int y = 0; y < m_dimensions.y; y++) for (int x = 0; x < m_dimensions.x; x++) m_cells[x + y * m_dimensions.x]->Step();
    //m_cells[x + y * m_dimensions.x]->Step();

    // Add new cells
    if (rand() < 200)
      for (int i = 0; i < 1; i++)
      {
        vec2ui cellPos = { rand() % m_dimensions.x, rand() % m_dimensions.y };
        auto ppCell = &m_cells[cellPos.x + cellPos.y * m_dimensions.x];
        (*ppCell)->m_health = 1;
        (*ppCell)->m_brain.Generate();
        uint8_t *rgb = (uint8_t*)(&(*ppCell)->m_fullColor);
        float r = rand() % 255;
        float g = rand() % 255;
        float b = rand() % 255;
        (*ppCell)->m_fullColor = uint32_t(r) + (uint32_t(g) << 8) + (uint32_t(b) << 16);
      }

    // Step one at random
    for (int i = 0; i < 50000; i++)
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
        float bright = ((*ppCell)->m_health / 10.f) * 0.5f + 0.5f;
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
    Cell() {}
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
      m_brain.Mutate();
      uint8_t *rgb = (uint8_t*)(&m_fullColor);
      float r = rgb[0];
      float g = rgb[1];
      float b = rgb[2];
      r += (1 - rand() % 3) * 4;
      g += (1 - rand() % 3) * 4;
      b += (1 - rand() % 3) * 4;
      r = Min(Max(r, 0), 255);
      g = Min(Max(g, 0), 255);
      b = Min(Max(b, 0), 255);
      m_fullColor = uint32_t(r) + (uint32_t(g) << 8) + (uint32_t(b) << 16);
    }

    class Brain
    {
    public:
      Brain() {}
      ~Brain() {}

      CellAction Think()
      {
        // Generate Ideas
        if (ideas.size() == 0)
          Generate();

        // Select an idea
        if (activeIdea == -1)
          activeIdea = rand() % ideas.size();
        Idea &idea = ideas[activeIdea];
        CellAction ret = CellAction(idea.m_steps[idea.m_step]);
        if (idea.m_step++ >= idea.m_steps.size() - 1) { idea.m_step = 0; activeIdea = -1; }
        return ret;
      }

      void Generate()
      {
        ideas.clear();
        for (int p = 0; p < 5; p++) // Programs
          ideas.push_back(Idea().Generate());
      }

      void Mutate()
      {
        ideas.erase(ideas.begin() + rand() % ideas.size());
        ideas.push_back(Idea().Generate());
      }

    private:

      class Idea
      {
      public:
        Idea() {}
        ~Idea() {}

        Idea& Generate(int length = 5)
        {
          for (int i = 0; i < (rand() % (length - 1)) + 1; i++) // Instructions
            m_steps.push_back(CellAction(rand() % CA_NUM));
          return *this;
        }

        //private:
        std::vector<CellAction> m_steps;
        uint32_t m_step = 0;
      };

      uint32_t activeIdea = -1;
      std::vector<Idea> ideas;

    };

    //private:

    Brain m_brain;
    uint32_t m_color = 0;
    uint32_t m_fullColor = 0;
    uint32_t m_health = 0;
    //uint32_t m_energy = 0;
  };

private:

  void PerformAction(Cell *pCell, const vec2ui &pos, CellAction action)
  {
    static Cell empty;
    Cell *pEmpty = &empty;
    Cell **ppUp = pos.y > 0 ? &m_cells[pos.x + (pos.y - 1) * m_dimensions.x] : &pEmpty;
    Cell **ppDn = pos.y < m_dimensions.y - 1 ? &m_cells[pos.x + (pos.y + 1) * m_dimensions.x] : &pEmpty;
    Cell **ppLt = pos.x > 0 ? &m_cells[(pos.x - 1) + pos.y * m_dimensions.x] : &pEmpty;
    Cell **ppRt = pos.x < m_dimensions.x - 1 ? &m_cells[(pos.x + 1) + pos.y * m_dimensions.x] : &pEmpty;
    switch (action)
    {

    case CA_Stay: { /*m_energy += 1;*/ break; }

    case CA_Heal: { pCell->m_health = Min(pCell->m_health + 1, 10); break; }

    case CA_GrowUp: { if ((*ppUp)->m_health == 0 && pCell->m_health >= 10) { pCell->m_health = 1; **ppUp = *pCell; (*ppUp)->Mutate(); } break; }
    case CA_GrowDown: { if ((*ppDn)->m_health == 0 && pCell->m_health >= 10) { pCell->m_health = 1; **ppDn = *pCell; (*ppDn)->Mutate(); } break; }
    case CA_GrowLeft: { if ((*ppLt)->m_health == 0 && pCell->m_health >= 10) { pCell->m_health = 1; **ppLt = *pCell; (*ppLt)->Mutate(); } break; }
    case CA_GrowRight: { if ((*ppRt)->m_health == 0 && pCell->m_health >= 10) { pCell->m_health = 1; **ppRt = *pCell; (*ppRt)->Mutate(); } break; }

    case CA_MoveUp: { if ((*ppUp)->m_health == 0) { **ppUp = *pCell; pCell->m_health = 0; } break; }
    case CA_MoveDown: { if ((*ppDn)->m_health == 0) { **ppDn = *pCell; pCell->m_health = 0; } break; }
    case CA_MoveLeft: { if ((*ppLt)->m_health == 0) { **ppLt = *pCell; pCell->m_health = 0; } break; }
    case CA_MoveRight: { if ((*ppRt)->m_health == 0) { **ppRt = *pCell; pCell->m_health = 0; } break; }

    case CA_SmakUp: { if ((*ppUp)->m_health > 0) (*ppUp)->m_health = 0; break; }
    case CA_SmakDown: { if ((*ppDn)->m_health > 0) (*ppDn)->m_health = 0; break; }
    case CA_SmakLeft: { if ((*ppLt)->m_health > 0) (*ppLt)->m_health = 0; break; }
    case CA_SmakRight: { if ((*ppRt)->m_health > 0) (*ppRt)->m_health = 0; break; }

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
  Window window("CellSim", false, 1200, 600);
  CellSim simulation(&window, 2);

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
