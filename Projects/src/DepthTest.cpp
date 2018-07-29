// #include "DepthTest.h"
// #include "Window.h"
// #include "Kinect.h"
// #include "Controls.h"
// 
// void DepthTest()
// {
//   Kinect kinect(KinRes_640x480);
// 
//   Window window("Depth Test", false, kinect.Width(), kinect.Height(), false);
// 
//   uint32_t *pColor = new uint32_t[kinect.Width() * kinect.Height()];
//   float *pDepth = new float[kinect.Width() * kinect.Height()];
// 
//   while (Controls::Update())
//   {
//     window.Clear();
// 
//     kinect.GetColor(pColor);
//     kinect.GetDepth(pDepth);
// 
//     // Display Depth
//     if (!Controls::KeyDown(SDL_SCANCODE_1))
//       for (int y = 1; y < window.height - 1; y++)
//         for (int x = 1; x < window.width - 1; x++)
//           pColor[x + y * window.width] = pDepth[x + y * window.width] * 123456789;
// 
//     // Display Color
//     for (int y = 0; y < window.height; y++)
//       for (int x = 0; x < window.width; x++)
//         window.pixels[x + y * window.width] = pColor[x + y * window.width];
// 
//     window.Swap();
//   }
//   delete[] pDepth;
// }
