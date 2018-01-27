#ifndef SoftGraph_h__
#define SoftGraph_h__

#include "SoftObject.h"

class SoftGraph : public SoftObject
{
public:

  SoftGraph(const std::string &name, Window *window, int xpos, int ypos, int width, int height, uint32_t mainCol, uint32_t backCol);

  void SetValueList(std::vector<int64_t> *valueList);

  void Update();

  std::vector<int64_t> *m_valueList = nullptr;

  double m_zoom = 64.0;
  double m_targetZoom = 64.0;
 };

#endif // SoftGraph_h__