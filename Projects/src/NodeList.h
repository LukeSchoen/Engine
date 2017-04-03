#ifndef NodeList_h__
#define NodeList_h__

#include "stdint.h"

typedef uint32_t Node;
typedef uint32_t Color;

struct NodeList
{
public:
  uint32_t NodeCount = 0;
  Color *Colors = nullptr;
  Node *Children = nullptr;
  Node CreateNode(Node child, Color color);
  void DeleteNode(Node node);
  void SaveFile(const char* filePath);
  void LoadFile(const char* filePath);
private:
  Node *FreeNodes = nullptr;
  uint32_t FreeNodeCount = 0;
  uint32_t FreeNodeHead = 0;
  uint32_t MaxNodeCount = 0;
  void Reallocate(uint32_t newSize);
};

#endif // NodeList_h__
