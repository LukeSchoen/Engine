#ifndef RectPacker_h__
#define RectPacker_h__

#include "maths.h"
#include <vector>

struct Rect
{
  Rect(int w = 0, int h = 0, int t = 0, int x = 0, int y = 0)
  {
    xpos = x;
    ypos = y;
    width = w;
    height = h;
    type = t;
  }
  int xpos, ypos, layer, width, height, type;
};

enum FreeRectChoiceHeuristic
{
  RectBestShortSideFit, // Seems to be the best !
  RectBestLongSideFit,
  RectBestAreaFit,
  RectBottomLeftRule,
  RectContactPointRule
};

struct RectPacker
{
  int binWidth = 0;
  int binHeight = 0;
  bool allowRotations;

  std::vector<Rect> usedRectangles;
  std::vector<Rect> freeRectangles;

  RectPacker(int width, int height, bool rotations = true);

  void Init(int width, int height, bool rotations = true);

  Rect Insert(int width, int height, FreeRectChoiceHeuristic method = RectBestShortSideFit);

  void Insert(std::vector<Rect> rects, std::vector<Rect> dst, FreeRectChoiceHeuristic method);

  void PlaceRect(Rect node);

  Rect ScoreRect(int width, int height, FreeRectChoiceHeuristic method, int &score1, int& score2);

  // Computes the ratio of used surface area.
  float Occupancy();

  Rect FindPositionForNewNodeBottomLeft(int width, int height, int &bestY, int &bestX);

  Rect FindPositionForNewNodeBestShortSideFit(int width, int height, int &bestShortSideFit, int &bestLongSideFit);

  Rect FindPositionForNewNodeBestLongSideFit(int width, int height, int &bestShortSideFit, int &bestLongSideFit);

  Rect FindPositionForNewNodeBestAreaFit(int width, int height, int &bestAreaFit, int &bestShortSideFit);

  // Returns 0 if the two intervals i1 and i2 are disjoint, or the length of their overlap otherwise.
  int CommonIntervalLength(int i1start, int i1end, int i2start, int i2end);

  int ContactPointScoreNode(int x, int y, int width, int height);

  Rect FindPositionForNewNodeContactPoint(int width, int height, int &bestContactScore);

  bool SplitFreeNode(Rect freeNode, Rect &usedNode);

  void PruneFreeList();

  bool IsContainedIn(Rect a, Rect b);

};
#endif // RectPacker_h__