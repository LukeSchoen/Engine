#include "RenderObject.h"
#include "MolecualrDynamicsSimulator.h"

class ParticleSimulator
{
public:

  ParticleSimulator();

  void Update(int steps = 1);

  void Render(const mat4 & MVP);

  void AddPoint(vec3 pos, vec3 col);

  MolecularDynamicsSimulator world;
  RenderObject mesh;
  RenderObject debugLineMesh;
};