#ifndef ImageFile_h__
#define ImageFile_h__
#include "stdint.h"

struct ImageFile
{
  static uint32_t *ReadImage(const char *imagePath, uint32_t *width, uint32_t *height);

  static bool WriteImagePNG(const char *imagePath, uint32_t *RGBA_Data, uint32_t width, uint32_t height);
};

#endif // ImageFile_h__
