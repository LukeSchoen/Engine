#include "Effect Object.h"
#include "CustomRenderObjects.h"

BufferObject::BufferObject(int _width, int _height)
{
  width = _width;
  height = _height;
  depthTexture = Textures::CreateTexture(width, height, true);
}

void BufferObject::ClearRenderInstances()
{
  RenderInstances.clear();
}

void BufferObject::ClearBuffers()
{
  buffers.clear();
}

void BufferObject::AddRenderObject(RenderObject *modelMesh, mat4 modelMat /*= mat4()*/)
{
  RenderInstances.emplace_back(modelMesh, modelMat);
}

void BufferObject::AddPolyModel(PolyModel *pm, mat4 modelMat /*= mat4()*/)
{
  RenderObject *meshes;
  int64_t meshCount;
  pm->GetMeshData(&meshCount, &meshes); for (int m = 0; m < meshCount; m++)
    AddRenderObject(&meshes[m], modelMat);
}

void BufferObject::AddBuffer(const char *name)
{
  if (buffers.find(name) == buffers.end())
  {
    GLuint colorTexture = Textures::CreateTexture(width, height);
    buffers.emplace(name, colorTexture);
  }
}

void BufferObject::DelBuffer(const char *name)
{
  buffers.erase(name);
}

GLint BufferObject::GetBuffer(const char *name)
{
  return buffers[name];
}

GLint BufferObject::GetDepth()
{
  return depthTexture;
}

void BufferObject::Render(const Matrix4x4 &VP)
{
  // Prepare frame buffer
  int vp[4];
  static FrameBuffer fb;
  glGetIntegerv(GL_VIEWPORT, vp);
  fb.Bind(FBBM_Write);
  fb.AttachDepth(depthTexture);
  glViewport(0, 0, width, height);

  // Bind shader outputs
  fb.AttachColour(0, buffers.begin()->second);
  FrameBufferStatus a = fb.GetStatus(FBBM_Write);

  int t = 0; for (auto &buffer : buffers) fb.AttachColour(t++, buffer.second);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  t = 0; for (auto &buffer : buffers) fb.DetachColour(t++);

  for (auto &RenderInstance : RenderInstances)
  {
    RenderInstance.modelMesh->UploadToGPU();
    for (auto &buffer : buffers)
    {
      int loc = glGetFragDataLocation(RenderInstance.modelMesh->program, buffer.first.c_str());
      fb.AttachColour(loc, buffer.second);
    }
    mat4 MVP = VP * RenderInstance.modelMat;
    MVP.Transpose();
    RenderInstance.modelMesh->Render(MVP);
    for (auto &buffer : buffers)
    {
      int loc = glGetFragDataLocation(RenderInstance.modelMesh->program, buffer.first.c_str());
      fb.DetachColour(loc);
    }
  }

  for (auto &buffer : buffers)
  {
    glBindTexture(GL_TEXTURE_2D, buffer.second);
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  // Restore default state
  glViewport(vp[0], vp[1], vp[2], vp[3]);
  fb.BindToScreen();
}

void BufferObject::DisplayFullscreenTexture(GLint texture)
{
  static Matrix4x4 identity;
  static RenderObject *fullscreenQuad = nullptr;
  if (!fullscreenQuad)
  {
    TexturedRenderObjectMaker maker;
    maker.AddVertex(vec3(-1, -1, -1), vec2(0, 0));
    maker.AddVertex(vec3(1, -1, -1), vec2(1, 0));
    maker.AddVertex(vec3(1, 1, -1), vec2(1, 1));
    maker.AddVertex(vec3(1, 1, -1), vec2(1, 1));
    maker.AddVertex(vec3(-1, 1, -1), vec2(0, 1));
    maker.AddVertex(vec3(-1, -1, -1), vec2(0, 0));
    fullscreenQuad = maker.AssembleRenderObject();
  }
  glBindTexture(GL_TEXTURE_2D, texture);
  glGenerateMipmap(GL_TEXTURE_2D);
  fullscreenQuad->AssignTexture("texture0", texture);
  fullscreenQuad->Render(identity);
}

RenderObject * FullScreenQuad(const char *fragShaderFilePath)
{
  static Matrix4x4 identity;
  TexturedRenderObjectMaker maker;
  maker.AddVertex(vec3(-1, -1, -1), vec2(0, 0));
  maker.AddVertex(vec3(1, -1, -1), vec2(1, 0));
  maker.AddVertex(vec3(1, 1, -1), vec2(1, 1));
  maker.AddVertex(vec3(1, 1, -1), vec2(1, 1));
  maker.AddVertex(vec3(-1, 1, -1), vec2(0, 1));
  maker.AddVertex(vec3(-1, -1, -1), vec2(0, 0));
  RenderObject *mesh = maker.AssembleRenderObject();
  mesh->AssignShader(SHADERDIR "TexturedRenderObject.vert", fragShaderFilePath);
  return mesh;
}
