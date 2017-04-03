#ifndef Shaders_h__
#define Shaders_h__
#include "RenderObject.h"
#include "File.h"

#ifdef FINAL
  #define SHADERDIR "shaders/"
#else
  #define SHADERDIR "src/Mesh/shaders/"
#endif

void printProgramLog(GLuint program);

void printShaderLog(GLuint shader);

struct Shaders
{
  static GLuint Load(const char *vertShaderFile, const char *fragShaderFile, const char *geomShaderFile = nullptr);
  static void Report();
};


#endif // Shaders_h__