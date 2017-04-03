#ifndef FastPack_h__
#define FastPack_h__

#include "maths.h"
#include "RectPacker.h"

struct Node;

struct FastPacker
{
  FastPacker(int width = 1024, int height = 1024);

  bool InsertRect(Rect &rect);

  void Clear();

private:
  Node *rootNode;

};
#endif // FastPack_h__
