#include "ImageFile.h"
#include "File.h"
#include "SDL.h"
#include "SDL_image.h"
#include "Maths.h"

uint32_t *ImageFile::ReadImage(const char *imagePath, uint32_t *width, uint32_t *height)
{
  SDL_Surface *rawImg = IMG_Load(imagePath);

  if (!rawImg)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    return nullptr;
  }

  uint32_t &w = *width;
  uint32_t &h = *height;

  w = rawImg->w;
  h = rawImg->h;

  uint32_t *img = new uint32_t[w * h];

  uint8_t *imgData = (uint8_t*)rawImg->pixels;
  uint32_t address, r, g, b, a, rOffset, gOffset, bOffset, aOffset;
  rOffset = (rawImg->format->Rshift / 8);
  gOffset = (rawImg->format->Gshift / 8);
  bOffset = (rawImg->format->Bshift / 8);
  aOffset = (rawImg->format->Ashift / 8);
  int step = rawImg->format->BytesPerPixel;
  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      address = x * step + y * rawImg->pitch;
      r = imgData[address + rOffset];
      g = imgData[address + gOffset];
      b = imgData[address + bOffset];
      a = 255;
      //a = imgData[address + aOffset];
      img[x + y * w] = b | (g << 8) | (r << 16) | (a << 24);
    }
  }

  SDL_FreeSurface(rawImg);

  return img;
}

bool ImageFile::WriteImagePNG(const char *imagePath, uint32_t *RGBA_Data, uint32_t width, uint32_t height)
{
  SDL_Surface* img = SDL_CreateRGBSurface(0, width, height, 32, 255, 255 << 8, 255 << 16, 255 << 24);

  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      *((uint32_t*)(((uint8_t*)img->pixels) + (x * sizeof(uint32_t) + y * img->pitch))) = RGBA_Data[x + y * width];

  int res = IMG_SavePNG(img, imagePath) == -1;

  SDL_FreeSurface(img);

  if (res)
  {
    printf("IMG_SavePng: %s\n", IMG_GetError());
    return false;
  }

  return true;
}
