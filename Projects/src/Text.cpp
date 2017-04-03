#include "Text.h"
#include "Assets.h"

static RenderObject *textMesh = nullptr;

Text::Text()
{
  ColouredTexturedRenderObjectMaker ctrom;
  ctrom.SetTexture(ASSETDIR "/Font.bmp");
  textMesh = ctrom.AssembleRenderObject();
}

Text::~Text()
{
  textMesh->Clear();
}

void Text::DrawText(const char* text, vec2 pos, vec2 size, vec3 color, bool inWorld)
{
  int len = strlen(text);

  vec3 *positions = new vec3[len * 6];
  vec3 *colors = new vec3[len * 6];
  vec2 *uvs = new vec2[len * 6];

  for (int character = 0; character < len; character++)
  {
    float Xpos1 = pos.x + (size.x) * (character / (len + 0.0));
    float Ypos1 = pos.y;
    float Xpos2 = Xpos1 + (size.x) / (len + 0.0);
    float Ypos2 = Ypos1 + size.y;
    float Zpos = - 2;

    char tc = text[character];
    if (tc == ' ')
      continue;
    else
      tc -= 33;

    int UVX = tc % 32;
    int UVY = tc / 32;

    float U1 = UVX / 32.0f;
    float V1 = UVY / 32.0f;

    float U2 = U1 + 1.0 / 32.0f;
    float V2 = V1 + 1.0 / 32.0f;

    //if (!inWorld)
    //{
    //  vec3 c = vec3() - Camera::Position();
    //  Xpos1 += c.x;
    //  Xpos2 += c.x;
    //  Ypos1 += c.z;
    //  Ypos2 += c.z;
    //  Zpos = c.y - 1.0;
    //}



    positions[character * 6 + 0] = vec3(Xpos1, Zpos, Ypos1);
    colors[character * 6 + 0] = vec3(color);
    uvs[character * 6 + 0] = vec2(U1, V1);

    positions[character * 6 + 1] = vec3(Xpos2, Zpos, Ypos1);
    colors[character * 6 + 1] = vec3(color);
    uvs[character * 6 + 1] = vec2(U2, V1);

    positions[character * 6 + 2] = vec3(Xpos1, Zpos, Ypos2);
    colors[character * 6 + 2] = vec3(color);
    uvs[character * 6 + 2] = vec2(U1, V2);



    positions[character * 6 + 3] = vec3(Xpos2, Zpos, Ypos1);
    colors[character * 6 + 3] = vec3(color);
    uvs[character * 6 + 3] = vec2(U2, V1);

    positions[character * 6 + 4] = vec3(Xpos2, Zpos, Ypos2);
    colors[character * 6 + 4] = vec3(color);
    uvs[character * 6 + 4] = vec2(U2, V2);

    positions[character * 6 + 5] = vec3(Xpos1, Zpos, Ypos2);
    colors[character * 6 + 5] = vec3(color);
    uvs[character * 6 + 5] = vec2(U1, V2);

  }

  mat4 m;
  m.Ortho(-5, 5, -5, 5);
  //m.Perspective(90, 1, 0.01, 1000);

  textMesh->AssignAttribute("position0", GLAttributeType::AT_FLOAT, positions, 3, len * 6);
  textMesh->AssignAttribute("colour0", GLAttributeType::AT_FLOAT, colors, 3, len * 6);
  textMesh->AssignAttribute("texcoord0", GLAttributeType::AT_FLOAT, uvs, 2, len * 6);

  textMesh->Render(m);
}

void Text::DrawText(const char *text, vec2 pos, float fontSize, bool draw3D)
{
  float fSize = fontSize / 10.0f;
  int len = strlen(text);
  DrawText(text, pos, vec2(len * fSize, fSize), vec3(1,1,1), draw3D);
}
