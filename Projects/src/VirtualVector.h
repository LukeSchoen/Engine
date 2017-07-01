#include <vector>
#include "Pool.h"
#include "Maths.h"

#ifndef VirtualVector_h__
#define VirtualVector_h__

class VirtualVector
{
public:
  void Insert(int64_t position, float data)
  {
    int64_t page, slot;
    FindSlot(position, &page, &slot);
    int64_t insertionCost = Min(slot, m_pages[page].size() - slot);
    int64_t fragmentCost = insertionCost;
    if (CostToAddPage < insertionCost)
    {
      // Add page
    }
    else
    {
      // Insert
    }
  }

  void Remove(int64_t position)
  {

  }

protected:
  bool FindSlot(int64_t position, int64_t *pPage, int64_t *pSlot)
  {
    if (position < 0 || position > m_data.UseCount)
      return false;
    if (position == m_data.UseCount)
    {
      if (pPage)
        *pPage = m_pages.size() - 1;
      if (pSlot)
        *pSlot = m_pages.back().size();
      return true;
    }
    // This could be somewhat faster with a tree !
    int64_t currentPos = 0;
    for (int64_t p = 0; p < m_pages.size(); p++)
    {
      if (currentPos + m_pages[p].size() > position)
      {
        if (pPage)
          *pPage = p;
        if (pSlot)
          *pSlot = position - currentPos;
        return true;
      }
      currentPos += m_pages[p].size();
    }
  }

  const int64_t CostToMoveItem = sizeof(int64_t);
  const int64_t CostToAddPage = CostToMoveItem * 64;

  std::vector<std::vector<int64_t>> m_pages; // Indirection Pages
  Pool m_data; // Actual Data
};

#endif // VirtualVector_h__
