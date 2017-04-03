#ifndef FaceOptimizer_h__
#define FaceOptimizer_h__

#include <vector>
#include "BlockWorld.h"
#include "BlockTypes.h"
#include "BlockMesher.h"

struct FaceOptimizer
{
  static void SplitOptimizeCombineFaces(BlockWorld *world, std::vector<Face> &faces, blockSide side);

  static void OptimizeFaces(BlockWorld *world, std::vector<Face> &faces, blockSide side);

  static void FastOptimizedGenerator(BlockWorld *world, std::vector<Face> &faces, blockSide side);

  static void SimpleSplitOptimizeCombineFaces(std::vector<Face> &faces, blockSide side, float slackRatio = 0, int slackOffset = 0);

  static void SimpleOptimizeFaces(std::vector<Face> &faces, blockSide side, float slackRatio = 0, int slackOffset = 0);
};

#endif // FaceOptimizer_h__
