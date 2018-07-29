#include <queue>
#include <vector>
#include <unordered_set>
#include "Window.h"
#include "Controls.h"

template<typename PathNode> struct MinHeapComparitor { bool operator()(PathNode const& a, PathNode const& b) const { return b.HeuristicCompare(a); } };
template<typename PathNode> struct PathNodeHasher { size_t operator()(PathNode const& node) const { return node.GetProblemStateHash(); } };
template<typename PathNode> struct PathNodeEquality { bool operator()(PathNode const& nodeA, PathNode const& nodeB) const { return nodeA.IsProblemStateEqual(nodeB); } };
template<typename T> using OutList = std::unordered_set <T, PathNodeHasher<T>, PathNodeEquality<T>>;
template<typename T> using InList = std::priority_queue <T, std::vector<T>, MinHeapComparitor<T>>;

extern Window *pWindow;

template<typename PathNode, bool useOutlist = false> struct AStar
{
  static PathNode FindPath(PathNode initialNode) { return FindPath(std::vector<PathNode>{ std::move(initialNode) }); }
  static PathNode FindPath(std::vector<PathNode> initialNodes)
  {
    static int frame = 0;
    InList<PathNode> inList;
    for (PathNode& node : initialNodes) inList.push(std::move(node));
    while (true)
    {
      PathNode bestNode = std::move(inList.top());
      if (frame++ % 1 == 0)
      {
        pWindow->Clear();

        for (auto &rect : bestNode.m_rects)
        {
          uint32_t color = rand() + rand() * 256;
          for (int y = 0; y < rect.height; y++)
            for (int x = 0; x < rect.width; x++)
            {
              int scaling = 20;
              int sx = (rect.x + x) * scaling;
              int sy = (rect.y + y) * scaling;
              for (int iy = 0; iy < scaling; iy++)
                for (int ix = 0; ix < scaling; ix++)
                  pWindow->pixels[(sx + ix) + (sy + iy) * pWindow->width] = color;
            }
        }

        pWindow->Swap();
        Controls::Update();
      }
      //getchar();
      inList.pop();
      if (bestNode.IsComplete()) return bestNode;
      else
      {
        std::vector<PathNode> newNodes = bestNode.Expand();
        for (PathNode& node : newNodes) inList.push(std::move(node));
        if ((inList.size() * bestNode.m_Image.size() / 8) > 1024 * 1024 * 1024)
          throw(std::exception("oh no..."));
      }
    }
  }
};

template<typename PathNode> struct AStar<PathNode, true>
{
  static PathNode FindPath(PathNode initialNode) { return FindPath(std::vector<PathNode>{ std::move(initialNode) }); }
  static PathNode FindPath(std::vector<PathNode> initialNodes)
  {
    OutList<PathNode> outList;
    InList<PathNode> inList;
    for (PathNode& node : initialNodes)
    {
      inList.push(node);
      outList.emplace(std::move(node));
    }

    static int frame = 0;
    static int maxNodes = 0;
    while (true)
    {
      PathNode bestNode = std::move(inList.top());

      if (frame++ % 5000 == 0)
      {
        pWindow->Clear();

        for (auto &rect : bestNode.m_rects)
        {
          uint32_t color = rand() + rand() * 256;
          for (int y = 0; y < rect.height; y++)
            for (int x = 0; x < rect.width; x++)
            {
              int scaling = 20;
              int sx = (rect.x + x) * scaling;
              int sy = (rect.y + y) * scaling;
              for (int iy = 0; iy < scaling; iy++)
                for (int ix = 0; ix < scaling; ix++)
                  pWindow->pixels[(sx + ix) + (sy + iy) * pWindow->width] = color;
            }
        }

        pWindow->Swap();
        Controls::Update();
        //getchar();
      }

      inList.pop();
      if (bestNode.IsComplete())
      {
        printf("%d\n", maxNodes);
        return bestNode;
      }
      else
      {
        std::vector<PathNode> newNodes = bestNode.Expand();
        for (PathNode& node : newNodes)
        {
          auto it = outList.find(node);
          if (it == outList.end())
          {
            inList.push(node);
            outList.emplace(std::move(node));
          }
          else if (node.HeuristicCompare(*it))
          {
            inList.push(node);
            outList.erase(it);
            outList.emplace(std::move(node));

          }
          maxNodes = std::max(maxNodes, (int)(outList.size() + inList.size()));
          if ((inList.size() * bestNode.m_Image.size() / 8) > 1024 * 1024 * 1024)
            throw(std::exception("oh no..."));
        }
      }
    }
  }
};
