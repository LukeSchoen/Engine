#include "Assimp.h"
#include "Math.h"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postProcess.h"
#include <vector>

AssimpModel model;
std::vector<face> modelData;

void SceneAddMesh(aiMesh *mesh, aiMatrix4x4 transformationMatrix)
{
  int faces = mesh->mNumFaces;
  for (int faceItr = 0; faceItr < faces; faceItr++)
  {
    if (mesh->HasPositions())
    {
      unsigned int index = mesh->mFaces[faceItr].mIndices;
      unsigned int material = mesh->mMaterialIndex;
      unsigned int *indicies = mesh->mIndices + index;
      vertex verticies[3];
      if (mesh->mFaces[faceItr].mNumIndices == 3)
      {
        for (int vertItr = 0; vertItr < 3; vertItr++)
        {
          aiVector3D vertPos = transformationMatrix * mesh->mVertices[indicies[vertItr]];
          aiVector3D vertNorm = mesh->mNormals[indicies[vertItr]];
          if (mesh->HasTextureCoords(0))
          {
            aiVector3D* pTexCoords = &(mesh->mTextureCoords[0][indicies[vertItr]]);
            verticies[vertItr] = vertex(vec3(vertPos.x, vertPos.y, vertPos.z), vec3(1, 1, 1), vec3(vertNorm.x, vertNorm.y, vertNorm.z), vec2(pTexCoords->x, 1.0 - pTexCoords->y));
          }
          else
          {
            verticies[vertItr] = vertex(vec3(vertPos.x, vertPos.y, vertPos.z), vec3(1, 1, 1), vec3(vertNorm.x, vertNorm.y, vertNorm.z), vec2(0, 0));
          }
        }
        modelData.push_back(face(verticies[0], verticies[1], verticies[2], material));
      }
    }
  }
}

void RecursiveSceneDescent(const aiScene *scene, const aiNode *node, aiMatrix4x4 transformationMatrix)
{
  transformationMatrix *= node->mTransformation;
  for (int subMeshItr = 0; subMeshItr < node->mNumMeshes; subMeshItr++)
    SceneAddMesh(scene->mMeshes[node->mMeshes[subMeshItr]], node->mTransformation);
  for (int subNodeItr = 0; subNodeItr < node->mNumChildren; subNodeItr++)
    RecursiveSceneDescent(scene, node->mChildren[subNodeItr], transformationMatrix);
}

AssimpModel AssimpLoadModel(const char *filePath)
{
  // Cleanup Any Previous Conversion Information
  modelData.clear();
  delete[] model.faces;
  delete[] model.materials;
  model.faces = nullptr;
  model.materials = nullptr;

  // Open Model
  Assimp::Importer importer;
  const aiScene *pScene;
  aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
  pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_GenNormals | aiProcess_GenSmoothNormals | aiProcess_PreTransformVertices | aiProcess_TransformUVCoords);
  
  // Parse Materials & textures
  int texCount = pScene->mNumMaterials;
  model.materials = new Material[texCount];
  model.materialCount = texCount;
  for (int texItr = 0; texItr < texCount; texItr++)
  {
    aiMaterial *pMaterial = pScene->mMaterials[texItr];
    if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
      aiString Path;
      pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL);
      model.materials[texItr] = Material(Path.C_Str(), filePath);
    }
    else
    {
      aiColor4D diffuseColor;
      if (aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor) == aiReturn_SUCCESS)
        model.materials[texItr] = Material(nullptr, nullptr, vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a));
      else
        model.materials[texItr] = Material();
    }
  }


  // Iterate through Scene Graph
  aiNode *rootNode = pScene->mRootNode;
  RecursiveSceneDescent(pScene, rootNode, aiMatrix4x4(/*identity*/));

  // Compile Final Model
  model.faceCount = modelData.size();
  model.faces = new face[model.faceCount];
  for (int faceItr = 0; faceItr < model.faceCount; faceItr++)
    model.faces[faceItr] = modelData[faceItr];
  return model;
}


