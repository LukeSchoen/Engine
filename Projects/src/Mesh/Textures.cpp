#include "Textures.h"
#include "ImageFile.h"
#include "SDL.h"
#include <SDL_opengl.h>
#include "SDL_opengles2.h"

bool nearestNeighbour = false;
bool useTextureArrays = false;

void Textures::SetTextureFilterMode(bool smooth)
{
  nearestNeighbour = !smooth;
}

GLuint Textures::CreateTexture(uint32_t width, uint32_t height, bool depthTexture /*= false*/)
{
  GLuint ret;
  glGenTextures(1, &ret);
  glBindTexture(GL_TEXTURE_2D, ret);

  //GLfloat maximumAnistrophy;
  //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnistrophy);
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnistrophy);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (nearestNeighbour | depthTexture) ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (nearestNeighbour | depthTexture) ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

  uint32_t *img = new uint32_t[width * height];
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      img[x + y * width] = rand() + rand() * 256;
  glTexImage2D(GL_TEXTURE_2D, 0, depthTexture ? GL_DEPTH_COMPONENT32 : GL_RGBA, width, height, 0, depthTexture ? GL_DEPTH_COMPONENT : GL_RGBA, depthTexture ? GL_FLOAT : GL_UNSIGNED_BYTE, img);
  delete[] img;
  return ret;
}

GLuint Textures::LoadTextureWithScaleup(char *imagePath, int scaleRatio)
{
  uint32_t w;
  uint32_t h;
  float iscaleRatio = 1.0 / scaleRatio;
  uint32_t *img = ImageFile::ReadImage(imagePath, &w, &h);
  { // rescale image
    uint32_t nw = w * scaleRatio;
    uint32_t nh = h * scaleRatio;
    uint32_t *nimg = new uint32_t[nw * nh];
    for (uint32_t ny = 0; ny < nh; ny++)
      for (uint32_t nx = 0; nx < nw; nx++)
        nimg[nx + ny * nw] = img[int(nx * iscaleRatio) + int(ny * iscaleRatio) * w];
    delete[] img;
    img = nimg;
    w = nw;
    h = nh;
  }

  if (!img) return glUndefined;

  GLuint ret;
  glGenTextures(1, &ret);
  glBindTexture(GL_TEXTURE_2D, ret);

  if (nearestNeighbour)
  {
    GLfloat maximumAnistrophy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnistrophy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnistrophy);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  else
  {
    GLfloat maximumAnistrophy;
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnistrophy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnistrophy);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
  delete[] img;
  return ret;
}

GLuint Textures::LoadTexture(char *imagePath)
{
  uint32_t w;
  uint32_t h;
  uint32_t *img = ImageFile::ReadImage(imagePath, &w, &h);

  if (!img) return glUndefined;

  GLuint ret;
  glGenTextures(1, &ret);
  glBindTexture(GL_TEXTURE_2D, ret);

  if (nearestNeighbour)
  {
    GLfloat maximumAnistrophy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnistrophy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnistrophy);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  else
  {
    GLfloat maximumAnistrophy;
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnistrophy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnistrophy);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
  delete[] img;
  return ret;
}

GLuint Textures::LoadTextureArray(char *imagePath, const int tileSizeX, const int tileSizeY)
{
  uint32_t w;
  uint32_t h;
  uint32_t *img = ImageFile::ReadImage(imagePath, &w, &h);
  if (!img) return glUndefined;

  int imageTileSizeX = w / tileSizeX;
  int imageTileSizeY = h / tileSizeY;

  GLuint ret;
  glGenTextures(1, &ret);
  glBindTexture(GL_TEXTURE_2D_ARRAY, ret);
  GLfloat maximumAnistrophy;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnistrophy);
  glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnistrophy);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, GL_TRUE);
  //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  int mips = 0;
  int size = 8;
  while (size < w)
  {
    size <<= 1;
    mips++;
  }

  glTexStorage3D(GL_TEXTURE_2D_ARRAY, mips, GL_RGBA8, tileSizeX, tileSizeY, imageTileSizeX * imageTileSizeY);
  int tileSize = tileSizeX * tileSizeY;
  uint32_t *tile = new uint32_t[tileSizeX * tileSizeY * imageTileSizeX * imageTileSizeY];
  int layer = 0;
  for (int y = 0; y < imageTileSizeY; y++)
    for (int x = 0; x < imageTileSizeX; x++)
    {
      for (int ty = 0; ty < tileSizeY; ty++)
        for (int tx = 0; tx < tileSizeX; tx++)
          tile[tx + ty * tileSizeX + layer * tileSize] = img[(tx + x * tileSizeX) + (ty + y * tileSizeY) * w];
      layer++;
    }
  glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, tileSizeX, tileSizeY, imageTileSizeX * imageTileSizeY, GL_RGBA, GL_UNSIGNED_BYTE, tile);

  delete[] img;
  delete[] tile;
  return ret;
}

bool Textures::GetTextureArrayMode()
{
  return useTextureArrays;
}
