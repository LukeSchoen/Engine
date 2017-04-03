#include "PointCloud.h"
#include "File.h"

PointCloud::PointCloud(const char *PCFpath /*= nullptr*/)
{
  if (PCFpath)
    Load(PCFpath);
}

PointCloud::~PointCloud()
{
  delete[] points;
  points = nullptr;
}

void PointCloud::Load(const char *PCFpath)
{
  points = (Point*)File::ReadFile(PCFpath, &pointCount);
  pointCount /= sizeof(Point);
}

void PointCloud::Save(const char *PCFpath)
{
  File::WriteFile(PCFpath, points, pointCount * sizeof(Point));
}

void PointCloud::AddPoint(const Point &p)
{

}
