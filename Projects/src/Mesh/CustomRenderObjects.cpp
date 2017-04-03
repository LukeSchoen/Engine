#include "CustomRenderObjects.h"
#include "Shaders.h"


//////////////////////////////////////////////////////////////////////////
// Textured And Array Textured Render Object
//////////////////////////////////////////////////////////////////////////
TexturedAndArrayTexturedRenderObjectMaker::TexturedAndArrayTexturedRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  uv0ID = SpecifyAttribute("texcoord0", GLAttributeType::AT_FLOAT, 2);
  uv1ID = SpecifyAttribute("texcoord1", GLAttributeType::AT_FLOAT, 3);
  m_pRenderObject->AssignShader(SHADERDIR "TexturedAndArrayTexturedRenderObject.vert", SHADERDIR "TexturedAndArrayTexturedRenderObject.frag");
}

void TexturedAndArrayTexturedRenderObjectMaker::AddVertex(const vec3 &position, const vec2 &uv, const vec3 &arrauUv)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(uv0ID, &uv);
  AddAttribute(uv1ID, &arrauUv);
}

void TexturedAndArrayTexturedRenderObjectMaker::SetTexture(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture0", texturePath);
}

void TexturedAndArrayTexturedRenderObjectMaker::SetArrayTexture(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture1", texturePath);
}




//////////////////////////////////////////////////////////////////////////
// Coloured ArrayTextured Render Object
//////////////////////////////////////////////////////////////////////////
ColouredArrayTexturedRenderObjectMaker::ColouredArrayTexturedRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  uv0ID = SpecifyAttribute("texcoord0", GLAttributeType::AT_FLOAT, 3);
  col0ID = SpecifyAttribute("colour0", GLAttributeType::AT_FLOAT, 3);
  m_pRenderObject->AssignShader(SHADERDIR "ColouredArrayTexturedRenderObject.vert", SHADERDIR "ColouredArrayTexturedRenderObject.frag");
}

void ColouredArrayTexturedRenderObjectMaker::AddVertex(const vec3 &position, const vec3 &uv, const vec3 &col)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(uv0ID, &uv);
  AddAttribute(col0ID, &col);
}

void ColouredArrayTexturedRenderObjectMaker::SetTexture(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture0", texturePath);
}


//////////////////////////////////////////////////////////////////////////
// Alpha Coloured ArrayTextured Render Object
//////////////////////////////////////////////////////////////////////////
AlphaColouredArrayTexturedRenderObjectMaker::AlphaColouredArrayTexturedRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  uv0ID = SpecifyAttribute("texcoord0", GLAttributeType::AT_FLOAT, 3);
  col0ID = SpecifyAttribute("colour0", GLAttributeType::AT_FLOAT, 4);
  m_pRenderObject->AssignShader(SHADERDIR "AlphaColouredArrayTexturedRenderObject.vert", SHADERDIR "AlphaColouredArrayTexturedRenderObject.frag");
}

void AlphaColouredArrayTexturedRenderObjectMaker::AddVertex(const vec3 &position, const vec3 &uv, const vec4 &col)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(uv0ID, &uv);
  AddAttribute(col0ID, &col);
}

void AlphaColouredArrayTexturedRenderObjectMaker::SetTexture(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture0", texturePath);
}




//////////////////////////////////////////////////////////////////////////
// Specular Render Object
//////////////////////////////////////////////////////////////////////////
SpecularColouredRenderObjectMaker::SpecularColouredRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  col0ID = SpecifyAttribute("colour0", GLAttributeType::AT_FLOAT, 3);
  nor0ID = SpecifyAttribute("normal0", GLAttributeType::AT_FLOAT, 3);
  m_pRenderObject->AssignShader(SHADERDIR "SpecularRenderObject.vert", SHADERDIR "SpecularRenderObject.frag");
}

void SpecularColouredRenderObjectMaker::AddVertex(const vec3 &position, const vec3 &colour, const vec3 &normal)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(col0ID, &colour);
  AddAttribute(nor0ID, &normal);
}



//////////////////////////////////////////////////////////////////////////
// Textured Render Object
//////////////////////////////////////////////////////////////////////////
SpecularTexturedRenderObjectMaker::SpecularTexturedRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  uv0ID = SpecifyAttribute("texcoord0", GLAttributeType::AT_FLOAT, 2);
  nor0ID = SpecifyAttribute("normal0", GLAttributeType::AT_FLOAT, 3);
  m_pRenderObject->AssignShader(SHADERDIR "SpecularTexturedRenderObject.vert", SHADERDIR "SpecularTexturedRenderObject.frag");
}

void SpecularTexturedRenderObjectMaker::AddVertex(const vec3 &position, const vec2 &uv, const vec3 &normal)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(uv0ID, &uv);
  AddAttribute(nor0ID, &normal);
}

void SpecularTexturedRenderObjectMaker::SetTexture(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture0", texturePath);
}



//////////////////////////////////////////////////////////////////////////
// Coloured Render Object
//////////////////////////////////////////////////////////////////////////
ColouredRenderObjectMaker::ColouredRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  col0ID = SpecifyAttribute("colour0", GLAttributeType::AT_FLOAT, 3);
  m_pRenderObject->AssignShader(SHADERDIR "ColouredRenderObject.vert", SHADERDIR "ColouredRenderObject.frag");
}

void ColouredRenderObjectMaker::AddVertex(const vec3 &position, const vec3 &colour)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(col0ID, &colour);
}


//////////////////////////////////////////////////////////////////////////
// Alpha Coloured Render Object
//////////////////////////////////////////////////////////////////////////
AlphaColouredRenderObjectMaker::AlphaColouredRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  col0ID = SpecifyAttribute("colour0", GLAttributeType::AT_FLOAT, 4);
  m_pRenderObject->AssignShader(SHADERDIR "AlphaColouredRenderObject.vert", SHADERDIR "AlphaColouredRenderObject.frag");
}

void AlphaColouredRenderObjectMaker::AddVertex(const vec3 &position, const vec4 &colour)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(col0ID, &colour);
}


//////////////////////////////////////////////////////////////////////////
// Textured Render Object
//////////////////////////////////////////////////////////////////////////
TexturedRenderObjectMaker::TexturedRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  uv0ID = SpecifyAttribute("texcoord0", GLAttributeType::AT_FLOAT, 2);
  m_pRenderObject->AssignShader(SHADERDIR "TexturedRenderObject.vert", SHADERDIR "TexturedRenderObject.frag");
}

void TexturedRenderObjectMaker::AddVertex(const vec3 &position, const vec2 &uv)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(uv0ID, &uv);
}

void TexturedRenderObjectMaker::SetTexture(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture0", texturePath);
}


//////////////////////////////////////////////////////////////////////////
// Alpha Textured Render Object
//////////////////////////////////////////////////////////////////////////
AlphaTexturedRenderObjectMaker::AlphaTexturedRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  uv0ID = SpecifyAttribute("texcoord0", GLAttributeType::AT_FLOAT, 2);
  m_pRenderObject->AssignShader(SHADERDIR "TexturedRenderObject.vert", SHADERDIR "AlphaTexturedRenderObject.frag");
}

void AlphaTexturedRenderObjectMaker::AddVertex(const vec3 &position, const vec2 &uv)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(uv0ID, &uv);
}

void AlphaTexturedRenderObjectMaker::SetTexture(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture0", texturePath);
}






//////////////////////////////////////////////////////////////////////////
// Coloured Textured Render Object
//////////////////////////////////////////////////////////////////////////
ColouredTexturedRenderObjectMaker::ColouredTexturedRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  uv0ID = SpecifyAttribute("texcoord0", GLAttributeType::AT_FLOAT, 2);
  col0ID = SpecifyAttribute("colour0", GLAttributeType::AT_FLOAT, 3);
  m_pRenderObject->AssignShader(SHADERDIR "ColouredTexturedRenderObject.vert", SHADERDIR "ColouredTexturedRenderObject.frag");
}

void ColouredTexturedRenderObjectMaker::AddVertex(const vec3 &position, const vec2 &uv, const vec3 &col)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(uv0ID, &uv);
  AddAttribute(col0ID, &col);
}

void ColouredTexturedRenderObjectMaker::SetTexture(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture0", texturePath);
}


//////////////////////////////////////////////////////////////////////////
// Alpha Coloured Textured Render Object
//////////////////////////////////////////////////////////////////////////
AlphaColouredTexturedRenderObjectMaker::AlphaColouredTexturedRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  uv0ID = SpecifyAttribute("texcoord0", GLAttributeType::AT_FLOAT, 2);
  col0ID = SpecifyAttribute("colour0", GLAttributeType::AT_FLOAT, 4);
  m_pRenderObject->AssignShader(SHADERDIR "AlphaColouredTexturedRenderObject.vert", SHADERDIR "AlphaColouredTexturedRenderObject.frag");
}

void AlphaColouredTexturedRenderObjectMaker::AddVertex(const vec3 &position, const vec2 &uv, const vec4 &col)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(uv0ID, &uv);
  AddAttribute(col0ID, &col);
}

void AlphaColouredTexturedRenderObjectMaker::SetTexture(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture0", texturePath);
}





//////////////////////////////////////////////////////////////////////////
// Double Textured Render Object
//////////////////////////////////////////////////////////////////////////
DoubleTexturedRenderObjectMaker::DoubleTexturedRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  uv0ID = SpecifyAttribute("texcoord0", GLAttributeType::AT_FLOAT, 2);
  uv1ID = SpecifyAttribute("texcoord1", GLAttributeType::AT_FLOAT, 2);
  m_pRenderObject->AssignShader(SHADERDIR "DoubleTexturedRenderObject.vert", SHADERDIR "DoubleTexturedRenderObject.frag");
}

void DoubleTexturedRenderObjectMaker::AddVertex(const vec3 &position, const vec2 &uv1, const vec2 &uv2)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(uv0ID, &uv1);
  AddAttribute(uv1ID, &uv2);
}

void DoubleTexturedRenderObjectMaker::SetTexture1(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture0", texturePath);
}

void DoubleTexturedRenderObjectMaker::SetTexture2(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture1", texturePath);
}


//////////////////////////////////////////////////////////////////////////
// Alpha Double Textured Render Object
//////////////////////////////////////////////////////////////////////////
AlphaDoubleTexturedRenderObjectMaker::AlphaDoubleTexturedRenderObjectMaker()
{
  pos0ID = SpecifyAttribute("position0", GLAttributeType::AT_FLOAT, 3);
  uv0ID = SpecifyAttribute("texcoord0", GLAttributeType::AT_FLOAT, 2);
  uv1ID = SpecifyAttribute("texcoord1", GLAttributeType::AT_FLOAT, 2);
  m_pRenderObject->AssignShader(SHADERDIR "DoubleTexturedRenderObject.vert", SHADERDIR "DoubleTexturedRenderObject.frag");
}

void AlphaDoubleTexturedRenderObjectMaker::AddVertex(const vec3 &position, const vec2 &uv1, const vec2 &uv2)
{
  AddAttribute(pos0ID, &position);
  AddAttribute(uv0ID, &uv1);
  AddAttribute(uv1ID, &uv2);
}

void AlphaDoubleTexturedRenderObjectMaker::SetTexture1(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture0", texturePath);
}

void AlphaDoubleTexturedRenderObjectMaker::SetTexture2(const char *texturePath)
{
  m_pRenderObject->AssignTexture("texture1", texturePath);
}
