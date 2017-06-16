#include "FastPack.h"
#include <stdlib.h>

Node *_FastNewNode(int width, int height, int xpos, int ypos, Node *parent);

void _passUpBestRect(Node *node);

int steps = 0;
int avg = 0;
int all = 0;

struct Node
{
  Node(int width, int height, int xpos, int ypos, Node *_parent) : myRect(width, height, 0, xpos, ypos), parent(_parent) {}

  Node *AddRect(Rect rect)
  {
    //while (left && left->left && left->left->full) left = left->right; // Cull left Tree!
    //while (right && right->left && right->left->full) right = right->right; // Cull right Tree!

    steps++;
    if (full || rect.width > myRect.width || rect.height > myRect.height) return nullptr;
    if (fails > 64) full = true; // this node just sucks!
    if (left)
    {
      Node *node = left->AddRect(rect);
      if (node) return node;
      node = right->AddRect(rect);
      if (node) return node;
      if (left->full && right->full)
        full = true;
      fails++;
      return nullptr;
    }

    if (rect.width == myRect.width && rect.height == myRect.height)
    {
      full = true;
      return this;
    }

    int width_diff = myRect.width - rect.width;
    int height_diff = myRect.height - rect.height;
    if (width_diff > height_diff)
    { // horizontal
      left = _FastNewNode(rect.width, myRect.height, myRect.xpos, myRect.ypos, this);
      right = _FastNewNode(myRect.width - rect.width, myRect.height, myRect.xpos + rect.width, myRect.ypos, this);
    }
    else
    { // vertical
      left = _FastNewNode(myRect.width, rect.height, myRect.xpos, myRect.ypos, this);
      right = _FastNewNode(myRect.width, myRect.height - rect.height, myRect.xpos, myRect.ypos + rect.height, this);
    }
    Node *c = left->AddRect(rect);
    //_passUpBestRect(c);
    return c;
  }

  Node *left = nullptr;
  Node *right = nullptr;
  Node *parent = nullptr;
  Rect myRect;
  int fails = 0;
  bool full = false;
};

const int maxNodes = 1000000;
static Node *nodes = nullptr;
int nodeID = 0;
Node *_FastNewNode(int width, int height, int xpos, int ypos, Node *parent)
{
  if (!nodes)
    nodes = (Node*)malloc(maxNodes * sizeof(Node));
  new (nodes + nodeID) Node(width, height, xpos, ypos, parent);
  return &nodes[nodeID++];
}

void _passUpBestRect(Node *node)
{
  Node *p = node->parent;
  if (!p) return;
  //if (node->myRect.width < p->myRect.width || node->myRect.width < p->myRect.width)
  {
    if (p->left->full) p->myRect = p->right->myRect;
    else if (p->right->full) p->myRect = p->left->myRect;
    else
    {
      if (p->left->myRect.width * p->left->myRect.height > p->right->myRect.width * p->right->myRect.height)
        p->myRect = p->left->myRect;
      else
        p->myRect = p->right->myRect;
    }
  }
  return _passUpBestRect(p);
}

void _ClearNodes()
{
  nodeID = 0;
}

FastPacker::FastPacker(int width /*= 1024*/, int height /*= 1024*/)
{
  rootNode = new Node(width, height, 0, 0, nullptr);
}

bool FastPacker::InsertRect(Rect &rect)
{
  steps = 0;
    Node *place = rootNode->AddRect(rect);
  avg += steps;
  all++;
  if (all == 512)
  {
    //printf("--------------- %d\n", avg / all);
    all = 0;
    avg = 0;
  }
  if (!place) return false;
  rect.xpos = place->myRect.xpos;
  rect.ypos = place->myRect.ypos;
  return true;
}

void FastPacker::Clear()
{
  _ClearNodes();
  delete rootNode;
  rootNode = nullptr;
}
