#include "Maths.h"
#include "Renderobject.h"
#include "Camera.h"
#include "CustomRenderObjects.h"

struct Text
{
  Text();

  ~Text();

  void DrawText(const char *text, vec2 pos, float fontSize, bool draw3D);

  void DrawText(const char* text, vec2 pos, vec2 size, vec3 color, bool inWorld);
};
