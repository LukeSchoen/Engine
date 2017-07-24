#ifndef Sorter_h__
#define Sorter_h__

#include <vector>
#include "Maths.h"

#include "StreamFile.h"

class Sorter
{
public:
  static std::vector<int> MergeSort(const std::vector<int> &data)
  {
    auto newData = data;
    if (data.size() == 2)
      if (data[0] > data[1]) std::swap(newData[0], newData[1]); // Swap data
    if (data.size() > 2)
    {
      newData.clear();
      auto left = MergeSort(std::vector<int>(data.begin(), data.begin() + data.size() / 2)); // left side
      auto right = MergeSort(std::vector<int>(data.begin() + data.size() / 2, data.end())); // right side
      int64_t leftItem = 0;
      int64_t rightItem = 0;
      while (true)
      {
        bool inLeft = leftItem < left.size();
        bool inRight = rightItem < right.size();
        if (!(inLeft || inRight)) break;
        if (inLeft && inRight)
        {
          if (left[leftItem] < right[rightItem])
            newData.push_back(left[leftItem++]);
          else
            newData.push_back(right[rightItem++]);
        }
        else
        {
          if (inLeft) newData.push_back(left[leftItem++]);
          if (inRight) newData.push_back(right[rightItem++]);
        }
      }
    }
    return newData;
  }
};

#endif // Sorter_h__
