#include "Controls.h"
#include "Window.h"
#include "FrameRate.h"
#include "CustomRenderObjects.h"
#include "Assets.h"
#include "Camera.h"
#include "PolyModel.h"
#include <string>
#include <vector>
#include "File.h"
#include <time.h>

vec3 UnProject(vec2 pos /*-1 to 1*/, const mat4 &VP)
{
  mat4 iVP = VP;
  iVP.inverse();
  vec4 screenPos = vec4(pos.x, pos.y, 1, 1);
  vec4 wp = screenPos * iVP;
  wp = wp.Normalized();
  vec3 ret(wp.x, wp.y, wp.z);
  return ret;
}

extern int lastClock;
const float tileWidth = 102.4;

struct TileType
{
  std::string name;
  PolyModel model;
};

class TileTypes
{
public:
  TileTypes()
  {
    AddTile("empty", "");
    AddTile("grass", "grass.obj");
    AddTile("path", "path.obj");
    AddTile("OutOfPlay", "OutOfPlay.obj");
    AddTile("Gateway", "Gateway.obj");
  }

  void AddTile(const std::string & name, const std::string & modelFile)
  {
    TileType tile;
    tile.name = name;
    std::string path = ((ASSETDIR "/mansion/tiles/") + modelFile).c_str();
    if(File::FileExists(path.c_str()))
      tile.model.LoadModel(path.c_str());
    tileTypes.push_back(tile);
  }

  uint8_t IDFromName(const std::string &name)
  {
    for (size_t i = 0; i < TileTypes::tileTypes.size(); i++)
      if (TileTypes::tileTypes[i].name == name)
        return i;
    return -1;
  }

  std::vector<TileType> tileTypes;
};

TileTypes Tiles;

class Mansion
{
public:
  int width = 16;
  int height = 16;
  std::vector<char> tiles;

  void Generate()
  {
    tiles.resize(width * height);

    // Grass
    for (int y = 0; y < height; y++)
      for (int x = 0; x < width; x++)
        tiles[x + y * width] = Tiles.IDFromName("grass");

    // out of play Area
    for (int x = 0; x < width; x++)
    {
      tiles[x + 0 * width] = Tiles.IDFromName("OutOfPlay");
      tiles[x + (height - 1) * width] = Tiles.IDFromName("OutOfPlay");
    }
    for (int y = 0; y < height; y++)
    {
      tiles[0 + y * width] = Tiles.IDFromName("OutOfPlay");
      tiles[(width-1) + y * width] = Tiles.IDFromName("OutOfPlay");
    }

    // GateWay
    tiles[6] = Tiles.IDFromName("Gateway");
    tiles[7] = Tiles.IDFromName("empty");
    tiles[8] = Tiles.IDFromName("empty");





  }

  void Render(const mat4 &VP)
  {
    for (int y = 0; y < height; y++)
      for (int x = 0; x < width; x++)
      {
        auto tile = tiles[x + y * width];
        mat4 modelMat;
        modelMat.Translate(x * tileWidth, 0, y * tileWidth);
        mat4 MVP = VP * modelMat;
        MVP.Transpose();
        Tiles.tileTypes[tile].model.Render(MVP);
      }
  }
};


void DrawSelection(vec2i pos, const mat4 &MVP)
{
  static bool started = false;
  static RenderObject * pMesh;
  float height = 0;
  if (!started)
  {
    started = true;
    ColouredRenderObjectMaker maker;

    maker.AddVertex(vec3(pos.x, height, pos.y), vec3(0, 1, 0));
    maker.AddVertex(vec3(pos.x + tileWidth, height, pos.y), vec3(0, 1, 0));
    maker.AddVertex(vec3(pos.x + tileWidth, height, pos.y + tileWidth), vec3(0, 1, 0));

    maker.AddVertex(vec3(pos.x + tileWidth, height, pos.y + tileWidth), vec3(0, 1, 0));
    maker.AddVertex(vec3(pos.x, height, pos.y + tileWidth), vec3(0, 1, 0));
    maker.AddVertex(vec3(pos.x, height, pos.y), vec3(0, 1, 0));
    pMesh = maker.AssembleRenderObject();
  }
  pMesh->Render(MVP);
}

void Sims()
{
  // Setup Window
  Window window("Sims", true, 1920, 1080, true);

  mat4 projectionMat;
  projectionMat.Perspective(45.0f * (float)DegsToRads, (float)window.width / window.height, 20, 200000);

  Mansion mansion;
  mansion.Generate();

  PolyModel town;
  town.LoadModel(ASSETDIR "/mansion/models/town1.obj");

  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Main Game Loop
  while (Controls::Update())
  {
    window.Clear(16,64,255);
    if (Controls::GetLeftClick())
    {
      Controls::SetMouseLock(true);
      Camera::Update(5000);
    }
    else
    {
      Controls::SetMouseLock(false);
      lastClock = clock();
      //Camera::Update(5000);
    }

    mat4 viewMat = Camera::Matrix();
    mat4 townMat;
    mat4 VP = projectionMat * viewMat;
    mat4 modelMat;
    mat4 MVP = projectionMat * viewMat * modelMat;
    MVP.Transpose();

    // Draw Selections
    if(!Controls::GetLeftClick())
    {
      // project mouse onto mansion ground plane
      vec2i mp = Controls::GetMouse();
      vec2 mf(mp.x / (float)window.width, mp.y / (float)window.height);
      vec3 pixelDir = UnProject(mf, VP);
    }
    DrawSelection(vec2i(0, 0), MVP);

    // Draw Town
    townMat.Translate(950, -10, 550);
    mat4 townMVP = projectionMat * viewMat * townMat;
    townMVP.Transpose();
    town.Render(townMVP);

    // Draw Mansion
    mansion.Render(VP);

    RenderAlphaPolygons();
    FrameRate::AddDraw(0);
    FrameRate::Update();
    window.Swap();
  }
}

