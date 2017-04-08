#include "NodeList.h"
#include "Maths.h"
#include <malloc.h>
#include <stdio.h>
#include "StreamFile.h"

void NodeList::Reallocate(uint32_t newMaxNodeCount)
{
  if (newMaxNodeCount > MaxNodeCount)
  {
    Children = (Node*)realloc(Children, newMaxNodeCount * sizeof(Node));
    Colors = (uint32_t*)realloc(Colors, newMaxNodeCount * sizeof(uint32_t));
    FreeNodes = (Node*)realloc(FreeNodes, newMaxNodeCount * sizeof(Node));
    // Add these newly created nodes to the free node list
    for (uint32_t i = MaxNodeCount; i < newMaxNodeCount; i++)
    {
      FreeNodes[i] = i;
      Children[i] = 0;
    }
    MaxNodeCount = newMaxNodeCount;
    FreeNodeCount = MaxNodeCount - NodeCount;
  }
}

Node NodeList::CreateNode(Node child, uint32_t color)
{
  if (NodeCount + 16 >= MaxNodeCount) // check if we need more space
  {
    uint32_t newCount = Max(MaxNodeCount, 32768) * 2;
    Reallocate(newCount);
  }
  // Add node to list
  uint32_t location = FreeNodes[FreeNodeHead++];
  Children[location] = child;
  Colors[location] = color;
  FreeNodeCount--;
  NodeCount++;
  return location;
}

void NodeList::DeleteNode(Node location)
{
  // Return node to free list
  FreeNodes[--FreeNodeHead] = location;
  FreeNodeCount++;
  NodeCount--;
}

void NodeList::SaveFile(StreamFileWriter *stream)
{
  //Header
  stream->WriteBytes(&NodeCount, sizeof(NodeCount));
  stream->WriteBytes(&MaxNodeCount, sizeof(MaxNodeCount));
  stream->WriteBytes(&FreeNodeHead, sizeof(FreeNodeHead));
  stream->WriteBytes(&FreeNodeCount, sizeof(FreeNodeCount));
  //Data
  stream->WriteBytes(Colors, sizeof(uint32_t) * MaxNodeCount);
  stream->WriteBytes(Children, sizeof(Node) * MaxNodeCount);
  stream->WriteBytes(FreeNodes, sizeof(Node) * MaxNodeCount);
}

void NodeList::LoadFile(StreamFileReader *stream)
{
  //Header
  stream->ReadBytes(&NodeCount, sizeof(NodeCount));
  stream->ReadBytes(&MaxNodeCount, sizeof(MaxNodeCount));
  stream->ReadBytes(&FreeNodeHead, sizeof(FreeNodeHead));
  stream->ReadBytes(&FreeNodeCount, sizeof(FreeNodeCount));

  Colors = (uint32_t*)realloc(Colors, MaxNodeCount * sizeof(uint32_t));
  Children = (Node*)realloc(Children, MaxNodeCount * sizeof(Node));
  FreeNodes = (Node*)realloc(FreeNodes, MaxNodeCount * sizeof(Node));

  //Data
  stream->ReadBytes(Colors, sizeof(uint32_t) * MaxNodeCount);
  stream->ReadBytes(Children, sizeof(Node) * MaxNodeCount);
  stream->ReadBytes(FreeNodes, sizeof(Node) * MaxNodeCount);
}
