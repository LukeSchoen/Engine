#ifndef Octree_h__
#define Octree_h__
#include "nodeList.h"
#include "PointCloud.h"
#include "maths.h"

struct Octree
{
public:
  Octree(const char *octFile);

  Octree(int8_t treeDepth, uint32_t color = 0xFFFFFFFF);

  Octree(PointCloud &pc);

  NodeList nodes;

  Node RootNode();

  uint8_t Depth();

  void Split(Node node);

  bool IsLeaf(Node node) const;

  Node GetChild(Node node, int childIndex) const;

  Node GetChild(Node node, int childIndex, vec3 *position, int parentLayer) const;

  void SetChild(Node node, Node child);

  bool GetSolid(Node node);
  void SetSolid(Node node, bool solidity);

  uint32_t GetColor(Node node) const;
  void SetColor(Node node, uint32_t color);

  void AddVoxel(uint32_t Color, int Xpos, int Ypos, int Zpos);

  void SaveFile(const char *filePath);

  void LoadFile(const char *filePath);
  
private:
  int8_t depth;
  Node root;
};

#endif // Octree_h__
