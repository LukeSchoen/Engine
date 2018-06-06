#include "BattleEvolution.h"
#include "Window.h"
#include "Controls.h"

class BattleEvolutionSim
{
public:
  BattleEvolutionSim();
  ~BattleEvolutionSim();

private:

};


BattleEvolutionSim::BattleEvolutionSim()
{
   
}

BattleEvolutionSim::~BattleEvolutionSim()
{

}



void BattleEvoltion()
{
  Window window;

  BattleEvolutionSim evoSim;

  while (Controls::Update())
  {

    window.Clear();

    evoSim;

    window.Swap();


  }
}
