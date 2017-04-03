#include "NodeList.h"
#include "Maths.h"
#include <malloc.h>
#include <stdio.h>

void NodeList::Reallocate(uint32_t newMaxNodeCount)
{
  if (newMaxNodeCount > MaxNodeCount)
  {
    Children = (Node*)realloc(Children, newMaxNodeCount * sizeof(Node));
    Colors = (Color*)realloc(Colors, newMaxNodeCount * sizeof(Color));
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

Node NodeList::CreateNode(Node child, Color color)
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

void NodeList::SaveFile(const char* filePath)
{
  FILE* f = fopen(filePath, "wb");
  if (!f) return;
  //Header
  fwrite(&NodeCount, sizeof(NodeCount), 1, f);
  fwrite(&MaxNodeCount, sizeof(MaxNodeCount), 1, f);
  fwrite(&FreeNodeHead, sizeof(FreeNodeHead), 1, f);
  fwrite(&FreeNodeCount, sizeof(FreeNodeCount), 1, f);
  //Data
  fwrite(Colors, sizeof(Color), MaxNodeCount, f);
  fwrite(Children, sizeof(Node), MaxNodeCount, f);
  fwrite(FreeNodes, sizeof(Node), MaxNodeCount, f);
  fclose(f);
}

void NodeList::LoadFile(const char* filePath)
{
  FILE* f = fopen(filePath, "rb");
  if (!f) return;
  //Header
  fread(&NodeCount, sizeof(NodeCount), 1, f);
  fread(&MaxNodeCount, sizeof(MaxNodeCount), 1, f);
  fread(&FreeNodeHead, sizeof(FreeNodeHead), 1, f);
  fread(&FreeNodeCount, sizeof(FreeNodeCount), 1, f);

  Children = (Node*)realloc(Children, MaxNodeCount * sizeof(Node));
  Colors = (Color*)realloc(Colors, MaxNodeCount * sizeof(Color));
  FreeNodes = (Node*)realloc(FreeNodes, MaxNodeCount * sizeof(Node));

  //Data
  fread(Colors, sizeof(Color), MaxNodeCount, f);
  fread(Children, sizeof(Node), MaxNodeCount, f);
  fread(FreeNodes, sizeof(Node), MaxNodeCount, f);
  fclose(f);
}
