#ifndef EntityHandler_h__
#define EntityHandler_h__

#include <vector>
#include "Entities.h"

class EntityHandler
{
public:
  int64_t Add(Entity entity);

  Entity* Get(int64_t entityID);

  void Draw(mat4 vp);

  void Update();

private:
  int64_t nextUID = 0;
  std::vector<Entity> entities;
};

#endif // EntityHandler_h__
