#ifndef Window_h__
#define Window_h__
#define GLEW_STATIC
#include <stdint.h>
#include <gl\glew.h>
#include <gl\glu.h>
#include "SDL.h"
#include <SDL_opengl.h>
#include "SDL_opengles2.h"

struct Window
{
  SDL_Window *window;
  uint32_t *pixels;
  uint32_t width;
  uint32_t height;
  SDL_GLContext glContext;

  Window(char * windowTitle = "Program", bool OpenGL = false, int width = 800, int height = 600, bool fullscreen = false);
  void Clear(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0);
  void Swap(bool responsive = false);
  ~Window();
private:
  bool OpenGLEnabled;
};

#endif // Window_h__