#ifndef Assimp_h__
#define Assimp_h__

#include "Maths.h"
#include "FilePath.h"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postProcess.h"
#include <vector>

struct vertex
{
  vertex()
  {
    pos = vec3();
    col = vec3();
    uvs = vec2();
  }
  vertex(vec3 _pos, vec3 _col, vec3 _nor, vec2 _uvs)
  {
    pos = _pos;
    col = _col;
    nor = _nor;
    uvs = _uvs;
  }

  vec3 pos;
  vec3 col;
  vec3 nor;
  vec2 uvs;
};

struct face
{
  face()
  {
    v1 = vertex();
    v2 = vertex();
    v3 = vertex();
  }
  face(vertex _v1, vertex _v2, vertex _v3, int _material)
  {
    v1 = _v1;
    v2 = _v2;
    v3 = _v3;
    material = _material;
  }
  vertex v1, v2, v3;
  int material;
};

struct Material
{
  Material()
  {

  }
  Material(const char *_path, const char *_basePath, vec4 _color = vec4(1, 1, 1, 1))
  {
    // Find path of base
    int baseLen = 0;
    if(_basePath)
      baseLen = strlen(_basePath);
    int baseFileNameAdd = 0;
    for (int i = baseLen-1; i > 0 ; i--)
      if (_basePath[i] == '/' || _basePath[i] == '\\')
      {
        baseFileNameAdd = i + 1;
        break;
      }
    int pathLen = 0;
    if(_path)
      pathLen = strlen(_path) + 1;
    int newLen = pathLen + baseFileNameAdd;
    if (newLen)
    {
      path = new char[newLen];
      memcpy(path, _basePath, baseFileNameAdd);
      memcpy(path + baseFileNameAdd, _path, pathLen);
    }
    color = _color;
  }

  ~Material()
  {
    
  }
  char *path = nullptr;
  vec4 color;
};



struct AssimpModel
{
  face *faces = nullptr;
  unsigned int faceCount = 0;
  Material *materials;
  unsigned int materialCount = 0;
};

AssimpModel AssimpLoadModel(const char *filePath);

#endif // Assimp_h__
