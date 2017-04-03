#include <vector>

struct sortElement
{
  sortElement(void *k, float v)
  {
    key = k;
    value = v;
  }
  void *key;
  float value;
};

struct SortedList
{
  void AddElement(void *key, float value)
  {
    int insertSpot = 0;
    while (insertSpot < list.size())
    {
      if (list[insertSpot].value > value)
      {
        list.emplace(list.begin() + insertSpot, key, value);
        insertSpot = -1;
        break;
      }
      insertSpot++;
    }
    if(insertSpot != -1)
      list.emplace_back(key, value);
  }

  int GetCount()
  {
    return list.size();
  }

  void *GetElement(int64_t index)
  {
    return list[index].key;
  }

  std::vector<sortElement> list;
};
