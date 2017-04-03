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

class BufferObject
{
public:
  BufferObject(int _width, int _height);

  void Reset();
  void AddRenderObject(RenderObject *ro);
  void AddBuffer(const char *name);
  void DelBuffer(const char *name);
  GLint GetBuffer(const char *name);
  void Render(const Matrix4x4 &MVP);
  static void DisplayFullscreenTexture(GLint texture);

private:

  int width, height;
  GLuint depthTexture;
  std::vector<RenderObject*> meshes;
  std::unordered_map<std::string, GLuint> buffers;

};

RenderObject *FullScreenQuad(const char *fragShaderFilePath);

#endif // Effect Object_h__
