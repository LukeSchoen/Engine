#ifndef DynamicTextureAtlas_h__
#define DynamicTextureAtlas_h__

#include "SDL_config_winrt.h"
#include "SDL_opengl.h"
#include "GLtypes.h"
#include "RectPacker.h"
#include "FastPack.h"

//#pragma optimize("", off) // debug

struct DynamicTextureArrayAtlas
{
  DynamicTextureArrayAtlas(int _width = 1024, int _height = 1024, int _layers = 1)
    : atlas(_width, _height)
  {
    height = _height;
    width = _width;
    GLint max_layers;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers);
    layers = Min(_layers, max_layers);

    //packers = (RectPacker*) malloc(sizeof(RectPacker) * layers);
    //for (int i = 0; i < layers; i++)
    //  new (packers + i) RectPacker(width, height, false);

    packers = (FastPacker*)malloc(sizeof(FastPacker) * layers);
    for (int i = 0; i < layers; i++)
      new (packers + i) FastPacker(width, height);

    image = new uint32_t[width * height * layers];

    for (int z = 0; z < layers; z++)
      for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
          image[x + y * width + z * width * height] = 0xFFFF0000;
  }

  ~DynamicTextureArrayAtlas()
  {
    delete[] image;
    free(packers);
    //DELETE texture here
  }

  void UploadToGPU()
  {
    if (texture == glUndefined)
    {
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
      glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, GL_FALSE);
      glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layers);
    }
    if (updates.size() == 0) return;
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    for (int rectID = 0; rectID < updates.size(); rectID++)
    {
      if (rectID == 8041)
        rectID = rectID;
      Rect &rect = updates[rectID];
      uint32_t *img = new uint32_t[rect.width * rect.height];
      for (int y = 0; y < rect.height; y++)
        for (int x = 0; x < rect.width; x++)
          img[x + y * rect.width] = image[rect.xpos + x + ((rect.ypos + y) * width) + (rect.layer * width * height)];
      glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, rect.xpos, rect.ypos, rect.layer, rect.width, rect.height, 1, GL_RGBA, GL_UNSIGNED_BYTE, img);
      delete[] img;
    }
    updates.clear();
  }

  bool AddTile(uint32_t *_image, int _width, int _height, int _layer, vec2 *TopLeftUVs, vec2 *BotRightUVs)
  {
    Rect rect(_width, _height);
    rect.layer = _layer;
    if (!packers[_layer].InsertRect(rect))
      return false;
    for (int y = 0; y < rect.height; y++)
      for (int x = 0; x < rect.width; x++)
        image[rect.xpos + x + ((rect.ypos + y) * width) + (_layer * width * height)] = _image[x + y * rect.width];
    updates.push_back(rect);

    (*TopLeftUVs).x = (rect.xpos) / float(width);
    (*TopLeftUVs).y = ((rect.ypos) / float(height));

    (*BotRightUVs).x = ((rect.xpos) + rect.width) / float(width);
    (*BotRightUVs).y = ((rect.ypos) + rect.height) / float(height);
    return true;
  }

  bool AddBorderedTile(uint32_t *_image, int _width, int _height, int _layer, vec2 *TopLeftUVs, vec2 *BotRightUVs)
  {
    int w = _width + 2;
    int h = _height + 2;
    static uint32_t img[1024 * 1024];

    // Copy image
    for (int y = 0; y < _height; y++)
      for (int x = 0; x < _width; x++)
        img[x + 1 + (y + 1) * w] = _image[x + y * _width];

    // Create borders
    for (int x = 0; x < _width; x++)
    {
      img[x + 1 + (0) * w] = _image[x + 0 * _width];
      img[x + 1 + (h - 1) * w] = _image[x + (_height - 1) * _width];
    }
    for (int y = 0; y < _height; y++)
    {
      img[0 + (y + 1) * w] = _image[0 + y * _width];
      img[w - 1 + (y + 1) * w] = _image[_width - 1 + y * _width];
    }
    // Corners
    img[0 + (0) * w] = _image[0 + 0 * _width];
    img[w - 1 + (0) * w] = _image[_width - 1 + 0 * _width];
    img[0 + (h - 1) * w] = _image[0 + (_height - 1) * _width];
    img[w - 1 + (h - 1) * w] = _image[_width - 1 + (_height - 1) * _width];

    bool res = AddTile(img, w, h, _layer, TopLeftUVs, BotRightUVs);
    *TopLeftUVs = *TopLeftUVs + vec2(1.0f / width, 1.0f / height);
    *BotRightUVs = *BotRightUVs - vec2(1.0f / width, 1.0f / height);
    return res;
  }

  void ClearLayer(int _layer)
  {
    packers[_layer].Clear();
    new (packers + _layer) FastPacker(width, height);
  }

  int width = 0;
  int height = 0;
  int layers = 0;

  uint32_t *image = nullptr;
  GLuint texture = glUndefined;
  FastPacker *packers;
  Rect atlas;
  std::vector<Rect> updates;
};

#endif // DynamicTextureAtlas_h__