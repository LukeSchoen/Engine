#ifndef softButton_h__
#define softButton_h__

#include "SoftObject.h"

class SoftButton : public SoftObject
{
public:
  SoftButton(const std::string &name, Window *window, int xpos, int ypos, int width, int height, uint32_t mainCol, uint32_t backCol);

  bool Update();
  bool clickable = false;

};

#endif // softButton_h__
