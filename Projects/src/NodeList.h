#ifndef NodeList_h__
#define NodeList_h__

#include "stdint.h"

typedef uint32_t Node;

struct StreamFileWriter;
struct StreamFileReader;

struct NodeList
{
public:
  uint32_t NodeCount = 0;
  uint32_t *Colors = nullptr;
  Node *Children = nullptr;
  Node CreateNode(Node child, uint32_t color);
  void DeleteNode(Node node);
  void SaveFile(StreamFileWriter *stream);
  void LoadFile(StreamFileReader *stream);
private:
  Node *FreeNodes = nullptr;
  uint32_t FreeNodeCount = 0;
  uint32_t FreeNodeHead = 0;
  uint32_t MaxNodeCount = 0;
  void Reallocate(uint32_t newSize);
};

#endif // NodeList_h__
