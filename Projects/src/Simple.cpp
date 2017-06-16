// #include "Simple.h"
// #include "Camera.h"
// #include "Window.h"
// #include "Controls.h"
// #include "RenderObject.h"
// #include "CustomRenderObjects.h"
// #include <algorithm>
// #include "Matrix4x4.h"
// 
// enum SegType
// {
//   root,
//   leaf,
//   wood,
//   stalk,
// };
// 
// enum GeneConditionEnum
// {
//   WITHIN_X_DIST_OF_GROUND,
//   LESS_THAN_X_SEGS_OF_TYPE_Y,
//   WITHIN_X_DIST_OF_SEG_TYPE_Y,
// };
// 
// enum GeneActionEnum
// {
//   SET_POS_X,
//   SET_POS_Y,
//   SET_POS_Z,
//   SET_ANG_X,
//   SET_ANG_Y,
//   SET_ANG_Z,
//   SET_SCALE_X,
//   SET_SCALE_Y,
//   SET_SCALE_Z,
//   SET_BRANCH_0,
//   SET_BRANCH_1,
//   SET_BRANCH_2,
//   SET_BRANCH_3,
//   SET_BRANCH_4,
//   SET_BRANCH_5,
// };
// 
// struct GeneCondition
// {
//   float x, y;
//   GeneConditionEnum genecondition;
//   bool inverted;
// };
// 
// struct GeneAction
// {
//   float x;
//   GeneActionEnum geneaction;
// };
// 
// struct Segment
// {
//   SegType type;
//   vec3 pos;
//   vec3 ang;
//   vec3 scale;
// };
// 
// struct Gene
// {
//   std::vector<GeneCondition> conditions;
//   std::vector<GeneAction> actions;
// };
// 
// class Plant
// {
// public:
//   std::vector<Segment> segments;
//   std::vector<Gene> genome;
//   float energy = 100;
// 
//   int AddSegment(vec3 pos)
//   {
//     Segment segment;
//     segment.pos = pos;
//     segment.ang;
//     segment.scale = vec3(1, 1, 1);
//     int childCount = Express(segment);
//     mat4 rotmat;
//     rotmat.Rotate(segment.ang);
//     vec3 endPoint = segment.pos + vec3(rotmat * vec4(1, 0, 0)) * segment.scale;
//     for (int c = 0; c < childCount; c++)
//       AddSegment(endPoint);
// 
//     segments.push_back(segment);
//   }
// 
//   void SpawnPlant(vec3 pos)
//   {
//     AddSegment(pos);
//   }
// 
//   int Express(Segment &segment)
//   {
//     int branchCount = 0;
//     for (auto & gene : genome)
//     {
//       // Check Conditions
//       bool  run = true;
//       for (auto & condition : gene.conditions)
//       {
//         switch (condition.genecondition)
//         {
//         case WITHIN_X_DIST_OF_GROUND:
//           bool good = condition.x < std::max(segment.pos.y, 0.f);
//           if (condition.inverted) good = !good;
//           run = run & good;
//           break;
// 
//         case LESS_THAN_X_SEGS_OF_TYPE_Y:
//           int count = 0;
//           bool good;
//           for (auto & set : segments)
//             count++;
//           good = count > 0;
//           if (condition.inverted) good = !good;
//           run = run & good;
//           break;
// 
//         case WITHIN_X_DIST_OF_SEG_TYPE_Y:
// 
//           break;
// 
//         default:
//           break;
//         }
//       }
// 
//       // Execute actions
//       if (run)
//       {
//         for (auto & action : gene.actions)
//         {
//           switch (action.geneaction)
//           {
//           //case SET_POS_X: segment.pos.x = action.x; break;
//           //case SET_POS_Y: segment.pos.y = action.x; break;
//           //case SET_POS_Z: segment.pos.z = action.x; break;
// 
//           case SET_ANG_X: segment.ang.x = action.x; break;
//           case SET_ANG_Y: segment.ang.y = action.x; break;
//           case SET_ANG_Z: segment.ang.z = action.x; break;
//           case SET_SCALE_X: segment.scale.x = action.x; break;
//           case SET_SCALE_Y: segment.scale.y = action.x; break;
//           case SET_SCALE_Z: segment.scale.z = action.x; break;
//           case SET_BRANCH_0: branchCount = 0; break;
//           case SET_BRANCH_1: branchCount = 1; break;
//           case SET_BRANCH_2: branchCount = 2; break;
//           case SET_BRANCH_3: branchCount = 3; break;
//           case SET_BRANCH_4: branchCount = 4; break;
//           case SET_BRANCH_5: branchCount = 5; break;
// 
//           default: break;
//           }
//         }
//       }
// 
//     }
// 
//   }
// 
//   RenderObject* Express()
//   {
// 
//     maker.AddVertex({ 0, 0, 0 }, { 1, 1, 1, });
//     maker.AddVertex({ 0, 1, 0 }, { 1, 1, 1, });
//     maker.AddVertex({ 1, 1, 0 }, { 1, 1, 1, });
//     RenderObject *pModel = maker.AssembleRenderObject();
//   }
// 
//   ColouredRenderObjectMaker maker;
// 
// };
// 
// 
// void Simple()
// {
//   // Window
//   Window window("Simple Voxels", true);
// 
//   // Camera
//   mat4 projectionMat;
//   projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 1.0 / 4.0, 515);
//   mat4 MVP;
//   mat4 modelMat;
//   Controls::SetMouseLock(true);
// 
//   // Model
//   ColouredRenderObjectMaker maker;
//   maker.AddVertex({ 0, 0, 0 }, { 1, 1, 1, });
//   maker.AddVertex({ 0, 1, 0 }, { 1, 1, 1, });
//   maker.AddVertex({ 1, 1, 0 }, { 1, 1, 1, });
//   RenderObject *pModel = maker.AssembleRenderObject();
// 
// 
//   Plant tree;
// 
//   while (Controls::Update())
//   {
// 
//     window.Clear();
// 
//     Camera::Update();
//     mat4 viewMat = Camera::Matrix();
//     MVP = projectionMat * viewMat * modelMat;
//     MVP.Transpose();
//     pModel->Render(MVP);
// 
//     window.Swap();
// 
//   }
// 
// }
