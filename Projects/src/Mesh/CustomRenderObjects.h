#ifndef CustomRenderObjects_h__
#define CustomRenderObjects_h__
#include "RenderObjectMaker.h"



struct TexturedAndArrayTexturedRenderObjectMaker : public RenderObjectMaker
{
  int pos0ID, uv0ID, uv1ID;
  TexturedAndArrayTexturedRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec2 &uv, const vec3 &ArrayUv);
  void SetTexture(const char *texturePath);
  void SetArrayTexture(const char *texturePath);
}; // make an alpha version for this



struct ColouredArrayTexturedRenderObjectMaker : public RenderObjectMaker
{
  int pos0ID, uv0ID, col0ID;
  ColouredArrayTexturedRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec3 &uv, const vec3 &colour);
  void SetTexture(const char *texturePath);
};

struct AlphaColouredArrayTexturedRenderObjectMaker : public AlphaRenderObjectMaker
{
  int pos0ID, uv0ID, col0ID;
  AlphaColouredArrayTexturedRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec3 &uv, const vec4 &colour);
  void SetTexture(const char *texturePath);
};


struct SpecularColouredRenderObjectMaker : public RenderObjectMaker
{
  int pos0ID, col0ID, nor0ID;
  SpecularColouredRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec3 &colour, const vec3 &normal);
};


struct SpecularTexturedRenderObjectMaker : public RenderObjectMaker
{
  int pos0ID, uv0ID, nor0ID;
  SpecularTexturedRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec2 &uv, const vec3 &normal);
  void SetTexture(const char *texturePath);
};




struct ColouredRenderObjectMaker : public RenderObjectMaker
{
  int pos0ID, col0ID;
  ColouredRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec3 &colour);
};

struct AlphaColouredRenderObjectMaker : public AlphaRenderObjectMaker
{
  int pos0ID, col0ID;
  AlphaColouredRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec4 &colour);
};



struct TexturedRenderObjectMaker : public RenderObjectMaker
{
  int pos0ID, uv0ID;
  TexturedRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec2 &uv);
  void SetTexture(const char *texturePath);
};

struct AlphaTexturedRenderObjectMaker : public AlphaRenderObjectMaker
{
  int pos0ID, uv0ID;
  AlphaTexturedRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec2 &uv);
  void SetTexture(const char *texturePath);
};



struct ColouredTexturedRenderObjectMaker : public RenderObjectMaker
{
  int pos0ID, uv0ID, col0ID;
  ColouredTexturedRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec2 &uv, const vec3 &colour);
  void SetTexture(const char *texturePath);
};

struct AlphaColouredTexturedRenderObjectMaker : public AlphaRenderObjectMaker
{
  int pos0ID, uv0ID, col0ID;
  AlphaColouredTexturedRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec2 &uv, const vec4 &colour);
  void SetTexture(const char *texturePath);
};



struct DoubleTexturedRenderObjectMaker : public RenderObjectMaker
{
  int pos0ID, uv0ID, uv1ID;
  DoubleTexturedRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec2 &uv1, const vec2 &uv2);
  void SetTexture1(const char *texturePath);
  void SetTexture2(const char *texturePath);
};

struct AlphaDoubleTexturedRenderObjectMaker : public RenderObjectMaker
{
  int pos0ID, uv0ID, uv1ID;
  AlphaDoubleTexturedRenderObjectMaker();
  void AddVertex(const vec3 &position, const vec2 &uv1, const vec2 &uv2);
  void SetTexture1(const char *texturePath);
  void SetTexture2(const char *texturePath);
};

#endif // CustomRenderObjects_h__
