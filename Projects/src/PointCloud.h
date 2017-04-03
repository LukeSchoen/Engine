#ifndef PointCloud_h__
#define PointCloud_h__
#include "stdint.h"

struct Point
{
  int32_t x;
  int32_t y;
  int32_t z;
  uint32_t color;
};

struct PointCloud
{
  PointCloud(const char *PCFpath = nullptr);
  ~PointCloud();
  void Load(const char *PCFpath);
  void Save(const char *PCFpath);
  void AddPoint(const Point &p);
  Point *points = nullptr;
  int64_t pointCount = 0;
};

#endif // PointCloud_h__