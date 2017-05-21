#include "Window.h"
#include "math/Maths.h"

Window::Window(char * windowTitle /*= "Program"*/, bool OpenGL /*= false*/, int Width /*= 800*/, int Height /*= 600*/, bool Fullscreen /*= false*/)
{
  OpenGLEnabled = OpenGL;

  SDL_Init(SDL_INIT_EVERYTHING); // Initialize SDL

  SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);

  if (OpenGL) // PreWindow Creation GL Options
  {
    // VSYNC
    SDL_GL_SetSwapInterval(1);

    // Do these help?
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    //glEnable(GL_LINE_SMOOTH);
    //glEnable(GL_POLYGON_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_MULTISAMPLE);

    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // OpenGL 3.1 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // Anti Aliasing
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
  }

  int windowPos = SDL_WINDOWPOS_UNDEFINED;
  window = SDL_CreateWindow(windowTitle, windowPos, windowPos, Width, Height, (SDL_WINDOW_OPENGL * OpenGL) | SDL_WINDOW_SHOWN | (SDL_WINDOW_FULLSCREEN * Fullscreen));
  pixels = (uint32_t*)SDL_GetWindowSurface(window)->pixels;
  int cWidth, cHeight;
  SDL_GetWindowSize(window, &cWidth, &cHeight);
  width = cWidth;
  height = cHeight;
  width = Width;
  height = Height;

  if (OpenGL) // PostWindow Creation GL Options
  {
    glContext = SDL_GL_CreateContext(window); // Create OpenGL Context
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit(); // Initialize glew
    glEnable(GL_DEPTH_TEST); // Enable Depth Testing
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE); // Enable Multi sampling
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}

void Window::Clear(unsigned char red, unsigned char green, unsigned char blue)
{
  if (OpenGLEnabled)
  {
    glClearColor(red / 255.0f, green / 255.0f, blue / 255.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  else
  {
    uint32_t color = blue | green << 8 | red << 16;
    for (int y = 0; y < height; y++)
      for (int x = 0; x < width; x++)
        pixels[x + y * width] = color;
  }
}

int Window::GetRefreshRate()
{
  static int RR = 0;
  if (RR == 0)
  {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_DisplayMode mode;
    for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
    {
      SDL_GetCurrentDisplayMode(i, &mode);
      RR = Max(mode.refresh_rate, RR);
    }
  }
  return RR;
}

vec2i Window::GetScreenRes()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  static int w = 0;
  static int h = 0;
  if (w == 0)
  {
    SDL_DisplayMode mode;
    for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
    {
      SDL_GetCurrentDisplayMode(i, &mode);
      w = Max(w, mode.w);
      h = Max(h, mode.h);
    }
  }
  return vec2i(w, h);
}

void Window::Swap(bool responsive)
{
  if (OpenGLEnabled)
  {
    SDL_GL_SwapWindow(window);
    if (responsive)
    {
      static float averageFPS = 0;

      static int drawTime = 0;
      if (averageFPS > GetRefreshRate() / 2) // If were running well then decrease latency!
        glFinish();
      drawTime = SDL_GetTicks() - drawTime;
      averageFPS = ((averageFPS * 9) + (1000 / drawTime)) / 10;
      static int refreshTime = floor(1000.0f / GetRefreshRate()) - 2;
      int freshWait = Min(Max(refreshTime - drawTime, 0), refreshTime);
      if(averageFPS + 5 > GetRefreshRate()) // If were running really well then further decrease latency!
        SDL_Delay(freshWait);
      drawTime = SDL_GetTicks();

      // simple perfotce measure
      if(false)
      {
        if (averageFPS + 5 > GetRefreshRate())
          printf("great\n");
        else if (averageFPS > GetRefreshRate() / 2)
          printf("good\n");
        else
          printf("bad\n");
      }
    }

  }
  else
    SDL_UpdateWindowSurface(window);
}

Window::~Window()
{
  SetThreadExecutionState(ES_CONTINUOUS);

  SDL_DestroyWindow(window);
  SDL_Quit();
}
