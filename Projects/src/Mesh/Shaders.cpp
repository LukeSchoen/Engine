#include "Shaders.h"
#include "Cache.h"
#include <string>

int loadedCount = 0;

Cache shaderCache;

void printProgramLog(GLuint program)
{
  //Make sure name is shader
  if (glIsProgram(program))
  {
    //Program log length
    int infoLogLength = 0;
    int maxLength = infoLogLength;

    //Get info string length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    //Allocate string
    char* infoLog = new char[maxLength];

    //Get info log
    glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0)
    {
      //Print Log
      printf("%s\n", infoLog);
    }

    //Deallocate string
    delete[] infoLog;
  }
  else
  {
    printf("Name %d is not a program\n", program);
  }
}

void printShaderLog(GLuint shader)
{
  //Make sure name is shader
  if (glIsShader(shader))
  {
    //Shader log length
    int infoLogLength = 0;
    int maxLength = infoLogLength;

    //Get info string length
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    //Allocate string
    char* infoLog = new char[maxLength];

    //Get info log
    glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0)
    {
      //Print Log
      printf("%s\n", infoLog);
    }

    //Deallocate string
    delete[] infoLog;
  }
  else
  {
    printf("Name %d is not a shader\n", shader);
  }
}

GLuint Shaders::Load(const char *vertShaderFile, const char *fragShaderFile, const char *geomShaderFile)
{
  // Check Cache First
  std::string key(vertShaderFile);
  key = key + "&";
  key = key + fragShaderFile;
  if (geomShaderFile)
  {
    key = key + "&";
    key = key + geomShaderFile;
  }
  GLuint *SGLID = (GLuint*)shaderCache.GetItem(key.c_str());
  if (SGLID != nullptr)
    return *SGLID;

  //Generate shader program
  GLuint gProgramID = glCreateProgram();

  //Create vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

  //Get vertex source
  char *vertShader[1];
  vertShader[0] = (char*)File::ReadFile(vertShaderFile, nullptr, true);

  //Set vertex source
  glShaderSource(vertexShader, 1, vertShader, NULL);

  //Compile vertex source
  glCompileShader(vertexShader);

  //Check vertex shader for errors
  GLint vShaderCompiled = GL_FALSE;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
  if (vShaderCompiled != GL_TRUE)
  {
    printf("Unable to compile vertex shader: %s\n", vertShaderFile);
    printShaderLog(vertexShader);
    getchar();
    return glUndefined;
  }

  //Attach vertex shader to program
  glAttachShader(gProgramID, vertexShader);

  //Create fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  //Get fragment source
  char *fragShader[1];
  fragShader[0] = (char*)File::ReadFile(fragShaderFile, nullptr, true);

  //Set fragment source
  glShaderSource(fragmentShader, 1, fragShader, NULL);

  //Compile fragment source
  glCompileShader(fragmentShader);

  //Check fragment shader for errors
  GLint fShaderCompiled = GL_FALSE;
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
  if (fShaderCompiled != GL_TRUE)
  {
    printf("Unable to compile fragment shader: %s\n", fragShaderFile);
    printShaderLog(fragmentShader);
    getchar();
    return glUndefined;
  }

  //Attach fragment shader to program
  glAttachShader(gProgramID, fragmentShader);

  if (geomShaderFile)
  {
    //Create Geometry shader
    GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    //Get Geometry source
    char *geomShader[1];
    geomShader[0] = (char*)File::ReadFile(geomShaderFile, nullptr, true);

    //Set Geometry source
    glShaderSource(geometryShader, 1, geomShader, NULL);

    //Compile Geometry source
    glCompileShader(geometryShader);

    //Check Geometry shader for errors
    GLint gShaderCompiled = GL_FALSE;
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &gShaderCompiled);
    if (gShaderCompiled != GL_TRUE)
    {
      printf("Unable to compile geometry shader: %s\n", geomShaderFile);
      printShaderLog(geometryShader);
      getchar();
      return glUndefined;
    }

    //Attach geometry shader to program
    glAttachShader(gProgramID, geometryShader);
  }

  //Link program
  glLinkProgram(gProgramID);

  //Check for errors
  GLint programSuccess = GL_TRUE;
  glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
  if (programSuccess != GL_TRUE)
  {
    printf("Error linking program %d!\n", gProgramID);
    printProgramLog(gProgramID);
    return glUndefined;
  }

  //success
  // Add Loaded Shader To Cache
  GLuint *res = new GLuint;
  *res = gProgramID;
  shaderCache.AddItem(strdup(key.c_str()), res);
  loadedCount++;

  return gProgramID;
}

void Shaders::Report()
{
  printf("programs loaded: %d\n", loadedCount);
  loadedCount = 0;
}
