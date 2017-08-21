#include "DrawTree.h"
#include "Window.h"
#include "FrameRate.h"
#include "Controls.h"
#include "Octree.h"
#include "Camera.h"

void WritePixel(const int &xpos, const int &ypos, const Window &window, const uint32_t &color)
{
  window.pixels[xpos + (window.height - ypos - 1) * window.width] = color;
}

// void FastDrawNode(Octree &tree, const Node &node, const float &size, const Window &window, const vec2i low, vec2i high)
// {
//   float newSize = size * 0.5;
//   if (newSize > 1.0)
//   {
//     for (int zc = 0; zc < 2; zc++)
//       for (int yc = 0; yc < 2; yc++)
//         for (int xc = 0; xc < 2; xc++)
//         {
//           Node newNode = tree.GetChild(node, xc + yc * 2 + zc * 2 * 2);
//           if (tree.GetSolid(newNode))
//             FastDrawNode(tree, newNode, newSize, window, low, high + vec3(xc, yc, zc) * newSize, MVP);
//         }
//   }
//   else
//   {
//     for (int y = low.y; y <= high.y; y++)
//       for (int x = low.x; x <= high.x; x++)
//         WritePixel(x, y, window, tree.GetColor(node));
//   }
// }

bool OverLaps(int l1, int r1, int t1, int b1, int l2, int r2, int t2, int b2)
{
  return !(l1 > r2 || r1 < l2 || b1 < t2 || t1 > b2);
}

void DrawNode(Octree &tree, const Node &node, const float &size, const Window &window, const vec3 &position, const mat4 &MVP)
{
  // Apply Viewing Matrices
  vec4 pojection = MVP * vec4(position.x, position.y, position.z, 1);
  // Apply Perspective Divide
  float invZ = 1.0 / pojection.z;
  pojection.x *= invZ;
  pojection.y *= invZ;
  float xp = (pojection.x + 1) * 0.5 * window.width;
  float yp = (pojection.y + 1) * 0.5 * window.height;
  // Draw Voxel
  float screenSize = size * invZ * (window.width * 0.5);
  vec2i lowScreenPos = { int(xp - screenSize), int(yp - screenSize) };
  vec2i highScreenPos = { int(xp + screenSize), int(yp + screenSize) };

  bool lowOnScreen = !(lowScreenPos.x < 0 | lowScreenPos.y < 0 | lowScreenPos.x >= window.width | lowScreenPos.y >= window.height) & (invZ > 0.1);
  bool highOnScreen = !(highScreenPos.x < 0 | highScreenPos.y < 0 | highScreenPos.x >= window.width | highScreenPos.y >= window.height) & (invZ > 0.1);

  if(OverLaps(lowScreenPos.x, highScreenPos.x, lowScreenPos.y, highScreenPos.y, 0, window.width, 0, window.height))

  //if (lowOnScreen | highOnScreen) // Either on screen ?
  //if (invZ < 10.0f) // Either on screen ?
  {
    //if (screenSize > 0.5f)
    if (screenSize > 1)
    //vec2i diff = (highScreenPos - lowScreenPos);
    //if (diff.x > 3 || diff.y > 3);
    {
      float newSize = size * 0.5f;
      for (int zc = 0; zc < 2; zc++)
        for (int yc = 0; yc < 2; yc++)
          for (int xc = 0; xc < 2; xc++)
          {
            Node newNode = tree.GetChild(node, xc + yc * 2 + zc * 2 * 2);
            if(tree.GetSolid(newNode))
              DrawNode(tree, newNode, newSize, window, position + vec3(xc, yc, zc) * newSize, MVP);
          }
      return;
    }
    else if (lowOnScreen & highOnScreen) // Both on screen
    {
      for (int y = lowScreenPos.y; y <= highScreenPos.y; y++)
        for (int x = lowScreenPos.x; x <= highScreenPos.x; x++)
          WritePixel(x, y, window, tree.GetColor(node));
          //WritePixel(x, y, window, 0xFFFFFFFF);
      FrameRate::AddDraw(1);
    }
    else
    {
      FrameRate::AddDraw(1);
      if (lowOnScreen) WritePixel(lowScreenPos.x, lowScreenPos.y, window, tree.GetColor(node));
      if (highOnScreen) WritePixel(highScreenPos.x, highScreenPos.y, window, tree.GetColor(node));
    }
  }


}


void DrawTree()
{
  Octree tree("f:/temp/carrickhill.oct"); // Load Model

  printf("Loading Octree... ");
  Window window("DrawTree", false, 800, 600, false); // Create Game Window
  //Window window("DrawTree", false, 1024, 768, true); // Create Game Window
  printf("Done\n");

  Controls::SetMouseLock(true);

  mat4 projectionMat; // Create Camera
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.02, 8000.0f);
  
  while (Controls::Update()) // Main Game Loop
  {
    window.Clear(0, 0, 0);

    Camera::Update(1);

    mat4 MVP;
    mat4 modelMat;
    mat4 viewMat = Camera::Matrix();
    MVP = projectionMat * viewMat * modelMat;
    //MVP.Transpose();

    DrawNode(tree, tree.RootNode(), 1.0f, window, vec3(0, 0, -0.5), MVP);

    window.Swap(); // Swap Window
    FrameRate::Update(); // FPS Counter
  }
}
