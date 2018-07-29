// #include "Kinect.h"
// 
// #include "stdint.h"
// #include <Windows.h>
// #include <Ole2.h>
// 
// #include <NuiApi.h>
// #include <NuiImageCamera.h>
// #include <NuiSensor.h>
// #include <intsafe.h>
// #include <vector>
// 
// HANDLE m_hNextDepthFrameEvent;
// HANDLE m_hNextColorFrameEvent;
// HANDLE m_pDepthStreamHandle;
// HANDLE m_pColorStreamHandle;
// 
// Kinect::Kinect(KinectRes res /*= KinRes_640x480*/)
// {
//   ready = false;
//   int numSensors;
//   if (NuiGetSensorCount(&numSensors) < 0 || numSensors < 1) return;
//   for (int i = 0; i < numSensors; ++i)
//   {
//     INuiSensor* newSensor;
//     if (FAILED(NuiCreateSensorByIndex(i, &newSensor))) continue;
//     if (newSensor->NuiStatus() == S_OK)
//     {
//       m_pSensor = newSensor;
//       break;
//     }
//     newSensor->Release();
//   }
//   if (m_pSensor)
//   {
//     if (SUCCEEDED(((INuiSensor*)m_pSensor)->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR)))
//     {
//       m_hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//       m_hNextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//       ((INuiSensor*)m_pSensor)->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, (_NUI_IMAGE_RESOLUTION)(NUI_IMAGE_RESOLUTION_80x60 + res), 0, 2, m_hNextDepthFrameEvent, &m_pDepthStreamHandle);
//       ((INuiSensor*)m_pSensor)->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, (_NUI_IMAGE_RESOLUTION)(NUI_IMAGE_RESOLUTION_80x60 + res), 0, 2, m_hNextColorFrameEvent, &m_pColorStreamHandle);
//     }
//   }
//   m_res = res;
//   ready = true;
// }
// 
// Kinect::~Kinect()
// {
//   ready = false;
//   ((INuiSensor*)m_pSensor)->NuiShutdown();
// }
// 
// int Kinect::Width()
// {
//   if (m_res == KinRes_80x60) return 80;
//   if (m_res == KinRes_320x240) return 320;
//   if (m_res == KinRes_640x480) return 640;
//   if (m_res == KinRes_1280x960) return 1280;
// }
// 
// int Kinect::Height()
// {
//   if (m_res == KinRes_80x60) return 60;
//   if (m_res == KinRes_320x240) return 240;
//   if (m_res == KinRes_640x480) return 480;
//   if (m_res == KinRes_1280x960) return 960;
// }
// 
// void Kinect::GetColor(uint32_t *pColorImg)
// {
//   NUI_IMAGE_FRAME imageFrame;
//   INuiFrameTexture* pTexture;
//   if (FAILED(((INuiSensor*)m_pSensor)->NuiImageStreamGetNextFrame(m_pColorStreamHandle, 0, &imageFrame))) return;
//   NUI_LOCKED_RECT LockedRect;
//   pTexture = imageFrame.pFrameTexture;
//   pTexture->LockRect(0, &LockedRect, NULL, 0);
//   int w = Width();
//   int h = Height();
//   for (int y = 0; y < h; y++)
//     for (int x = 0; x < w; x++)
//     {
//       auto rgb = &(LockedRect.pBits[(x + y * w) * 4]);
//       uint32_t col = rgb[0] | (rgb[1] << 8) | (rgb[2] << 16);
//       pColorImg[(w - x - 1) + y * w] = col;
//     }
//   pTexture->UnlockRect(0);
//   ((INuiSensor*)m_pSensor)->NuiImageStreamReleaseFrame(m_pColorStreamHandle, &imageFrame);
// }
// 
// void Kinect::GetDepth(float *pDepthImg)
// {
//   BOOL nearMode;
//   NUI_IMAGE_FRAME imageFrame;
//   INuiFrameTexture* pTexture;
//   if (FAILED(((INuiSensor*)m_pSensor)->NuiImageStreamGetNextFrame(m_pDepthStreamHandle, 0, &imageFrame))) return;
//   if (FAILED(((INuiSensor*)m_pSensor)->NuiImageFrameGetDepthImagePixelFrameTexture(m_pDepthStreamHandle, &imageFrame, &nearMode, &pTexture))) return (void)((INuiSensor*)m_pSensor)->NuiImageStreamReleaseFrame(m_pDepthStreamHandle, &imageFrame);
//   NUI_LOCKED_RECT LockedRect;
//   pTexture->LockRect(0, &LockedRect, NULL, 0);
//   if (LockedRect.Pitch != 0)
//   {
//     int w = Width();
//     int h = Height();
//     for (int y = 0; y < h; y++)
//       for (int x = 0; x < w; x++)
//       {
//         auto pDepth = reinterpret_cast<const NUI_DEPTH_IMAGE_PIXEL*>(LockedRect.pBits)[x + y * w].depth;
//         pDepthImg[(w - x - 1) + y * w] = pDepth / (float)USHORT_MAX;
//       }
//   }
//   pTexture->UnlockRect(0);
//   pTexture->Release();
//   ((INuiSensor*)m_pSensor)->NuiImageStreamReleaseFrame(m_pDepthStreamHandle, &imageFrame);
// }
// 
// //void Kinect::GetColorInDepthSpace(uint32_t *pColorImg)
// //{
// //  static uint32_t *colBuff = new uint32_t[Width() * Height()];
// //  static uint16_t *pDepth = new uint16_t[Width() * Height()];
// //  static int32_t *pColorOffset = new int32_t[Width() * Height() * 2];
// //  GetColor(colBuff);
// //  //GetDepth(pDepth);
// //  ((INuiSensor*)sensor)->NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution((_NUI_IMAGE_RESOLUTION)(NUI_IMAGE_RESOLUTION_80x60 + m_res), (_NUI_IMAGE_RESOLUTION)(NUI_IMAGE_RESOLUTION_80x60 + m_res), Width() * Height(), pDepth, Width() * Height() * 2, (LONG*)pColorOffset);
// //  for (int y = 0; y < Height(); ++y)
// //  {
// //    for (int x = 0; x < Width(); ++x)
// //    {
// //      int colorInDepthX = pColorOffset[(x + y * Width()) * 2];
// //      int colorInDepthY = pColorOffset[(x + y * Width()) * 2 + 1];
// //      uint32_t col = 0;
// //      if (colorInDepthX >= 0 && colorInDepthX < Width() && colorInDepthY >= 0 && colorInDepthY < Height())
// //        pColorImg[colorInDepthX + colorInDepthY * Width()] = colBuff[x + y * Width()];
// //    }
// //  }
// //}
