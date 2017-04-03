#ifndef GLShaderTypes_h__
#define GLShaderTypes_h__

#include "stdint.h"

enum GLTextureType
{
  TT_2D,
  TT_2D_ARRAY,
};

enum GLUniformType
{
  UT_1f,
  UT_2f,
  UT_3f,
  UT_4f,

  UT_1i,
  UT_2i,
  UT_3i,
  UT_4i,

  UT_1fv,
  UT_2fv,
  UT_3fv,
  UT_4fv,

  UT_1iv,
  UT_2iv,
  UT_3iv,
  UT_4iv,

  UT_mat2fv,
  UT_mat3fv,
  UT_mat4fv,
};

enum GLAttributeType
{
  AT_BYTE,
  AT_BYTE_NORM,
  AT_UNSIGNED_BYTE,
  AT_UNSIGNED_BYTE_NORM,

  AT_SHORT,
  AT_UNSIGNED_SHORT,

  AT_INT,
  AT_UNSIGNED_INT,

  AT_HALF_FLOAT,

  AT_FLOAT,

  AT_DOUBLE,
};

const unsigned int glUndefined = 0xDFDFDFDF;

union GLResouceType
{
  GLResouceType(int value = glUndefined)
  {
    uniform = (GLUniformType)value;
  }
  GLResouceType(GLTextureType t)
  {
    texture = t;
  }
  GLResouceType(GLUniformType t)
  {
    uniform = t;
  }
  GLResouceType(GLAttributeType t)
  {
    attribute = t;
  }
  GLTextureType texture;
  GLUniformType uniform;
  GLAttributeType attribute;
};

struct GLResource
{
  GLResource()
  {
    name = nullptr;
    count = 0;
    type = (GLUniformType)glUndefined;
    verts = 0;
    pData = nullptr;
    glPtr = glUndefined;
    glLoc = glUndefined;
    loaded = false;
  }

  GLResource(const char *_name, const void *_pData, uint32_t _count = 1, GLResouceType _type = (GLUniformType)glUndefined, uint32_t _verts = 1)
  {
    name = _name;
    count = _count;
    type = _type;
    verts = _verts;
    pData = _pData;
    glPtr = glUndefined;
    glLoc = glUndefined;
    loaded = false;
  }
  GLResouceType type;
  uint32_t verts;
  uint32_t count;
  const char *name;
  const void *pData;
  unsigned int glPtr;
  unsigned int glLoc;
  bool loaded = false;
};

int GetUniformSize(GLUniformType dataType);

int GetAttributeSize(GLAttributeType dataType);

#endif // GLShaderTypes_h__
