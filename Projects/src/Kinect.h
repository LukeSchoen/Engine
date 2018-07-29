// #ifndef Kinect_h__
// #define Kinect_h__
// 
// #include "stdint.h"
// 
// enum KinectRes
// {
//   KinRes_80x60,
//   KinRes_320x240,
//   KinRes_640x480,
//   KinRes_1280x960,
// };
// 
// class Kinect
// {
// public:
//   Kinect(KinectRes res = KinRes_640x480);
//   ~Kinect();
// 
//   int Width();
//   int Height();
// 
//   void GetDepth(float *pDepthImg);
//   void GetColor(uint32_t *pColorImg);
// 
//   void *m_pSensor = nullptr;
//   bool ready = false;
//   KinectRes m_res;
// };
// 
// #endif // Kinect_h__
