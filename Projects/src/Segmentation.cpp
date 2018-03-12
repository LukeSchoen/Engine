#include "Segmentation.h"
#include "Window.h"
#include "Controls.h"
#include "ImageFile.h"
#include "MeanShiftSeg.h"

vec2i WeightedRandomPoint(vec2i startPos, vec2i imageSize, float maxDist)
{
  float dist = (rand() % int(maxDist * 10000)) / 10000.f;
  vec2i selection = { rand() % imageSize.x, rand() % imageSize.y };
  //selection = (selection * dist) + (startPos * (1.0 - dist));
  selection = vec2i((selection.x * dist) + (startPos.x * (1.0 - dist))   , (selection.y * dist) + (startPos.y * (1.0 - dist)));
  return selection;
}

void Segmentation()
{
  Window window;

  MeanShiftSeg::ApplySegmentation("C:/Users/Luke/Desktop/segment.png", "C:/Users/Luke/Desktop/output.png", 7, 6.5, 20);
  exit(0);

  uint32_t imgSizeX, imgSizeY;
  uint32_t *img = ImageFile::ReadImage("C:/Users/Luke/Desktop/segment.png", &imgSizeX, &imgSizeY);

  while (Controls::Update())
  {
    if(Controls::KeyDown(SDL_SCANCODE_SPACE))
      img = ImageFile::ReadImage("C:/Users/Luke/Desktop/segment.png", &imgSizeX, &imgSizeY);

    window.Clear();


    // Display Image
    for (int y = 0; y < imgSizeY; y++) for (int x = 0; x < imgSizeX; x++) window.pixels[x + y * window.width] = img[x + y * imgSizeX];

    window.Swap();
  }
}
