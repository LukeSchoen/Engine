#include "Octree.h"
#include "Maths.h"
#include <limits.h>
#include "StreamFile.h"

void RecursiveAddVoxel(Octree &tree, Node node, uint32_t color, int targetX, int targetY, int targetZ, int curX, int curY, int curZ, int targetLayer, int curLayer)
{
  // Reached Leaves
  if (curLayer == targetLayer)
  {
    tree.SetSolid(node, true);
    tree.SetColor(node, color);
    return;
  }

  // Descend Toward Leaves
  ++curLayer;

  // Determine Next Step Of The Traversal
  int midsize = 1 << (targetLayer - curLayer);
  int midX = curX + midsize;
  int midY = curY + midsize;
  int midZ = curZ + midsize;
  int child = (targetX >= midX) + (targetY >= midY) * 2 + (targetZ >= midZ) * 4;

  // The Next Node Does Not Yet Exist?
  if (tree.IsLeaf(node))
  {
    // Split Parent Node To Continue Descent
    tree.Split(node);

    // Initialize Child Nodes To Transparent
    for (int Child = 0; Child < 8; ++Child)
      tree.SetSolid(tree.GetChild(node, Child), false);

    // Write in LOD node data
    tree.SetSolid(node, true);
    tree.SetColor(node, color);
  }

  // Get Next Node Of Traversal
  node = tree.GetChild(node, child);

  // Write in this nodes data
  tree.SetSolid(node, true);
  tree.SetColor(node, color);

  // Recursive Tail Call Descent
  return RecursiveAddVoxel(tree, node, color, targetX, targetY, targetZ, curX + midsize * (targetX >= midX), curY + midsize * (targetY >= midY), curZ + midsize * (targetZ >= midZ), targetLayer, curLayer);
};

Octree::Octree(int8_t treeDepth, uint32_t color)
{
  depth = treeDepth;
  root = nodes.CreateNode(0, color);
}

Octree::Octree(PointCloud &pc) 
{
  if (pc.pointCount == 0) return;

  // Calculate PointCloud Extents
  vec3i min(INT_MAX, INT_MAX, INT_MAX);
  vec3i max(INT_MIN, INT_MIN, INT_MIN);
  for (int64_t pItr = 0; pItr < pc.pointCount; pItr++)
  {
    Point &p = pc.points[pItr];
    min.x = Min(min.x, p.x);
    min.y = Min(min.y, p.y);
    min.z = Min(min.z, p.z);
    max.x = Max(max.x, p.x);
    max.y = Max(max.y, p.y);
    max.z = Max(max.z, p.z);
  }

  // Calculate PointClouds Longest Side
  int longestSide = Max(Max(Max(max.x - min.x, max.y - min.y), max.z - min.z), 0);

  // Calculate Bounding Octree
  depth = 0;
  while (longestSide > (1 << depth)) depth++;

  // Create Root
  root = nodes.CreateNode(0, pc.points[0].color);

  // Populate Octree With Points
  for (int64_t pItr = 0; pItr < pc.pointCount; pItr++)
  {
    Point &p = pc.points[pItr];
    AddVoxel(p.color, p.x - min.x, p.y - min.y, p.z - min.z);
  }
}

Octree::Octree(const char *octFile)
{
  LoadFile(octFile);
}

Node Octree::RootNode()
{
  return root;
}

uint8_t Octree::Depth()
{
  return depth;
}

void Octree::Split(Node node)
{
  if (IsLeaf(node))
  {
    Node child = nodes.CreateNode(0, GetColor(node));
    SetChild(node, child);
    for (int i = 0; i < 7; i++)
      nodes.CreateNode(0, GetColor(node));
  }
}

bool Octree::IsLeaf(Node node) const
{
  return nodes.Children[node] == 0;
}

Node Octree::GetChild(Node node, int childIndex) const
{
  Node child = nodes.Children[node];
  if (child == 0)
    return node;
  else
    return child + childIndex;
}

Node Octree::GetChild(Node node, int childIndex, vec3 *position, int layer) const
{
  Node child = nodes.Children[node];
  if (child == 0)
    return node;
  else
  {
    float layerSize = 1.0f / (1LL << layer);
    uint8_t xoff = childIndex & 1;
    uint8_t yoff = (childIndex >> 1) & 1;
    uint8_t zoff = (childIndex >> 2) & 1;
    *position += vec3(xoff * layerSize, yoff * layerSize, zoff * layerSize);
    return child + childIndex;
  }
}

uint32_t Octree::GetColor(Node node) const
{
  return nodes.Colors[node];
}

void Octree::SetChild(Node node, Node child)
{
  nodes.Children[node] = child;
}

void Octree::SetColor(Node node, uint32_t color)
{
  nodes.Colors[node] = color;
}

bool Octree::GetSolid(Node node)
{
  return GetColor(node) > 0;
}

void Octree::SetSolid(Node node, bool solidity)
{
  if (solidity)
    SetColor(node, 0xFFFFFF);
  else
    SetColor(node, 0);
}

void Octree::AddVoxel(uint32_t Color, int Xpos, int Ypos, int Zpos)
{
  RecursiveAddVoxel(*this, RootNode(), Color, Xpos, Ypos, Zpos, 0, 0, 0, depth, 0);
}

void Octree::SaveFile(const char* filePath)
{
  nodes.SaveFile(filePath);
}

void Octree::LoadFile(const char* filePath)
{
  nodes.LoadFile(filePath);
  root = 0;
}

//void Octree::SaveFile(const char* filePath)
//{
//  StreamFileWriter stream(filePath);
//  //stream.WriteBytes(&depth, sizeof(depth));
//  nodes.SaveFile(&stream);
//}
//
//void Octree::LoadFile(const char* filePath)
//{
//  StreamFileReader stream(filePath);
//  //stream.ReadBytes(&depth, sizeof(depth));
//  nodes.LoadFile(&stream);
//  root = 0;
//}
