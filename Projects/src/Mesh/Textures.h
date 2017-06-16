#ifndef Textures_h__
#define Textures_h__
#include "RenderObject.h"

struct Textures
{
  static void SetTextureFilterMode(bool smooth);
  static GLuint CreateTexture(uint32_t width, uint32_t height, bool depthTexture = false);
  static GLuint LoadTexture(char *imagePath);
  static GLuint LoadTextureWithScaleup(const char *imagePath, int scaleRatio);
  static GLuint LoadTextureArray(char *imagePath, const int tileSizeX, const int tileSizeY);
  static bool GetTextureArrayMode();
};

#endif // Textures_h__