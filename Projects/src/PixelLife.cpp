// #include "PixelLife.h"
// #include "Window.h"
// #include "Controls.h"
// 
// void PixeLife()
// {
//   Window window("PixelLife", false, 1920, 1080, true); // Create Game Window
// 
//   while (Controls::Update()) // Main Game Loop
//   {
//     window.Clear(0, 0, 0);
// 
//     _AddRandomAgent();
// 
//     _UpdateWorld();
// 
//     _DrawWorld(window, zoom);
// 
//     window.Swap(); // Swap Window
// 
//     FrameRate::Update();
// 
//     if (Controls::KeyDown(SDL_SCANCODE_1))
//       SDL_Delay(50);
//     if (Controls::KeyDown(SDL_SCANCODE_2))
//       SDL_Delay(100);
//     if (Controls::KeyDown(SDL_SCANCODE_3))
//       SDL_Delay(200);
//   }
// 
// 
// 
// }
