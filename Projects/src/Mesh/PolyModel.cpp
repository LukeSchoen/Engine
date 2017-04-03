#include "PolyModel.h"
#include "File.h"
#include "Assimp.h"
#include "CustomRenderObjects.h"
#include <shobjidl.h>
#include "StreamFile.h"
#include "Decimator.h"
#include "ImageFile.h"

const int meshModelFileFormatVersion = 1;
const char *meshModelFileIdentifier = "MeshModel";

PolyModel::PolyModel()
{

}


vec3 TriangleNormal(vec3 v1, vec3 v2, vec3 v3)
{
  vec3 ab = v1 - v2;
  vec3 ac = v1 - v3;
  ab = ab.Normalized();
  ac = ac.Normalized();
  return ab.CrossProduct(ac).Normalized();
}


bool PolyModel::LoadModel(const char *fileName, bool forceReload)
{
  // Load in binary form for speed
  int inLen = strlen(fileName);
  char *binForm = new char[inLen + 2];
  memcpy(binForm, fileName, inLen);
  binForm[inLen] = 'x';
  binForm[inLen + 1] = NULL;
  if (!forceReload)
  {
    if (File::FileExists(binForm))
      if (Load(binForm))
        return true;
  }

  AssimpModel model = AssimpLoadModel(fileName);

  if (model.faceCount == 0)
    return false;

  meshCount = model.materialCount;
  pMeshes = new RenderObject[meshCount];

  TexturedRenderObjectMaker *pTexMeshMakers = new TexturedRenderObjectMaker[meshCount];
  ColouredRenderObjectMaker *pColMeshMakers = new ColouredRenderObjectMaker[meshCount];

  // Create render objects from faces
  for (int faceID = 0; faceID < model.faceCount; faceID++)
  {
    int material = model.faces[faceID].material;
    if (model.materials[material].path)
    { // Textured Models
      vec3 nor1 = model.faces[faceID].v1.nor;
      vec3 nor2 = model.faces[faceID].v2.nor;
      vec3 nor3 = model.faces[faceID].v3.nor;
      pTexMeshMakers[material].AddVertex(model.faces[faceID].v1.pos, model.faces[faceID].v1.uvs);
      pTexMeshMakers[material].AddVertex(model.faces[faceID].v2.pos, model.faces[faceID].v2.uvs);
      pTexMeshMakers[material].AddVertex(model.faces[faceID].v3.pos, model.faces[faceID].v3.uvs);
    }
    else
    { // Colored Models
      vec3 nor1 = model.faces[faceID].v1.nor;
      vec3 nor2 = model.faces[faceID].v2.nor;
      vec3 nor3 = model.faces[faceID].v3.nor;
      pColMeshMakers[material].AddVertex(model.faces[faceID].v1.pos, model.materials[material].color);
      pColMeshMakers[material].AddVertex(model.faces[faceID].v2.pos, model.materials[material].color);
      pColMeshMakers[material].AddVertex(model.faces[faceID].v3.pos, model.materials[material].color);
    }
  }

  // Assemble render objects
  for (int meshID = 0; meshID < meshCount; meshID++)
  {
    if (model.materials[meshID].path)
    {  // Textured Models
      pTexMeshMakers[meshID].SetTexture(model.materials[meshID].path);
      pMeshes[meshID] = *pTexMeshMakers[meshID].AssembleRenderObject();
    }
    else
    {  // Coloured Models
      pMeshes[meshID] = *pColMeshMakers[meshID].AssembleRenderObject();
    }
  }

  // Save in binary form to make loading faster next time
  Save(binForm);

  return true;
}

void PolyModel::GetModelExtents(vec3 *pMinimum, vec3 *pMaximum)
{
  *pMinimum = minExtents;
  *pMaximum = maxExtents;
}

void PolyModel::GetMeshData(int64_t *pMeshCount, RenderObject **ppMeshes)
{
  *pMeshCount = meshCount;
  *ppMeshes = pMeshes;
}

bool PolyModel::IntersectRay(const vec3 &rayPos, const vec3 &rayDir, float *pDistance, const mat4 &modelMatrix, const vec3 &modelPivot)
{
  for (int meshID = 0; meshID < meshCount; meshID++)
  {
    int64_t vertCount;
    vec3 *pPositions;
    pMeshes[meshID].AccessAttribute("position0", &vertCount, nullptr, nullptr, (const void**)&pPositions);
    for (int tri = 0; tri < vertCount; tri += 3)
    {
      vec3 ModelPivot = modelPivot;
      ModelPivot.y = -ModelPivot.y; // Why do we need to do this!

      vec4 v1 = (pPositions[tri + 0] + ModelPivot, 1);
      vec4 v2 = (pPositions[tri + 1] + ModelPivot, 1);
      vec4 v3 = (pPositions[tri + 2] + ModelPivot, 1);

      v1 = modelMatrix * v1;
      v2 = v2 * modelMatrix;
      v3 = v3 * modelMatrix;

      if (RayWillHitTriangle(vec3(v1), vec3(v2), vec3(v3), rayPos, rayDir, pDistance))
        return true;
    }
  }
  return false;
}

void PolyModel::SetBrightness(float brightness)
{
  for (int meshItr = 0; meshItr < meshCount; meshItr++)
    pMeshes[meshItr].AssignUniform("brightness", UT_1f, &brightness);
}

void PolyModel::Save(char *filePath)
{
  // Open file
  FILE *pFile;
  pFile = fopen(filePath, "wb");

  // Write File Identifier
  fwrite(meshModelFileIdentifier, strlen(meshModelFileIdentifier) + 1, 1, pFile);

  // Write File Version
  fwrite(&meshModelFileFormatVersion, sizeof(meshModelFileFormatVersion), 1, pFile);

  // Write Model Extents
  fwrite(&minExtents, sizeof(minExtents), 1, pFile);
  fwrite(&maxExtents, sizeof(maxExtents), 1, pFile);

  // Writ Mesh Count
  fwrite(&meshCount, sizeof(meshCount), 1, pFile);

  // Write Mesh Data
  char *meshData;
  int meshDataLen;
  for (int meshItr = 0; meshItr < meshCount; meshItr++)
  {
    pMeshes[meshItr].Serialize(&meshData, &meshDataLen);
    fwrite(meshData, meshDataLen, 1, pFile);
    delete[] meshData;
  }

  // Close File
  fclose(pFile);
}

bool PolyModel::Load(char *filePath)
{
  char *fileData;
  int64_t fileLen;
  int64_t fPtr = 0;

  // Open File
  fileData = (char*)File::ReadFile(filePath, &fileLen);

  // Read file Identifier
  int IdentifierLen = strlen(fileData + fPtr) + 1;
  if (strcmp(fileData + fPtr, meshModelFileIdentifier) != 0)
    return false;

  fPtr += IdentifierLen;

  // Read File Version
  int loadedFileVersion;
  memcpy(&loadedFileVersion, fileData + fPtr, sizeof(loadedFileVersion));
  fPtr += sizeof(loadedFileVersion);
  // Verify file version
  if (loadedFileVersion != meshModelFileFormatVersion)
    return false; // wrong file version!

  // Read Model Extents
  memcpy(&minExtents, fileData + fPtr, sizeof(minExtents));
  fPtr += sizeof(minExtents);
  memcpy(&maxExtents, fileData + fPtr, sizeof(minExtents));
  fPtr += sizeof(maxExtents);

  // Read Mesh Count
  memcpy(&meshCount, fileData + fPtr, sizeof(meshCount));
  fPtr += sizeof(meshCount);

  // Read Mesh Data
  pMeshes = new RenderObject[meshCount];

  for (int meshItr = 0; meshItr < meshCount; meshItr++)
  {
    int meshSize;
    pMeshes[meshItr].DeSerialize(fileData + fPtr, &meshSize);
    fPtr += meshSize;
  }

  return true;
}

void PolyModel::DownSample(int64_t maxPolys /*= 1000*/)
{
  MeshDecimator preview;
  SpecularColouredRenderObjectMaker pColMeshMakers;

  for (int64_t meshItr = 0; meshItr < meshCount; meshItr++)
  {
    int64_t vertCount;
    vec3 *verts;
    vec3 *cols;
    pMeshes[meshItr].AccessAttribute("position0", &vertCount, nullptr, nullptr, (const void**)&verts);
    pMeshes[meshItr].AccessAttribute("colour0", nullptr, nullptr, nullptr, (const void**)&cols);
    for (int64_t v = 0; v < vertCount; v+=3)
      preview.AddTriangle(verts[v + 0], verts[v + 1], verts[v + 2], cols[v + 0], cols[v + 1], cols[v + 2]);
  }

  preview.simplify_mesh(maxPolys);

  // Create render objects from faces
  for (size_t triItr = 0; triItr < preview.triangles.size(); triItr++)
  {
    //if (!preview.triangles[triItr].deleted)
    {
      // Colored Models
      vec3 v1 = preview.vertices[preview.triangles[triItr].v[0]].p;
      vec3 v2 = preview.vertices[preview.triangles[triItr].v[1]].p;
      vec3 v3 = preview.vertices[preview.triangles[triItr].v[2]].p;

      vec3 c1 = preview.vertices[preview.triangles[triItr].v[0]].c;
      vec3 c2 = preview.vertices[preview.triangles[triItr].v[1]].c;
      vec3 c3 = preview.vertices[preview.triangles[triItr].v[2]].c;

      vec3 n = ((v2 - v1).CrossProduct(v3 - v1)).Normalized();

      pColMeshMakers.AddVertex(v1, c1, n);
      pColMeshMakers.AddVertex(v2, c2, n);
      pColMeshMakers.AddVertex(v3, c3, n);
    }
  }
  // Assign to me
  meshCount = 1;
  pMeshes = pColMeshMakers.AssembleRenderObject();
}

vec3 SampleTexture(uint32_t *image, uint32_t width, uint32_t height, vec2 uv)
{
  int xpos = FractionalPart(uv.x) * width;
  int ypos = FractionalPart(uv.y) * height;
  uint32_t c = image[xpos + ypos * width];
  uint8_t r, g, b;
  r = c & 255;
  g = (c >> 8) & 255;
  b = (c >> 16) & 255;
  return vec3(r / 255.0, g / 255.0, b / 255.0);
}

void PolyModel::BakeColors(bool accurate /*= false*/)
{
  ColouredRenderObjectMaker pColMeshMakers;

  // Find Textured Meshes
  for (int64_t meshItr = 0; meshItr < meshCount; meshItr++)
  {
    const char *path = nullptr;
    pMeshes[meshItr].AccessTexture("texture0", &path);
    if (File::FileExists(path))
    { // texture
      uint32_t width, height;
      uint32_t *image = ImageFile::ReadImage(path, &width, &height);
      int64_t vertCount;
      vec3 *verts;
      vec2 *uvs;
      pMeshes[meshItr].AccessAttribute("position0", &vertCount, nullptr, nullptr, (const void**)&verts);
      pMeshes[meshItr].AccessAttribute("texcoord0", nullptr, nullptr, nullptr, (const void**)&uvs);
      for (int64_t v = 0; v < vertCount; v += 3)
      {
        vec3 col = SampleTexture(image, width, height, uvs[v + 0]);
        pColMeshMakers.AddVertex(verts[v + 0], col);

        col = SampleTexture(image, width, height, uvs[v + 1]);
        pColMeshMakers.AddVertex(verts[v + 1], col);

        col = SampleTexture(image, width, height, uvs[v + 2]);
        pColMeshMakers.AddVertex(verts[v + 2], col);
      }
      delete[] image;
    }
    else
    { // color
      int64_t vertCount;
      vec3 *verts;
      vec3 *color;
      pMeshes[meshItr].AccessAttribute("position0", &vertCount, nullptr, nullptr, (const void**)&verts);
      pMeshes[meshItr].AccessAttribute("colour0", nullptr, nullptr, nullptr, (const void**)&color);
      for (int64_t v = 0; v < vertCount; v += 3)
      {
       
        pColMeshMakers.AddVertex(verts[v + 0], color[v + 0]);
        pColMeshMakers.AddVertex(verts[v + 1], color[v + 1]);
        pColMeshMakers.AddVertex(verts[v + 2], color[v + 2]);
      }
    }

  }

  meshCount = 1;
  pMeshes = pColMeshMakers.AssembleRenderObject();
}

void PolyModel::Render(const mat4 &MVP)
{
  for (int meshItr = 0; meshItr < meshCount; meshItr++)
    pMeshes[meshItr].Render(MVP);
}

void PolyModel::Destroy()
{
  for (int meshItr = 0; meshItr < meshCount; meshItr++)
    pMeshes[meshItr].~RenderObject();

  meshCount = 0;
}
