#include "Compression.h"
#include "ImageFile.h"
#include "Insight.h"
#include "Assets.h"
#include <memory.h>
#include <math.h>

void Compression()
{
  uint8_t input = 0;

  uint32_t w, h;
  uint32_t *output = ImageFile::ReadImage(ASSETDIR "Compression/Input.png", &w, &h);

  int64_t inputBits = 1;
  int64_t outputBits = w * h * 32;

  Insight cerebro(inputBits, outputBits);
  
  cerebro.AddExample(&input, output);

  cerebro.Train();
  memset(output, 0, w * h * 4);

  cerebro.Execute(&input, output);

  ImageFile::WriteImagePNG(ASSETDIR "Compression/Output.png", output, w, h);
}