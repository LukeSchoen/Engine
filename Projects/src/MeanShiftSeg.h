#ifndef MeanShiftSeg_h__
#define MeanShiftSeg_h__

#include <string>
#include <vector>
#include "maths.h"

class RgnAdjList
{
public:

  int		label = 0;
  float	edge_strength = 0;
  int		edge_pixel_count = 0;

  RgnAdjList	*next = nullptr;

  RgnAdjList();

  int Insert(RgnAdjList* entry);

private:
  RgnAdjList	*cur = nullptr;
  RgnAdjList	*prev = nullptr;
  unsigned char exists = false;
};

struct MeanShiftSegmentationState
{
  vec3 *image;
  int *regions;
  std::vector<float> modes;
  std::vector<int> mode_point_counts;
  std::vector<int> index_table;
  std::vector<int> mode_table;
  std::vector<int> point_list;
  int width;
  int height;
  int point_count;
  int rgn_count;
  float h[2];

  MeanShiftSegmentationState(int _width, int _height);
};

struct RegionAdjacencyTable
{
  std::vector<RgnAdjList> rgn_adj_list;
  std::vector<RgnAdjList> rgn_adj_pool;
  RgnAdjList* free_rgn_adj_lists;

  RegionAdjacencyTable(const MeanShiftSegmentationState& state);
};

class MeanShiftSeg
{
private:
  static float sigma_r_;
  static int sigma_s_;
  static int min_size_;
  static float speed_threshold_;

  static void Filter(vec3 *inp, MeanShiftSegmentationState& state);
  static void Fill(int regionLoc, int label, int neigh[], MeanShiftSegmentationState& state);
  static void Connect(MeanShiftSegmentationState& state);
  static bool InWindow(const MeanShiftSegmentationState& state, int mode1, int mode2);
  static void TransitiveClosure(MeanShiftSegmentationState& state);
  static float SqDistance(const MeanShiftSegmentationState& state, int mode1, int mode2);
  static void Prune(MeanShiftSegmentationState& state);
  static void FuseRegions(MeanShiftSegmentationState& state);

public:

  static void ApplySegmentation(std::string input, std::string output, int sigmaS = 7, float sigmaR = 6.5, int minRegionSize = 20);
};

#endif // MeanShiftSeg_h__