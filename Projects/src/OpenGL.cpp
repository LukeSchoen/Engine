#include "Window.h"
#include "Threads.h"
#include "FrameRate.h"
#include "Controls.h"
#include "Camera.h"
#include <stdlib.h>
#include "RenderObject.h"
#include "Polymodel.h"
#include "Maths.h"
#include "Shaders.h"
#include "Textures.h"
#include "Framebuffer.h"
#include "Effect Object.h"
#include "Assets.h"
#include "CustomRenderObjects.h"
#include "Octree.h"
#include "Convertor.h"
#include "Text.h"
#include <mutex>

vec3 ColorFromWord(uint32_t word)
{
  uint8_t b = word & 0xFF;
  uint8_t g = (word >> 8) & 0xFF;
  uint8_t r = (word >> 16) & 0xFF;
  return vec3(r / 255.f, g / 255.f, b / 255.f);
}

void GenerateDynamicTreeModel(vec3 campos, float camRes, Octree *tree, Node node, vec3 position, int maximumDepth, ColouredRenderObjectMaker *maker, int currentLayer = 0)
{

  float layerSize = 1.0f / (1LL << currentLayer);
  if (layerSize / (position - campos).Length() < 1.0 / camRes)
  {
    if (tree->GetColor(node))
      maker->AddVertex(position, ColorFromWord(tree->GetColor(node)));
    return;
  }

  if (maximumDepth == currentLayer)
    if (tree->GetColor(node))
      return maker->AddVertex(position, ColorFromWord(tree->GetColor(node)));

  for (int c = 0; c < 8; c++)
  {
    vec3 childPos = position;
    Node child = tree->GetChild(node, c, &childPos, currentLayer);
    if (child != node)
      GenerateDynamicTreeModel(campos, camRes, tree, child, childPos, maximumDepth, maker, currentLayer + 1);
  }
}


std::vector<RenderObject*> modelList;
std::mutex modelListLock;

void CloudStreamThread(Octree *tree, float camRes, int maximumDepth)
{
  while (true)
  {
    ColouredRenderObjectMaker maker;
    GenerateDynamicTreeModel(Camera::Position(), camRes, tree, tree->RootNode(), vec3(0, 0, 0), maximumDepth, &maker);
    modelListLock.lock();
    while (modelList.size() > 3) // sleep if main thread seems dead
    {
      modelListLock.unlock();
      Sleep(1);
      modelListLock.lock();
    }
    RenderObject *ro = maker.AssembleRenderObject();
    modelList.push_back(ro);
    modelListLock.unlock();
  }
}

void GenerateTreeModel(Octree *tree, Node node, vec3 position, int targetDepth, ColouredRenderObjectMaker *maker, int currentLayer = 0)
{
  if (targetDepth == currentLayer)
    if(tree->GetColor(node))
      return maker->AddVertex(position, ColorFromWord(tree->GetColor(node)));

  for (int c = 0; c < 8; c++)
  {
    vec3 childPos = position;
    Node child = tree->GetChild(node, c, &childPos, currentLayer);
    if (child != node)
      GenerateTreeModel(tree, child, childPos, targetDepth, maker, currentLayer + 1);
  }
}

void OpenGL()
{
  //Threads::SetFastMode();  // Work in real time mode while loading

#ifdef _DEBUG
  Window window("Game", true, 800, 600, false); // Create Debug Game Window
#else
  Window window("Game", true, 1920, 1080, true); // Create Game Window
#endif

  //Convertor::ResamplePCF("d:/temp/carrickhill.pcf", "d:/temp/2048carrickhill.pcf", 2048);
  Octree model(PointCloud("d:/temp/2048carrickhill.pcf"));
  model.SaveFile("d:/temp/carrickhill.oct");
  //exit(0);
  Octree tree("d:/temp/carrickhill.oct");

  Controls::SetMouseLock(true);

  mat4 projectionMat;
  projectionMat.Perspective(60.0f * (float)DegsToRads, (float)window.width / window.height, 0.01f, 100.0f);

  int PointSize = 5;
  glPointSize(PointSize);

  //std::thread(&CloudStreamThread, &tree, window.width / PointSize, (int)tree.Depth());



  RenderObject *currentModel = nullptr;

  ColouredRenderObjectMaker maker;
  GenerateTreeModel(&tree, tree.RootNode(), vec3(0,0,0), 11, &maker);
  currentModel = maker.AssembleRenderObject();


  while (Controls::Update()) // Main Game Loop
  {
    // Get latest model, delete old models
    //modelListLock.lock();
    //if (modelList.size())
    //{
    //  // Delete old active list
    //  //currentModel->Clear();
    //  delete currentModel;
    //
    //  // Take new active list
    //  currentModel = modelList[modelList.size() - 1];
    //  modelList.erase(modelList.begin() + modelList.size() - 1);
    //
    //  // Delete any extra lists
    //  for(auto &thing : modelList)
    //    delete &thing;
    //  modelList.clear();
    //}
    //modelListLock.unlock();


    window.Clear(0, 190, 255);
    Camera::Update(1);

    mat4 viewMat = Camera::Matrix();
    mat4 MVP;
    mat4 modelMat;
    MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();

    if(currentModel)
      currentModel->RenderPoints(MVP);

    window.Swap(); // Swap Window
    FrameRate::Update();
  }
}
