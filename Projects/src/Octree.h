#ifndef Octree_h__
#define Octree_h__
#include "nodeList.h"
#include "PointCloud.h"


struct Octree
{
public:
  Octree(const char *octFile);

  Octree(int8_t treeDepth, Color color = 0xFFFFFFFF);

  Octree(PointCloud &pc);

  NodeList nodes;

  Node RootNode();

  void Split(Node node);

  bool IsLeaf(Node node) const;

  Node GetChild(Node node, int childIndex) const;
  void SetChild(Node node, Node child);

  bool GetSolid(Node node);
  void SetSolid(Node node, bool solidity);

  Color GetColor(Node node) const;
  void SetColor(Node node, Color color);

  void AddVoxel(uint32_t Color, int Xpos, int Ypos, int Zpos);

  void SaveFile(const char* filePath);

  void LoadFile(const char* filePath);

private:
  int8_t depth;
  Node root;
};

#endif // Octree_h__
