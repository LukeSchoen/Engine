#include "EntityHandler.h"

int64_t EntityHandler::Add(Entity entity)
{
  entity.uniqueID = nextUID;
  entities.push_back(entity);
  return nextUID++;
}

Entity* EntityHandler::Get(int64_t entityID)
{
  for (int64_t e = 0; e < entities.size(); e++)
    if (entities[e].uniqueID == entityID)
      return &entities[e];
  return nullptr;
}

void EntityHandler::Draw(mat4 vp)
{
  for (auto entity : entities)
    entity.Draw(vp);
}

void EntityHandler::Update()
{
  for (auto &entity : entities) entity.Update();

  // Move entities apart
  for (int i = 0; i < entities.size() - 1; i++)
    for (int j = i + 1; j < entities.size(); j++)
    {
      vec3 ItoJ = entities[j].position - entities[i].position;
      if ((ItoJ).Length() < 1)
      {
        float amt = 0.01 / (ItoJ).Length();
        ItoJ.y = 0;
        ItoJ = ItoJ.Normalized();
        ItoJ = ItoJ * amt;
        entities[j].Move(ItoJ);
        entities[i].Move(vec3() - ItoJ);
      }
    }
}

