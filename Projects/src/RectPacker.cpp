#include "RectPacker.h"

const int MAXINT = 1000000000;

RectPacker::RectPacker(int width, int height, bool rotations /*= true*/)
{
  Init(width, height, rotations);
}

void RectPacker::Init(int width, int height, bool rotations /*= true*/)
{
  binWidth = width;
  binHeight = height;
  allowRotations = rotations;

  Rect n;
  n.xpos = 0;
  n.ypos = 0;
  n.width = width;
  n.height = height;

  usedRectangles.clear();

  freeRectangles.clear();
  freeRectangles.push_back(n);
}

void RectPacker::Insert(std::vector<Rect> rects, std::vector<Rect> dst, FreeRectChoiceHeuristic method)
{
  dst.clear();

  while (rects.size() > 0)
  {
    int bestScore1 = MAXINT;
    int bestScore2 = MAXINT;
    int bestRectIndex = -1;
    Rect bestNode;

    for (int i = 0; i < rects.size(); ++i)
    {
      int score1 = 0;
      int score2 = 0;
      Rect newNode = ScoreRect((int)rects[i].width, (int)rects[i].height, method, score1, score2);

      if (score1 < bestScore1 || (score1 == bestScore1 && score2 < bestScore2))
      {
        bestScore1 = score1;
        bestScore2 = score2;
        bestNode = newNode;
        bestRectIndex = i;
      }
    }

    if (bestRectIndex == -1)
      return;

    PlaceRect(bestNode);
    rects.erase(rects.begin() + bestRectIndex);
  }
}

Rect RectPacker::Insert(int width, int height, FreeRectChoiceHeuristic method /*= RectBestShortSideFit*/)
{
  Rect newNode;
  int score1 = 0; // Unused in this function. We don't need to know the score after finding the position.
  int score2 = 0;
  switch (method)
  {
  case RectBestShortSideFit: newNode = FindPositionForNewNodeBestShortSideFit(width, height, score1, score2); break;
  case RectBottomLeftRule: newNode = FindPositionForNewNodeBottomLeft(width, height, score1, score2); break;
  case RectContactPointRule: newNode = FindPositionForNewNodeContactPoint(width, height, score1); break;
  case RectBestLongSideFit: newNode = FindPositionForNewNodeBestLongSideFit(width, height, score2, score1); break;
  case RectBestAreaFit: newNode = FindPositionForNewNodeBestAreaFit(width, height, score1, score2); break;
  }

  if (newNode.height == 0)
    return newNode;

  int numRectanglesToProcess = freeRectangles.size();
  for (int i = 0; i < numRectanglesToProcess; ++i)
  {
    if (SplitFreeNode(freeRectangles[i], newNode))
    {
      freeRectangles.erase(freeRectangles.begin() + i);
      --i;
      --numRectanglesToProcess;
    }
  }

  PruneFreeList();

  usedRectangles.push_back(newNode);
  return newNode;
}

void RectPacker::PlaceRect(Rect node)
{
  int numRectanglesToProcess = freeRectangles.size();
  for (int i = 0; i < numRectanglesToProcess; ++i)
  {
    if (SplitFreeNode(freeRectangles[i], node))
    {
      freeRectangles.erase(freeRectangles.begin() + i);
      --i;
      --numRectanglesToProcess;
    }
  }

  PruneFreeList();

  usedRectangles.push_back(node);
}

Rect RectPacker::ScoreRect(int width, int height, FreeRectChoiceHeuristic method, int &score1, int& score2)
{
  Rect newNode;
  score1 = MAXINT;
  score2 = MAXINT;
  switch (method)
  {
  case RectBestShortSideFit: newNode = FindPositionForNewNodeBestShortSideFit(width, height, score1, score2); break;
  case RectBottomLeftRule: newNode = FindPositionForNewNodeBottomLeft(width, height, score1, score2); break;
  case RectContactPointRule: newNode = FindPositionForNewNodeContactPoint(width, height, score1);
    score1 = -score1; // Reverse since we are minimizing, but for contact point score bigger is better.
    break;
  case RectBestLongSideFit: newNode = FindPositionForNewNodeBestLongSideFit(width, height, score2, score1); break;
  case RectBestAreaFit: newNode = FindPositionForNewNodeBestAreaFit(width, height, score1, score2); break;
  }

  // Cannot fit the current rectangle.
  if (newNode.height == 0)
  {
    score1 = MAXINT;
    score2 = MAXINT;
  }

  return newNode;
}

float RectPacker::Occupancy()
{
  int usedSurfaceArea = 0;
  for (int i = 0; i < usedRectangles.size(); ++i)
    usedSurfaceArea += usedRectangles[i].width * usedRectangles[i].height;

  return (float)usedSurfaceArea / (binWidth * binHeight);
}

Rect RectPacker::FindPositionForNewNodeBottomLeft(int width, int height, int &bestY, int &bestX)
{
  Rect bestNode;
  //memset(bestNode, 0, sizeof(Rect));

  bestY = MAXINT;

  for (int i = 0; i < freeRectangles.size(); ++i)
  {
    // Try to place the rectangle in upright (non-flipped) orientation.
    if (freeRectangles[i].width >= width && freeRectangles[i].height >= height)
    {
      int topSideY = (int)freeRectangles[i].ypos + height;
      if (topSideY < bestY || (topSideY == bestY && freeRectangles[i].xpos < bestX))
      {
        bestNode.xpos = freeRectangles[i].xpos;
        bestNode.ypos = freeRectangles[i].ypos;
        bestNode.width = width;
        bestNode.height = height;
        bestY = topSideY;
        bestX = (int)freeRectangles[i].xpos;
      }
    }
    if (allowRotations && freeRectangles[i].width >= height && freeRectangles[i].height >= width)
    {
      int topSideY = (int)freeRectangles[i].ypos + width;
      if (topSideY < bestY || (topSideY == bestY && freeRectangles[i].xpos < bestX))
      {
        bestNode.xpos = freeRectangles[i].xpos;
        bestNode.ypos = freeRectangles[i].ypos;
        bestNode.width = height;
        bestNode.height = width;
        bestY = topSideY;
        bestX = (int)freeRectangles[i].xpos;
      }
    }
  }
  return bestNode;
}

Rect RectPacker::FindPositionForNewNodeBestShortSideFit(int width, int height, int &bestShortSideFit, int &bestLongSideFit)
{
  Rect bestNode;
  //memset(&bestNode, 0, sizeof(Rect));

  bestShortSideFit = MAXINT;

  for (int i = 0; i < freeRectangles.size(); ++i)
  {
    // Try to place the rectangle in upright (non-flipped) orientation.
    if (freeRectangles[i].width >= width && freeRectangles[i].height >= height)
    {
      int leftoverHoriz = abs(freeRectangles[i].width - width);
      int leftoverVert = abs(freeRectangles[i].height - height);
      int shortSideFit = Min(leftoverHoriz, leftoverVert);
      int longSideFit = Max(leftoverHoriz, leftoverVert);

      if (shortSideFit < bestShortSideFit || (shortSideFit == bestShortSideFit && longSideFit < bestLongSideFit))
      {
        bestNode.xpos = freeRectangles[i].xpos;
        bestNode.ypos = freeRectangles[i].ypos;
        bestNode.width = width;
        bestNode.height = height;
        bestShortSideFit = shortSideFit;
        bestLongSideFit = longSideFit;
      }
    }

    if (allowRotations && freeRectangles[i].width >= height && freeRectangles[i].height >= width)
    {
      int flippedLeftoverHoriz = abs((int)freeRectangles[i].width - height);
      int flippedLeftoverVert = abs((int)freeRectangles[i].height - width);
      int flippedShortSideFit = Min(flippedLeftoverHoriz, flippedLeftoverVert);
      int flippedLongSideFit = Max(flippedLeftoverHoriz, flippedLeftoverVert);

      if (flippedShortSideFit < bestShortSideFit || (flippedShortSideFit == bestShortSideFit && flippedLongSideFit < bestLongSideFit))
      {
        bestNode.xpos = freeRectangles[i].xpos;
        bestNode.ypos = freeRectangles[i].ypos;
        bestNode.width = height;
        bestNode.height = width;
        bestShortSideFit = flippedShortSideFit;
        bestLongSideFit = flippedLongSideFit;
      }
    }
  }
  return bestNode;
}

Rect RectPacker::FindPositionForNewNodeBestLongSideFit(int width, int height, int &bestShortSideFit, int &bestLongSideFit)
{
  Rect bestNode;
  //memset(&bestNode, 0, sizeof(Rect));

  bestLongSideFit = MAXINT;

  for (int i = 0; i < freeRectangles.size(); ++i)
  {
    // Try to place the rectangle in upright (non-flipped) orientation.
    if (freeRectangles[i].width >= width && freeRectangles[i].height >= height)
    {
      int leftoverHoriz = abs((int)freeRectangles[i].width - width);
      int leftoverVert = abs((int)freeRectangles[i].height - height);
      int shortSideFit = Min(leftoverHoriz, leftoverVert);
      int longSideFit = Max(leftoverHoriz, leftoverVert);

      if (longSideFit < bestLongSideFit || (longSideFit == bestLongSideFit && shortSideFit < bestShortSideFit))
      {
        bestNode.xpos = freeRectangles[i].xpos;
        bestNode.ypos = freeRectangles[i].ypos;
        bestNode.width = width;
        bestNode.height = height;
        bestShortSideFit = shortSideFit;
        bestLongSideFit = longSideFit;
      }
    }

    if (allowRotations && freeRectangles[i].width >= height && freeRectangles[i].height >= width)
    {
      int leftoverHoriz = abs((int)freeRectangles[i].width - height);
      int leftoverVert = abs((int)freeRectangles[i].height - width);
      int shortSideFit = Min(leftoverHoriz, leftoverVert);
      int longSideFit = Max(leftoverHoriz, leftoverVert);

      if (longSideFit < bestLongSideFit || (longSideFit == bestLongSideFit && shortSideFit < bestShortSideFit))
      {
        bestNode.xpos = freeRectangles[i].xpos;
        bestNode.ypos = freeRectangles[i].ypos;
        bestNode.width = height;
        bestNode.height = width;
        bestShortSideFit = shortSideFit;
        bestLongSideFit = longSideFit;
      }
    }
  }
  return bestNode;
}

Rect RectPacker::FindPositionForNewNodeBestAreaFit(int width, int height, int &bestAreaFit, int &bestShortSideFit)
{
  Rect bestNode;
  //memset(&bestNode, 0, sizeof(Rect));

  bestAreaFit = MAXINT;

  for (int i = 0; i < freeRectangles.size(); ++i)
  {
    int areaFit = (int)freeRectangles[i].width * (int)freeRectangles[i].height - width * height;

    // Try to place the rectangle in upright (non-flipped) orientation.
    if (freeRectangles[i].width >= width && freeRectangles[i].height >= height)
    {
      int leftoverHoriz = abs((int)freeRectangles[i].width - width);
      int leftoverVert = abs((int)freeRectangles[i].height - height);
      int shortSideFit = Min(leftoverHoriz, leftoverVert);

      if (areaFit < bestAreaFit || (areaFit == bestAreaFit && shortSideFit < bestShortSideFit))
      {
        bestNode.xpos = freeRectangles[i].xpos;
        bestNode.ypos = freeRectangles[i].ypos;
        bestNode.width = width;
        bestNode.height = height;
        bestShortSideFit = shortSideFit;
        bestAreaFit = areaFit;
      }
    }

    if (allowRotations && freeRectangles[i].width >= height && freeRectangles[i].height >= width)
    {
      int leftoverHoriz = abs((int)freeRectangles[i].width - height);
      int leftoverVert = abs((int)freeRectangles[i].height - width);
      int shortSideFit = Min(leftoverHoriz, leftoverVert);

      if (areaFit < bestAreaFit || (areaFit == bestAreaFit && shortSideFit < bestShortSideFit))
      {
        bestNode.xpos = freeRectangles[i].xpos;
        bestNode.ypos = freeRectangles[i].ypos;
        bestNode.width = height;
        bestNode.height = width;
        bestShortSideFit = shortSideFit;
        bestAreaFit = areaFit;
      }
    }
  }
  return bestNode;
}

int RectPacker::CommonIntervalLength(int i1start, int i1end, int i2start, int i2end)
{
  if (i1end < i2start || i2end < i1start)
    return 0;
  return Min(i1end, i2end) - Max(i1start, i2start);
}

int RectPacker::ContactPointScoreNode(int x, int y, int width, int height)
{
  int score = 0;

  if (x == 0 || x + width == binWidth)
    score += height;
  if (y == 0 || y + height == binHeight)
    score += width;

  for (int i = 0; i < usedRectangles.size(); ++i) {
    if (usedRectangles[i].xpos == x + width || usedRectangles[i].xpos + usedRectangles[i].width == x)
      score += CommonIntervalLength((int)usedRectangles[i].ypos, (int)usedRectangles[i].ypos + (int)usedRectangles[i].height, y, y + height);
    if (usedRectangles[i].ypos == y + height || usedRectangles[i].ypos + usedRectangles[i].height == y)
      score += CommonIntervalLength((int)usedRectangles[i].xpos, (int)usedRectangles[i].xpos + (int)usedRectangles[i].width, x, x + width);
  }
  return score;
}

Rect RectPacker::FindPositionForNewNodeContactPoint(int width, int height, int &bestContactScore)
{
  Rect bestNode;
  //memset(&bestNode, 0, sizeof(Rect));

  bestContactScore = -1;

  for (int i = 0; i < freeRectangles.size(); ++i)
  {
    // Try to place the rectangle in upright (non-flipped) orientation.
    if (freeRectangles[i].width >= width && freeRectangles[i].height >= height)
    {
      int score = ContactPointScoreNode((int)freeRectangles[i].xpos, (int)freeRectangles[i].ypos, width, height);
      if (score > bestContactScore)
      {
        bestNode.xpos = (int)freeRectangles[i].xpos;
        bestNode.ypos = (int)freeRectangles[i].ypos;
        bestNode.width = width;
        bestNode.height = height;
        bestContactScore = score;
      }
    }
    if (allowRotations && freeRectangles[i].width >= height && freeRectangles[i].height >= width)
    {
      int score = ContactPointScoreNode((int)freeRectangles[i].xpos, (int)freeRectangles[i].ypos, height, width);
      if (score > bestContactScore)
      {
        bestNode.xpos = (int)freeRectangles[i].xpos;
        bestNode.ypos = (int)freeRectangles[i].ypos;
        bestNode.width = height;
        bestNode.height = width;
        bestContactScore = score;
      }
    }
  }
  return bestNode;
}

bool RectPacker::SplitFreeNode(Rect freeNode, Rect &usedNode)
{
  // Test with SAT if the rectangles even intersect.
  if (usedNode.xpos >= freeNode.xpos + freeNode.width || usedNode.xpos + usedNode.width <= freeNode.xpos ||
    usedNode.ypos >= freeNode.ypos + freeNode.height || usedNode.ypos + usedNode.height <= freeNode.ypos)
    return false;

  if (usedNode.xpos < freeNode.xpos + freeNode.width && usedNode.xpos + usedNode.width > freeNode.xpos)
  {
    // New node at the top side of the used node.
    if (usedNode.ypos > freeNode.ypos && usedNode.ypos < freeNode.ypos + freeNode.height)
    {
      Rect newNode = freeNode;
      newNode.height = usedNode.ypos - newNode.ypos;
      freeRectangles.push_back(newNode);
    }

    // New node at the bottom side of the used node.
    if (usedNode.ypos + usedNode.height < freeNode.ypos + freeNode.height)
    {
      Rect newNode = freeNode;
      newNode.ypos = usedNode.ypos + usedNode.height;
      newNode.height = freeNode.ypos + freeNode.height - (usedNode.ypos + usedNode.height);
      freeRectangles.push_back(newNode);
    }
  }

  if (usedNode.ypos < freeNode.ypos + freeNode.height && usedNode.ypos + usedNode.height > freeNode.ypos)
  {
    // New node at the left side of the used node.
    if (usedNode.xpos > freeNode.xpos && usedNode.xpos < freeNode.xpos + freeNode.width)
    {
      Rect newNode = freeNode;
      newNode.width = usedNode.xpos - newNode.xpos;
      freeRectangles.push_back(newNode);
    }

    // New node at the right side of the used node.
    if (usedNode.xpos + usedNode.width < freeNode.xpos + freeNode.width)
    {
      Rect newNode = freeNode;
      newNode.xpos = usedNode.xpos + usedNode.width;
      newNode.width = freeNode.xpos + freeNode.width - (usedNode.xpos + usedNode.width);
      freeRectangles.push_back(newNode);
    }
  }

  return true;
}

void RectPacker::PruneFreeList()
{
  for (int i = 0; i < freeRectangles.size(); ++i)
    for (int j = i + 1; j < freeRectangles.size(); ++j)
    {
      if (IsContainedIn(freeRectangles[i], freeRectangles[j]))
      {
        freeRectangles.erase(freeRectangles.begin() + i);
        --i;
        break;
      }
      if (IsContainedIn(freeRectangles[j], freeRectangles[i]))
      {
        freeRectangles.erase(freeRectangles.begin() + j);
        --j;
      }
    }
}

bool RectPacker::IsContainedIn(Rect a, Rect b)
{
  return a.xpos >= b.xpos && a.ypos >= b.ypos && a.xpos + a.width <= b.xpos + b.width && a.ypos + a.height <= b.ypos + b.height;
}
