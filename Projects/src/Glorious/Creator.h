#ifndef Creator_h__
#define Creator_h__
#include "CustomRenderObjects.h"
#include "File.h"
#include "FilePath.h"
#include "ImageFile.h"
#include "LeastRects.h"

struct Creator
{

  static RenderObject *CreateRender(const char *gridFile, uint32_t gridSize)
  {
    // Open Grid
    uint32_t *grid = (uint32_t *)File::ReadFile(gridFile);

    // Create
    ColouredRenderObjectMaker maker;
    for (int z = 0; z < gridSize; z++)
      for (int y = 0; y < gridSize; y++)
        for (int x = 0; x < gridSize; x++)
        {
          uint32_t color = grid[x + y * gridSize + z * gridSize * gridSize];
          if (color)
          {
            vec3 col((color >> 0) & 255, (color >> 8) & 255, (color >> 16) & 255);
            col = col / 255.0;

            // bot
            vec3 bcol = col;// * 0.4f;
            maker.AddVertex(vec3(x, y, z), bcol);
            maker.AddVertex(vec3(x + 1, y, z), bcol);
            maker.AddVertex(vec3(x, y, z + 1), bcol);
            maker.AddVertex(vec3(x + 1, y, z), bcol);
            maker.AddVertex(vec3(x + 1, y, z + 1), bcol);
            maker.AddVertex(vec3(x, y, z + 1), bcol);

            // top
            maker.AddVertex(vec3(x, y + 1, z), col);
            maker.AddVertex(vec3(x + 1, y + 1, z), col);
            maker.AddVertex(vec3(x, y + 1, z + 1), col);
            maker.AddVertex(vec3(x + 1, y + 1, z), col);
            maker.AddVertex(vec3(x + 1, y + 1, z + 1), col);
            maker.AddVertex(vec3(x, y + 1, z + 1), col);

            // front
            vec3 zcol = col;// *0.8f;
            maker.AddVertex(vec3(x, y, z), zcol);
            maker.AddVertex(vec3(x + 1, y, z), zcol);
            maker.AddVertex(vec3(x, y + 1, z), zcol);
            maker.AddVertex(vec3(x + 1, y, z), zcol);
            maker.AddVertex(vec3(x + 1, y + 1, z), zcol);
            maker.AddVertex(vec3(x, y + 1, z), zcol);

            // back
            maker.AddVertex(vec3(x, y, z+1), zcol);
            maker.AddVertex(vec3(x + 1, y, z+1), zcol);
            maker.AddVertex(vec3(x, y + 1, z+1), zcol);
            maker.AddVertex(vec3(x + 1, y, z+1), zcol);
            maker.AddVertex(vec3(x + 1, y + 1, z+1), zcol);
            maker.AddVertex(vec3(x, y + 1, z+1), zcol);

            // left
            vec3 xcol = col;// *0.6f;
            maker.AddVertex(vec3(x, y, z), xcol);
            maker.AddVertex(vec3(x, y, z + 1), xcol);
            maker.AddVertex(vec3(x, y + 1, z), xcol);
            maker.AddVertex(vec3(x, y, z + 1), xcol);
            maker.AddVertex(vec3(x, y + 1, z + 1), xcol);
            maker.AddVertex(vec3(x, y + 1, z), xcol);

            // right
            maker.AddVertex(vec3(x + 1, y, z), xcol);
            maker.AddVertex(vec3(x + 1, y, z + 1), xcol);
            maker.AddVertex(vec3(x + 1, y + 1, z), xcol);
            maker.AddVertex(vec3(x + 1, y, z + 1), xcol);
            maker.AddVertex(vec3(x + 1, y + 1, z + 1), xcol);
            maker.AddVertex(vec3(x + 1, y + 1, z), xcol);

          }
        }
    return maker.AssembleRenderObject();
  }

  static RenderObject *SliceGrid(const char *gridFile, uint32_t gridSize, vec3i position = vec3i())
  {
    bool used = false;
    vec3 fPosition = vec3(position.x, position.y, position.z);
    AlphaTexturedRenderObjectMaker sliceMesh;
    uint32_t *grid = (uint32_t *)File::ReadFile(gridFile);
    uint32_t *img = new uint32_t[gridSize * 16 * gridSize * 16];
    bool *binImg = new bool[gridSize * gridSize];
    for (int y = 0; y < gridSize; y++)
    {
      int bx = y % 16;
      int by = y / 16;
      bool drew = false;
      for (int z = 0; z < gridSize; z++)
        for (int x = 0; x < gridSize; x++)
        {
          uint32_t color = grid[x + y * gridSize + z * gridSize * gridSize];
          binImg[x + z * gridSize] = color > 0;
          if (color)
          {
            drew = true;
            color |= 255 << 24;
          }
          img[(x + bx * 256) + (z + by * 256) * gridSize * 16] = color;
        }
      if (drew)
      {
        used = true;
        int rectCount;
        Rect * rects = LeastRects::FindRectsInImage(binImg, gridSize, gridSize, &rectCount);
        for (int i = 0; i < 1; i++)
        {
          int xx = (i % 10) * 256;
          int yy = (i / 10) * 256;

          //Rects
          for (int rectId = 0; rectId < rectCount; rectId++)
          {
            Rect &r = rects[rectId];
            float leftU = (bx + r.xpos / (float)gridSize) / 16.f;
            float rightU = (bx + (r.xpos + r.width) / (float)gridSize) / 16.f;
            float topV = (by + r.ypos / (float)gridSize) / 16.f;;
            float bottomV = (by + (r.ypos + r.height) / (float)gridSize) / 16.f;;
            sliceMesh.AddVertex(vec3(xx + r.xpos, y, yy + r.ypos) + fPosition, vec2(leftU, topV));
            sliceMesh.AddVertex(vec3(xx + r.xpos + r.width, y, yy + r.ypos) + fPosition, vec2(rightU, topV));
            sliceMesh.AddVertex(vec3(xx + r.xpos, y, yy + r.ypos + r.height) + fPosition, vec2(leftU, bottomV));

            sliceMesh.AddVertex(vec3(xx + r.xpos + r.width, y, yy + r.ypos) + fPosition, vec2(rightU, topV));
            sliceMesh.AddVertex(vec3(xx + r.xpos + r.width, y, yy + r.ypos + r.height) + fPosition, vec2(rightU, bottomV));
            sliceMesh.AddVertex(vec3(xx + r.xpos, y, yy + r.ypos + r.height) + fPosition, vec2(leftU, bottomV));
          }
        }
      }
    }
    FilePath imgName = gridFile;
    std::string fullImgName = imgName.GetFolder() + imgName.GetNameNoExt() + ".png";
    ImageFile::WriteImagePNG(fullImgName.c_str(), img, gridSize * 16, gridSize * 16);

    sliceMesh.SetTexture(fullImgName.c_str());

    delete[] img;
    delete[] grid;
    delete[] binImg;
    if (!used)
      return nullptr;
    return sliceMesh.AssembleRenderObject();
  }

};

#endif // Creator_h__
