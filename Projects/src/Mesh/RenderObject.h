#ifndef RenderObject_h__
#define RenderObject_h__


#define GLEW_STATIC
#include <stdint.h>
#include "Maths.h"
#include <limits.h>
#include <gl\glew.h>
#include <gl\glu.h>
#include "SDL.h"
#include <SDL_opengl.h>
#include "SDL_opengles2.h"
#include "GLTypes.h"

struct RenderObject
{
public:

  void AssignShader(const char *vertFile, const char *fragFile, const char *geomFile = nullptr);
  void AssignTexture(const char *varName, const char *texturePath, GLTextureType textureType = TT_2D);
  void AssignTexture(const char *varName, GLuint textureID, GLTextureType textureType = TT_2D);
  void AssignUniform(const char *varName, GLUniformType uType, const void *pData, int count = 1);
  void AssignAttribute(const char *varName, GLAttributeType aType, const void *pData, int attributeCount, int vertexCount);

  void AccessTexture(const char *textureName, const char **ppTexturePath);
  void AccessUniform(const char *uniformName, const void **ppUniformData, GLUniformType *uType, uint32_t *uCount);
  void AccessAttribute(const char *attributeName, int64_t *pVertCount, uint32_t *aCount, GLAttributeType *aType, const void **ppPosData);

  void Serialize(char **data, int *pLength);
  void DeSerialize(char *data, int *pLength = nullptr);

  void Render(const Matrix4x4 &MVP, int firstVertex = 0, int vertCount = INT_MAX);
  void RenderLines(const Matrix4x4 &MVP);
  void RenderPoints(const Matrix4x4 &MVP);

  void Clear();

  ~RenderObject();

  unsigned int attributeCount = 0;
  unsigned int uniformCount = 0;
  unsigned int textureCount = 0;

  unsigned int program = glUndefined;
  void UploadToGPU();

protected:

  void BindShaders();
  void BindTextures();
  void BindUniforms();
  void BindAttributes();

  void AssignResource(GLResource **ppResourceBlock, unsigned int *pResourceBlockSize, GLResource &newResouce, unsigned int newResourceSize);
  bool AccessResource(GLResource *pResourceBlock, unsigned int pResourceBlockSize, const char *resorceName, uint32_t *pResouceCount, int64_t *pResouceVerts, GLResouceType *pType, const void **ppResourceData);

  GLResource *pAttributeData = nullptr;
  GLResource *pUniformData = nullptr;
  GLResource *pTextureData = nullptr;

  unsigned int vertexCount = 0;

  char *vertShaderPath = nullptr;
  char *fragShaderPath = nullptr;
  char *geomShaderPath = nullptr;

  bool shadersUploaded = false;
  bool texturesUploaded = false;
  bool uniformsUploaded = false;
  bool attributesUploaded = false;
};


#endif // RenderObject_h__
