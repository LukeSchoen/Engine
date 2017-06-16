#include "Compression.h"
#include "ImageFile.h"
#include "Insight.h"
#include "Assets.h"
#include <memory.h>
#include <math.h>
#include "Window.h"
#include "Controls.h"
#include "FrameRate.h"

void _AddUnique(vec2i pos, std::vector<vec2i>& vec)
{
  for (auto & v : vec)
    if (v == pos)
      return;
  vec.push_back(pos);
}

void Compression()
{
  Window window("Magic Brush");

  const int regionSize = 4;
  const int drawingSize = 3;

  // Load image
  uint32_t w, h;
  uint32_t *image = ImageFile::ReadImage(ASSETDIR "Compression/Park.jpg", &w, &h);
  for (int y = 0; y < h; y++) for (int x = 0; x < w; x++) window.pixels[x + y * window.width] = image[x + y * w]; // Draw Image

  std::vector<vec2i> sky;
  std::vector<vec2i> nonSky;

  while (Controls::Update()) // Main Loop
  {
    vec2i mouse = Controls::GetMouse();
    bool mouseLeft = Controls::GetLeftClick();
    bool mouseRight = Controls::GetRightClick();

    static bool wasWorking = false;
    bool working = mouseLeft || mouseRight;

    // Training
    if (mouseLeft)
      for (int y = -drawingSize; y <= drawingSize; y++)
        for (int x = -drawingSize; x <= drawingSize; x++)
        {
          vec2i pos(mouse.x + x, mouse.y + y);
          if ((pos.x >= regionSize && pos.x < w - regionSize) && (pos.y >= regionSize && pos.y < h - regionSize))
          {
            _AddUnique(pos, sky);
            window.pixels[pos.x + pos.y * window.width] = 0xFFFFFFFF;
          }
        }

    if (mouseRight)
      for (int y = -drawingSize; y <= drawingSize; y++)
        for (int x = -drawingSize; x <= drawingSize; x++)
        {
          vec2i pos(mouse.x + x, mouse.y + y);
          if ((pos.x >= regionSize && pos.x < w - regionSize) && (pos.y >= regionSize && pos.y < h - regionSize))
          {
            _AddUnique(vec2i(mouse.x + x, mouse.y + y), nonSky);
            window.pixels[pos.x + pos.y * window.width] = 0xFFFFFFFF;
          }
        }

    // Magic
    if (!working && wasWorking)
    {
      printf("Working...");
      for (int y = 0; y < h; y++) for (int x = 0; x < w; x++) window.pixels[x + y * window.width] = image[x + y * w]; // Draw Image

      const int RegionCrossSection = regionSize * 2 + 1;
      const int RegionArea = RegionCrossSection * RegionCrossSection;

      Insight cerebro(sizeof(uint32_t) * RegionArea, 1);

      for (auto & v : sky)
      {
        static std::vector<uint32_t> input;
        static char t = 255;
        input.clear();
        for (int y = v.y - regionSize; y <= v.y + regionSize; y++)
          for (int x = v.x - regionSize; x <= v.x + regionSize; x++)
            input.push_back(image[x + y * w]);
        cerebro.AddExample(input.data(), &t);
      }

      for (auto & v : nonSky)
      {
        static std::vector<uint32_t> input;
        static char f = 0;
        input.clear();
        for (int y = v.y - regionSize; y <= v.y + regionSize; y++)
          for (int x = v.x - regionSize; x <= v.x + regionSize; x++)
            input.push_back(image[x + y * w]);
        cerebro.AddExample(input.data(), &f);
      }

      cerebro.Train();

      for (int y = regionSize; y < h - regionSize; y++)
        for (int x = regionSize; x < w - regionSize; x++)
        {
          static std::vector<uint32_t> input;
          input.clear();
          for (int iy = y - regionSize; iy <= y + regionSize; iy++)
            for (int ix = x - regionSize; ix <= x + regionSize; ix++)
              input.push_back(image[ix + iy * w]);
          char res = 0;
          cerebro.Execute(input.data(), &res);
          if (res)
            window.pixels[x + y * window.width] = 0xFF0000FF;
        }

      printf(" Done\n");
    }

    wasWorking = working;

    window.Swap();
    FrameRate::Update();
  }


  exit(0);

  uint8_t input = 0;


  int64_t inputBits = 1;
  int64_t outputBits = w * h * 32;

  Insight cerebro(inputBits, outputBits);
  
  cerebro.AddExample(&input, image);

  cerebro.Train();
  memset(image, 0, w * h * 4);

  cerebro.Execute(&input, image);

  ImageFile::WriteImagePNG(ASSETDIR "Compression/Output.png", image, w, h);
}