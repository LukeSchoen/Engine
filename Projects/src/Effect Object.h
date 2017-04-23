#ifndef Effect Object_h__
#define Effect Object_h__
#include <vector>
#include <unordered_map>
#include "RenderObject.h"
#include <string>
#include "Textures.h"
#include "SDL_opengl.h"
#include "Framebuffer.h"
#include "Shaders.h"
#include "PolyModel.h"

struct RenderInstance
{
  RenderInstance(RenderObject *a_modelMesh, mat4 a_modelMat)
  {
    modelMesh = a_modelMesh;
    modelMat = a_modelMat;
  }
  RenderObject *modelMesh;
  mat4 modelMat;
};

class BufferObject
{
public:
  BufferObject(int _width, int _height);

  void ClearBuffers();
  void ClearRenderInstances();
  void AddRenderObject(RenderObject *modelMesh, mat4 modelMat = mat4());
  void AddPolyModel(PolyModel *pm);
  void AddBuffer(const char *name);
  void DelBuffer(const char *name);
  GLint GetBuffer(const char *name);
  GLint GetDepth();
  void Render(const Matrix4x4 &MVP);
  static void DisplayFullscreenTexture(GLint texture);

private:

  int width, height;
  GLuint depthTexture;
  std::vector<RenderInstance> RenderInstances;
  std::unordered_map<std::string, GLuint> buffers;
};

RenderObject *FullScreenQuad(const char *fragShaderFilePath);

#endif // Effect Object_h__
