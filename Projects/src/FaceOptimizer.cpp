#include "FaceOptimizer.h"


void SwapAndPop(std::vector<Face> &faceList, size_t faceIndex)
{
  if (!((faceIndex + 1) >= faceList.size()))
    std::swap(faceList[faceIndex], faceList.back());
  faceList.pop_back();
}



void FaceOptimizer::SplitOptimizeCombineFaces(BlockWorld *world, std::vector<Face> &faces, blockSide side)
{
  if (side == blockTop || side == blockBot)
  {
    std::vector<Face> work[256];
    for (int me = 0; me < faces.size(); me++)
      work[faces[me].pos.y].push_back(faces[me]);
    faces.clear();
    for (int w = 0; w < 256; w++)
      if (work[w].size())
      {
        OptimizeFaces(world, work[w], side);
        for (int me = 0; me < work[w].size(); me++)
          faces.push_back(work[w][me]);
      }
  }


  if (side == blockFront || side == blockBack)
  {
    std::vector<Face> work[16];
    for (int me = 0; me < faces.size(); me++)
      work[faces[me].pos.z + 160000 & 0xf].push_back(faces[me]);
    faces.clear();
    for (int w = 0; w < 16; w++)
      if (work[w].size())
      {
        OptimizeFaces(world, work[w], side);
        for (int me = 0; me < work[w].size(); me++)
          faces.push_back(work[w][me]);
      }
  }


  if (side == blockRight || side == blockLeft)
  {
    std::vector<Face> work[16];
    for (int me = 0; me < faces.size(); me++)
      work[faces[me].pos.x + 160000 & 0xf].push_back(faces[me]);
    faces.clear();
    for (int w = 0; w < 16; w++)
      if (work[w].size())
      {
        OptimizeFaces(world, work[w], side);
        for (int me = 0; me < work[w].size(); me++)
          faces.push_back(work[w][me]);
      }
  }
}

void FaceOptimizer::OptimizeFaces(BlockWorld *world, std::vector<Face> &faces, blockSide side)
{
  bool YZS = false; // frontBack
  bool XYS = false; // leftRight
  int card = 1;
  switch (side)
  {
  case blockFront: YZS = true; break;
  case blockBack:  YZS = true; break;
  case blockRight: XYS = true; break;
  case blockLeft:  XYS = true; break;
  }

  if (side == blockBot || side == blockBack || side == blockLeft) card = -1;

  // Rotate face
  for (int me = 0; me < faces.size(); me++)
  {
    if (YZS) faces[me].pos = vec3(faces[me].pos.x, faces[me].pos.z, faces[me].pos.y);
    if (XYS) faces[me].pos = vec3(faces[me].pos.y, faces[me].pos.x, faces[me].pos.z);
  }

  bool success = true;
  while (success)
  {
    success = false;
    for (int me = 0; me < faces.size(); me++)
      for (int he = me + 1; he < faces.size(); he++)
      {
        if (faces[me].pos.y == faces[he].pos.y && faces[me].Type == faces[he].Type) // Faces have same height and type
        { // Attempt to merge
          int left = Min(faces[me].pos.x, faces[he].pos.x);
          int right = Max(faces[me].pos.x + faces[me].width, faces[he].pos.x + faces[he].width);
          int bottom = Min(faces[me].pos.z, faces[he].pos.z);
          int top = Max(faces[me].pos.z + faces[me].height, faces[he].pos.z + faces[he].height);

          int oldArea = (faces[me].width * faces[me].height) + (faces[he].width * faces[he].height);
          int newArea = (right - left) * (top - bottom);

          if (oldArea != newArea)
            continue;

          bool canMerge = true;
          for (int z = bottom; z < top; z++)
            for (int x = left; x < right; x++)
            {
              vec3 p = vec3i(x, faces[me].pos.y, z);
              vec3 n = vec3i(0, card, 0);
              if (YZS) { p = vec3(p.x, p.z, p.y); n = vec3(n.x, n.z, n.y); };
              if (XYS) { p = vec3(p.y, p.x, p.z); n = vec3(n.y, n.x, n.z); };
              if (!((world->GetBlock(p) == faces[me].Type))) // homogeneity failure! face would not be of correct type
              {
                // We can ignore homogeneity failures as long as they are hidden by other blocks
                if (
                  world->blockTypes.IsBlockEnabled(world->GetBlock(p + n)) &&
                  world->blockTypes.IsBlockEnabled(world->GetBlock(p))
                  ) continue;
                canMerge = false; // cant merge
                x = right;
                z = top;
              }
            }
          if (canMerge)
          {
            faces[me].pos.x = left;
            faces[me].pos.z = bottom;
            faces[me].width = right - left;
            faces[me].height = top - bottom;
            //SwapAndPop(faces, he--);
            faces.erase(faces.begin() + he--);
            success = true;
            break;
          }
        }
      }
  }

  // Rotate face back
  for (int me = 0; me < faces.size(); me++)
  {
    if (YZS) faces[me].pos = vec3(faces[me].pos.x, faces[me].pos.z, faces[me].pos.y);
    if (XYS) faces[me].pos = vec3(faces[me].pos.y, faces[me].pos.x, faces[me].pos.z);
  }
}

void FaceOptimizer::FastOptimizedGenerator(BlockWorld *world, std::vector<Face> &faces, blockSide side)
{

}

void FaceOptimizer::SimpleSplitOptimizeCombineFaces(std::vector<Face> &faces, blockSide side, float slackRatio, int slackOffset)
{
  if (side == blockTop || side == blockBot)
  {
    std::vector<Face> work[256];
    for (int me = 0; me < faces.size(); me++)
      work[faces[me].pos.y].push_back(faces[me]);
    faces.clear();
    for (int w = 0; w < 256; w++)
      if (work[w].size())
      {
        SimpleOptimizeFaces(work[w], side, slackRatio, slackOffset);
        for (int me = 0; me < work[w].size(); me++)
          faces.push_back(work[w][me]);
      }
  }


  if (side == blockFront || side == blockBack)
  {
    std::vector<Face> work[16];
    for (int me = 0; me < faces.size(); me++)
      work[faces[me].pos.z + 160000 & 0xf].push_back(faces[me]);
    faces.clear();
    for (int w = 0; w < 16; w++)
      if (work[w].size())
      {
        SimpleOptimizeFaces(work[w], side, slackRatio, slackOffset);
        for (int me = 0; me < work[w].size(); me++)
          faces.push_back(work[w][me]);
      }
  }


  if (side == blockRight || side == blockLeft)
  {
    std::vector<Face> work[16];
    for (int me = 0; me < faces.size(); me++)
      work[faces[me].pos.x + 160000 & 0xf].push_back(faces[me]);
    faces.clear();
    for (int w = 0; w < 16; w++)
      if (work[w].size())
      {
        SimpleOptimizeFaces(work[w], side, slackRatio, slackOffset);
        for (int me = 0; me < work[w].size(); me++)
          faces.push_back(work[w][me]);
      }
  }
}

void FaceOptimizer::SimpleOptimizeFaces(std::vector<Face> &faces, blockSide side, float slackRatio, int slackOffset)
{
  bool YZS = false; // frontBack
  bool XYS = false; // leftRight
  int card = 1;
  switch (side)
  {
  case blockFront: YZS = true; break;
  case blockBack:  YZS = true; break;
  case blockRight: XYS = true; break;
  case blockLeft:  XYS = true; break;
  }

  if (side == blockBot || side == blockBack || side == blockLeft) card = -1;

  // Rotate face
  for (int me = 0; me < faces.size(); me++)
  {
    if (YZS) faces[me].pos = vec3(faces[me].pos.x, faces[me].pos.z, faces[me].pos.y);
    if (XYS) faces[me].pos = vec3(faces[me].pos.y, faces[me].pos.x, faces[me].pos.z);
  }

  bool success = true;
  while (success)
  {
    success = false;
    for (int me = 0; me < faces.size(); me++)
      for (int he = me + 1; he < faces.size(); he++)
      {
        //if (faces[me].pos.y == faces[he].pos.y && faces[me].Type == faces[he].Type) // Faces have same height and type
        //if (faces[me].pos.y == faces[he].pos.y) // Faces have same height
        { // Attempt to merge
          int left = Min(faces[me].pos.x, faces[he].pos.x);
          int right = Max(faces[me].pos.x + faces[me].width, faces[he].pos.x + faces[he].width);
          int bottom = Min(faces[me].pos.z, faces[he].pos.z);
          int top = Max(faces[me].pos.z + faces[me].height, faces[he].pos.z + faces[he].height);

          int fullArea = (right - left) * (top - bottom);
          int usedArea = faces[me].Type + faces[he].Type;
          int wastedSpace = fullArea - usedArea;

          if (wastedSpace > usedArea * slackRatio + slackOffset) continue; // limit wasted space

          faces[me].pos.x = left;
          faces[me].pos.z = bottom;
          faces[me].width = right - left;
          faces[me].height = top - bottom;
          faces[me].Type += faces[he].Type;
          SwapAndPop(faces, he--);
          success = true;
          break;
        }
      }
  }

  // Rotate face back
  for (int me = 0; me < faces.size(); me++)
  {
    if (YZS) faces[me].pos = vec3(faces[me].pos.x, faces[me].pos.z, faces[me].pos.y);
    if (XYS) faces[me].pos = vec3(faces[me].pos.y, faces[me].pos.x, faces[me].pos.z);
  }
}