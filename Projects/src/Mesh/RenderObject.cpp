#include "RenderObject.h"
#include "Shaders.h"
#include "Textures.h"
#include "FrameRate.h"

void RenderObject::Clear()
{
  for (int aID = 0; aID < attributeCount; aID++)
  {
    glDeleteBuffers(1, &pAttributeData[aID].glPtr);
    delete[] pAttributeData[aID].pData;
    delete[] pAttributeData[aID].name;
  }
  for (int uID = 0; uID < uniformCount; uID++)
  {
    delete[] pUniformData[uID].name;
    delete[] pUniformData[uID].pData;
  }
  for (int tID = 0; tID < textureCount; tID++)
  {
    if(*((char*)(pTextureData[tID].pData)) != '$')
      glDeleteBuffers(1, &pTextureData[tID].glPtr);
    delete[] pTextureData[tID].name;
    delete[] pTextureData[tID].pData;
  }
  delete[] pAttributeData;
  delete[] pUniformData;
  delete[] pTextureData;
  delete[] vertShaderPath;
  delete[] fragShaderPath;
  vertShaderPath = nullptr;
  fragShaderPath = nullptr;
  pAttributeData = nullptr;
  pUniformData = nullptr;
  pTextureData = nullptr;
  program = glUndefined;
  attributeCount = 0;
  uniformCount = 0;
  textureCount = 0;
  shadersUploaded = false;
  texturesUploaded = false;
  uniformsUploaded = false;
  attributesUploaded = false;
}

void RenderObject::AssignShader(const char *vertFile, const char *fragFile, const char *geomFile)
{
  if (program != glUndefined)
    glDeleteProgram(program);
  // Vert shader
  delete[] vertShaderPath;
  int vertLen = strlen(vertFile) + 1;
  vertShaderPath = new char[vertLen];
  memcpy(vertShaderPath, vertFile, vertLen);

  // Frag shader
  delete[] fragShaderPath;
  int fragLen = strlen(fragFile) + 1;
  fragShaderPath = new char[fragLen];
  memcpy(fragShaderPath, fragFile, fragLen);

  // Geom shader
  if (geomFile)
  {
    delete[] geomShaderPath;
    int geomLen = strlen(geomFile) + 1;
    geomShaderPath = new char[fragLen];
    memcpy(geomShaderPath, geomFile, geomLen);
  }
  shadersUploaded = false;
}

void RenderObject::AssignShader(unsigned int shaderID)
{
  shadersUploaded = true;
  attributesUploaded = false;
  uniformsUploaded = false;
  texturesUploaded = false;
  program = shaderID;
}

void RenderObject::AssignTexture(const char *varName, GLuint textureID, GLTextureType textureType)
{
  char textureData[5];
  textureData[0] = '$';
  *((GLuint*)(&textureData[1])) = textureID;
  AssignResource(&pTextureData, &textureCount, GLResource(varName, textureData, 1, textureType), 5);
  texturesUploaded = false;
}

void RenderObject::AssignTexture(const char *varName, const char *texturePath, GLTextureType textureType)
{
  AssignResource(&pTextureData, &textureCount, GLResource(varName, texturePath, 1, textureType), strlen(texturePath) + 1);
  texturesUploaded = false;
}

void RenderObject::AssignUniform(const char *varName, GLUniformType uType, const void *pData, int count)
{
  AssignResource(&pUniformData, &uniformCount, GLResource(varName, pData, count, GLResouceType(uType)), GetUniformSize(uType) * count);
  uniformsUploaded = false;
}

void RenderObject::AssignAttribute(const char *varName, GLAttributeType aType, const void *pData, int aCount, int vertexCount)
{
  AssignResource(&pAttributeData, &attributeCount, GLResource(varName, pData, aCount, aType, vertexCount), GetAttributeSize(aType) * aCount * vertexCount);
  attributesUploaded = false;
}

void RenderObject::AssignResource(GLResource **ppResourceBlock, unsigned int *pResourceBlockSize, GLResource &newResouce, unsigned int newResourceSize)
{
  int resourceID = -1;
  for (int id = 0; id < *pResourceBlockSize; id++) // Find Resource
  {
    if (strcmp((*ppResourceBlock)[id].name, newResouce.name) == 0)
      resourceID = id;
  }
  if (resourceID == -1) // New Resource
  {
    resourceID = (*pResourceBlockSize)++; // Expand Resource Block
    *ppResourceBlock = (GLResource*)realloc(*ppResourceBlock, sizeof(GLResource) * (*pResourceBlockSize));
    int nameLen = strlen(newResouce.name) + 1; // Store Resource Name
    (*ppResourceBlock)[resourceID].name = new char[nameLen];
    memcpy((void*)(*ppResourceBlock)[resourceID].name, newResouce.name, nameLen);
    (*ppResourceBlock)[resourceID].glPtr = glUndefined; // Default GL Values
    (*ppResourceBlock)[resourceID].glLoc = glUndefined;
  }
  else
  {
    delete[](*ppResourceBlock)[resourceID].pData; // Free Previous Resource Data
  }
  int dataSize = newResourceSize; // Store Resource Data
  (*ppResourceBlock)[resourceID].pData = new char[dataSize];
  memcpy((void*)(*ppResourceBlock)[resourceID].pData, newResouce.pData, dataSize);
  (*ppResourceBlock)[resourceID].loaded = false;
  (*ppResourceBlock)[resourceID].type = newResouce.type;
  (*ppResourceBlock)[resourceID].count = newResouce.count;
  (*ppResourceBlock)[resourceID].verts = newResouce.verts;
}

bool RenderObject::AccessResource(GLResource *pResourceBlock, unsigned int pResourceBlockSize, const char *resorceName, uint32_t *pResouceCount, int64_t *pResouceVerts, GLResouceType *pType, const void **ppResourceData)
{
  for (int id = 0; id < pResourceBlockSize; id++)
  {
    if (strcmp(pResourceBlock[id].name, resorceName) == 0)
    {
      if (pResouceCount) *pResouceCount = pResourceBlock[id].count;
      if (pResouceVerts) *pResouceVerts = pResourceBlock[id].verts;
      if (pType) *pType = pResourceBlock[id].type;
      if (ppResourceData) *ppResourceData = pResourceBlock[id].pData;
      return true;
    }
  }
  return false;
}

void RenderObject::BindShaders()
{
  if (program == -1)
    __debugbreak();

  glUseProgram(program);
}

void RenderObject::BindTextures()
{
  for (int tID = 0; tID < textureCount; tID++)
  {
    glActiveTexture(GL_TEXTURE0 + tID);
    if (pTextureData[tID].type.texture == TT_2D)
      glBindTexture(GL_TEXTURE_2D, pTextureData[tID].glPtr);
    if (pTextureData[tID].type.texture == TT_2D_ARRAY)
      glBindTexture(GL_TEXTURE_2D_ARRAY, pTextureData[tID].glPtr);
    if (pTextureData[tID].glLoc != -1)
      glUniform1i(pTextureData[tID].glLoc, tID);
  }
}

void RenderObject::BindUniforms()
{
  for (int uID = 0; uID < uniformCount; uID++) // Upload Uniform data
  {
    GLint uLoc = pUniformData[uID].glLoc;

    if (uLoc == -1)
      continue;

    const void *pData = pUniformData[uID].pData;
    int dataCount = pUniformData[uID].count;
    switch (pUniformData[uID].type.uniform)
    {
    case UT_1f:
      glUniform1f(uLoc, *((GLfloat*)pData));
      break;
    case UT_2f:
      glUniform2f(uLoc, ((GLfloat*)pData)[0], ((GLfloat*)pData)[1]);
      break;
    case UT_3f:
      glUniform3f(uLoc, ((GLfloat*)pData)[0], ((GLfloat*)pData)[1], ((GLfloat*)pData)[2]);
      break;
    case UT_4f:
      glUniform4f(uLoc, ((GLfloat*)pData)[0], ((GLfloat*)pData)[1], ((GLfloat*)pData)[2], ((GLfloat*)pData)[3]);
      break;
    case UT_1i:
      glUniform1i(uLoc, *((GLint*)pData));
      break;
    case UT_2i:
      glUniform2i(uLoc, ((GLint*)pData)[0], ((GLint*)pData)[1]);
      break;
    case UT_3i:
      glUniform3i(uLoc, ((GLint*)pData)[0], ((GLint*)pData)[1], ((GLint*)pData)[2]);
      break;
    case UT_4i:
      glUniform4i(uLoc, ((GLint*)pData)[0], ((GLint*)pData)[1], ((GLint*)pData)[2], ((GLint*)pData)[3]);
      break;
    case UT_1fv:
      glUniform1fv(uLoc, dataCount, (GLfloat*)pData);
      break;
    case UT_2fv:
      glUniform2fv(uLoc, dataCount, (GLfloat*)pData);
      break;
    case UT_3fv:
      glUniform3fv(uLoc, dataCount, (GLfloat*)pData);
      break;
    case UT_4fv:
      glUniform4fv(uLoc, dataCount, (GLfloat*)pData);
      break;
    case UT_1iv:
      glUniform1iv(uLoc, dataCount, (GLint*)pData);
      break;
    case UT_2iv:
      glUniform2iv(uLoc, dataCount, (GLint*)pData);
      break;
    case UT_3iv:
      glUniform3iv(uLoc, dataCount, (GLint*)pData);
      break;
    case UT_4iv:
      glUniform4iv(uLoc, dataCount, (GLint*)pData);
      break;
    case UT_mat2fv:
      glUniformMatrix2fv(uLoc, dataCount, false, (GLfloat*)pData);
      break;
    case UT_mat3fv:
      glUniformMatrix3fv(uLoc, dataCount, false, (GLfloat*)pData);
      break;
    case UT_mat4fv:
      glUniformMatrix4fv(uLoc, dataCount, false, (GLfloat*)pData);
      break;
    default:
      break;
    }
  }
}

void RenderObject::BindAttributes()
{
  for (int aID = 0; aID < attributeCount; aID++) // Upload Uniform data
  {
    if (pAttributeData[aID].glLoc == -1)
      continue;

    glEnableVertexAttribArray(aID); // Enable Vertex Attribute Channel
    glBindBuffer(GL_ARRAY_BUFFER, pAttributeData[aID].glPtr); // Bind Vertex Attribute Channel
    switch (pAttributeData[aID].type.attribute) // Point To Vertex Attribute Data
    {
    case AT_BYTE:
      glVertexAttribPointer(pAttributeData[aID].glLoc, pAttributeData[aID].count, GL_BYTE, GL_FALSE, 0, (void*)0);
      break;
    case AT_BYTE_NORM:
      glVertexAttribPointer(pAttributeData[aID].glLoc, pAttributeData[aID].count, GL_BYTE, GL_TRUE, 0, (void*)0);
      break;
    case AT_UNSIGNED_BYTE:
      glVertexAttribPointer(pAttributeData[aID].glLoc, pAttributeData[aID].count, GL_UNSIGNED_BYTE, GL_FALSE, 0, (void*)0);
      break;
    case AT_UNSIGNED_BYTE_NORM:
      glVertexAttribPointer(pAttributeData[aID].glLoc, pAttributeData[aID].count, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);
      break;
    case AT_SHORT:
      glVertexAttribPointer(pAttributeData[aID].glLoc, pAttributeData[aID].count, GL_SHORT, GL_FALSE, 0, (void*)0);
      break;
    case AT_UNSIGNED_SHORT:
      glVertexAttribPointer(pAttributeData[aID].glLoc, pAttributeData[aID].count, GL_UNSIGNED_SHORT, GL_FALSE, 0, (void*)0);
      break;
    case AT_INT:
      glVertexAttribPointer(pAttributeData[aID].glLoc, pAttributeData[aID].count, GL_INT, GL_FALSE, 0, (void*)0);
      break;
    case AT_UNSIGNED_INT:
      glVertexAttribPointer(pAttributeData[aID].glLoc, pAttributeData[aID].count, GL_UNSIGNED_INT, GL_FALSE, 0, (void*)0);
      break;
    case AT_HALF_FLOAT:
      glVertexAttribPointer(pAttributeData[aID].glLoc, pAttributeData[aID].count, GL_HALF_FLOAT, GL_FALSE, 0, (void*)0);
      break;
    case AT_FLOAT:
      glVertexAttribPointer(pAttributeData[aID].glLoc, pAttributeData[aID].count, GL_FLOAT, GL_FALSE, 0, (void*)0);
      break;
    case AT_DOUBLE:
      glVertexAttribPointer(pAttributeData[aID].glLoc, pAttributeData[aID].count, GL_DOUBLE, GL_FALSE, 0, (void*)0);
      break;
    }
  }
}

void RenderObject::UploadToGPU()
{
  if (!shadersUploaded)
  {
    program = Shaders::Load(vertShaderPath, fragShaderPath, geomShaderPath); // Load Shaders
    attributesUploaded = false;
    uniformsUploaded = false;
    texturesUploaded = false;
  }
  if (!attributesUploaded)
  {
    vertexCount = INT_MAX; // Calculate The Largest Complete Set Of Available Vertex Data
    for (int aItr = 0; aItr < attributeCount; aItr++) // Attributes
      if (!pAttributeData[aItr].loaded)
      {
        if (pAttributeData[aItr].glPtr == glUndefined)
          glGenBuffers(1, &pAttributeData[aItr].glPtr); // Generate Attribute Channel Buffers
        vertexCount = Min(vertexCount, pAttributeData[aItr].verts); // Work down to the Smallest Complete Set
        pAttributeData[aItr].glLoc = glGetAttribLocation(program, pAttributeData[aItr].name); // Store Attribute Shader Layout
        if (!attributesUploaded) // Upload Vertex Attribute Data
        {
          glBindBuffer(GL_ARRAY_BUFFER, pAttributeData[aItr].glPtr);
          glBufferData(GL_ARRAY_BUFFER, pAttributeData[aItr].verts * pAttributeData[aItr].count * GetAttributeSize(pAttributeData[aItr].type.attribute), pAttributeData[aItr].pData, GL_STATIC_DRAW);
        }
        pAttributeData[aItr].loaded = true;
      }
  }
  if (!uniformsUploaded)
  {
    for (int uID = 0; uID < uniformCount; uID++) // Uniforms
      if (!pUniformData[uID].loaded)
      {
        if(pUniformData[uID].glLoc  == glUndefined)
          pUniformData[uID].glLoc = glGetUniformLocation(program, pUniformData[uID].name); // Store Uniform Shader Layout
        pUniformData[uID].loaded = true;
      }
    uniformsUploaded = true;
  }
  if (!texturesUploaded)
    for (int tItr = 0; tItr < textureCount; tItr++) // Textures
      if (!pTextureData[tItr].loaded)
      {
        if (*((char*)pTextureData[tItr].pData) == '$')
          memcpy(&pTextureData[tItr].glPtr, (char*)pTextureData[tItr].pData + 1, 4); // Immediate texture
        else
          pTextureData[tItr].glPtr = Textures::LoadTexture((char*)pTextureData[tItr].pData); // Texture file path
        pTextureData[tItr].glLoc = glGetUniformLocation(program, pTextureData[tItr].name); // Store Texture Shader Layout
        pTextureData[tItr].loaded = true;
      }
  shadersUploaded = true;
  texturesUploaded = true;
  uniformsUploaded = true;
  attributesUploaded = true;
}

void RenderObject::Render(const Matrix4x4 &MVP, int firstVertex, int vertCount)
{
  AssignUniform("MVP", UT_mat4fv, &MVP); // Specify Model View Projection
  UploadToGPU(); // Upload Geometry
  BindShaders(); // Bind Shader Programs
  BindUniforms(); // Bind Shader Uniforms
  BindTextures(); // Bind Shader Textures
  BindAttributes(); // Bind Vertex Attributes
  vertCount = Min(vertCount, vertexCount - firstVertex);
  glDrawArrays(GL_TRIANGLES, firstVertex, vertCount); // Render
  FrameRate::AddDraw(vertexCount / 3); // Report Tris
}

void RenderObject::RenderLines(const Matrix4x4 &MVP)
{
  AssignUniform("MVP", UT_mat4fv, &MVP); // Specify Model View Projection
  UploadToGPU(); // Upload Geometry
  BindShaders(); // Bind Shader Programs
  BindUniforms(); // Bind Shader Uniforms
  BindTextures(); // Bind Shader Textures
  BindAttributes(); // Bind Vertex Attributes
  glDrawArrays(GL_LINES, 0, vertexCount); // Render
  FrameRate::AddDraw(vertexCount); // Report Points
}

void RenderObject::RenderPoints(const Matrix4x4 &MVP)
{
  AssignUniform("MVP", UT_mat4fv, &MVP); // Specify Model View Projection
  UploadToGPU(); // Upload Geometry
  BindShaders(); // Bind Shader Programs
  BindUniforms(); // Bind Shader Uniforms
  BindTextures(); // Bind Shader Textures
  BindAttributes(); // Bind Vertex Attributes
  glDrawArrays(GL_POINTS, 0, vertexCount); // Render
  FrameRate::AddDraw(vertexCount); // Report Points
}

void RenderObject::AccessTexture(const char *textureName, const char **ppTexturePath)
{
  AccessResource(pTextureData, textureCount, textureName, nullptr, nullptr, nullptr, (const void **)ppTexturePath);
}

void RenderObject::AccessUniform(const char *uniformName, const void **ppUniformData, GLUniformType *uType, uint32_t *uCount)
{
  GLResouceType type;
  if (AccessResource(pUniformData, uniformCount, uniformName, uCount, nullptr, &type, ppUniformData) && uType)
    *uType = type.uniform;
}

void RenderObject::AccessAttribute(const char *attributeName, int64_t *pVertCount, uint32_t *aCount, GLAttributeType *aType, const void **ppPosData)
{
  GLResouceType type;
  if(AccessResource(pAttributeData, attributeCount, attributeName, aCount, pVertCount, &type, ppPosData) && aType)
    *aType = type.attribute;
}

void RenderObject::Serialize(char **data, int *pLength)
{
  int texLen = sizeof(textureCount);
  for (int texItr = 0; texItr < textureCount; texItr++)
  {
    texLen += strlen(pTextureData[texItr].name) + 1;
    texLen += strlen((char*)pTextureData[texItr].pData) + 1;
  }
  int uniLen = sizeof(uniformCount);
  for (int uniItr = 0; uniItr < uniformCount; uniItr++)
  {
    uniLen += strlen(pUniformData[uniItr].name) + 1;
    uniLen += sizeof(pUniformData[uniItr].count);
    uniLen += sizeof(pUniformData[uniItr].type);
    uniLen += GetUniformSize(pUniformData[uniItr].type.uniform) * pUniformData[uniItr].count;
  }
  int attLen = sizeof(attributeCount);
  for (int attItr = 0; attItr < attributeCount; attItr++)
  {
    attLen += strlen(pAttributeData[attItr].name) + 1;
    attLen += sizeof(pAttributeData[attItr].verts);
    attLen += sizeof(pAttributeData[attItr].count);
    attLen += sizeof(pAttributeData[attItr].type);
    attLen += GetAttributeSize(pAttributeData[attItr].type.attribute) * pAttributeData[attItr].count * pAttributeData[attItr].verts;
  }
  int ShaderDirLen = strlen(SHADERDIR);
  int vertLen = strlen(vertShaderPath) + 1 - ShaderDirLen;
  int fragLen = strlen(fragShaderPath) + 1 - ShaderDirLen;
  int fullObjectSize = texLen + uniLen + attLen + vertLen + fragLen;
  int sPtr = 0;
  // Serialize Object
  char *sData = new char[fullObjectSize]; // Reserve space for data
                                          // Shaders
  memcpy(sData + sPtr, vertShaderPath + ShaderDirLen, vertLen); // Vert
  sPtr += vertLen;
  memcpy(sData + sPtr, fragShaderPath + ShaderDirLen, fragLen); // Frag
  sPtr += fragLen;
  // Textures
  memcpy(sData + sPtr, &textureCount, sizeof(textureCount)); // Texture Count
  sPtr += sizeof(textureCount);
  for (int texItr = 0; texItr < textureCount; texItr++) // Texture Data
  {
    memcpy(sData + sPtr, pTextureData[texItr].name, strlen(pTextureData[texItr].name) + 1);
    sPtr += strlen(pTextureData[texItr].name) + 1;
    memcpy(sData + sPtr, pTextureData[texItr].pData, strlen((char*)pTextureData[texItr].pData) + 1);
    sPtr += strlen((char*)pTextureData[texItr].pData) + 1;
  }
  // Uniforms
  memcpy(sData + sPtr, &uniformCount, sizeof(uniformCount)); // Uniform Count
  sPtr += sizeof(uniformCount);
  for (int uniItr = 0; uniItr < uniformCount; uniItr++) // Uniform Data
  {
    memcpy(sData + sPtr, pUniformData[uniItr].name, strlen(pUniformData[uniItr].name) + 1);
    sPtr += strlen(pUniformData[uniItr].name) + 1;
    memcpy(sData + sPtr, &pUniformData[uniItr].count, sizeof(pUniformData[uniItr].count));
    sPtr += sizeof(pUniformData[uniItr].count);
    memcpy(sData + sPtr, &pUniformData[uniItr].type, sizeof(pUniformData[uniItr].type));
    sPtr += sizeof(pUniformData[uniItr].type);
    uniLen = GetUniformSize(pUniformData[uniItr].type.uniform) * pUniformData[uniItr].count;
    memcpy(sData + sPtr, pUniformData[uniItr].pData, uniLen);
    sPtr += uniLen;
  }
  // Attributes
  memcpy(sData + sPtr, &attributeCount, sizeof(attributeCount)); // Attribute Count
  sPtr += sizeof(attributeCount);
  for (int attItr = 0; attItr < attributeCount; attItr++) // Attribute Data
  {
    memcpy(sData + sPtr, pAttributeData[attItr].name, strlen(pAttributeData[attItr].name) + 1);
    sPtr += strlen(pAttributeData[attItr].name) + 1;
    memcpy(sData + sPtr, &pAttributeData[attItr].verts, sizeof(pAttributeData[attItr].verts));
    sPtr += sizeof(pAttributeData[attItr].verts);
    memcpy(sData + sPtr, &pAttributeData[attItr].count, sizeof(pAttributeData[attItr].count));
    sPtr += sizeof(pAttributeData[attItr].count);
    memcpy(sData + sPtr, &pAttributeData[attItr].type, sizeof(pAttributeData[attItr].type));
    sPtr += sizeof(pAttributeData[attItr].type);
    attLen = GetAttributeSize(pAttributeData[attItr].type.attribute) * pAttributeData[attItr].count * pAttributeData[attItr].verts;
    memcpy(sData + sPtr, pAttributeData[attItr].pData, attLen);
    sPtr += attLen;
  }
  *pLength = sPtr;
  *data = sData;
}

void RenderObject::DeSerialize(char *sData, int *pLength)
{
  int sPtr = 0;
  // Shaders
  int ShaderDirLen = strlen(SHADERDIR);
  int vertLen = strlen(sData + sPtr) + 1;
  char *vertShader = new char[ShaderDirLen + vertLen];
  memcpy(vertShader, SHADERDIR, ShaderDirLen);
  memcpy(vertShader + ShaderDirLen, sData + sPtr, vertLen);
  sPtr += vertLen;
  int fragLen = strlen(sData + sPtr) + 1;
  char *fragShader = new char[ShaderDirLen + fragLen];
  memcpy(fragShader, SHADERDIR, ShaderDirLen);
  memcpy(fragShader + ShaderDirLen, sData + sPtr, fragLen);
  sPtr += fragLen;
  AssignShader(vertShader, fragShader);
  delete[] vertShader;
  delete[] fragShader;
  // Textures
  int NewTextureCount; // Texture Count
  memcpy(&NewTextureCount, sData + sPtr, sizeof(NewTextureCount));
  sPtr += sizeof(NewTextureCount);
  for (int texItr = 0; texItr < NewTextureCount; texItr++) // Texture Data
  {
    char *textureName = sData + sPtr;
    sPtr += strlen(textureName) + 1;
    char *texturePath = sData + sPtr;
    sPtr += strlen(texturePath) + 1;
    AssignTexture(textureName, texturePath);
  }
  // Uniforms
  int NewUniformCount; // Uniform Count
  memcpy(&NewUniformCount, sData + sPtr, sizeof(NewUniformCount));
  sPtr += sizeof(NewUniformCount);
  // Uniform Data
  for (int uniItr = 0; uniItr < NewUniformCount; uniItr++)
  {
    char *uName = sData + sPtr;
    sPtr += strlen(uName) + 1;
    uint32_t uCount;
    memcpy(&uCount, sData + sPtr, sizeof(uCount));
    sPtr += sizeof(uCount);
    GLResouceType uType;
    memcpy(&uType.uniform, sData + sPtr, sizeof(uType.uniform));
    sPtr += sizeof(uType.uniform);
    AssignUniform(uName, uType.uniform, sData + sPtr, uCount);
    int uDataSize = GetUniformSize(uType.uniform) * uCount;
    sPtr += uDataSize;
  }
  // Attributes
  int NewAttributeCount; // Attribute Count
  memcpy(&NewAttributeCount, sData + sPtr, sizeof(NewAttributeCount));
  sPtr += sizeof(NewAttributeCount);
  for (int attItr = 0; attItr < NewAttributeCount; attItr++) // Attribute Data
  {
    char *aName = sData + sPtr;
    sPtr += strlen(aName) + 1;
    uint32_t aVerts;
    memcpy(&aVerts, sData + sPtr, sizeof(aVerts));
    sPtr += sizeof(aVerts);
    uint32_t aCount;
    memcpy(&aCount, sData + sPtr, sizeof(aCount));
    sPtr += sizeof(aCount);
    GLResouceType aType;
    memcpy(&aType.attribute, sData + sPtr, sizeof(aType.attribute));
    sPtr += sizeof(aType.attribute);
    AssignAttribute(aName, aType.attribute, sData + sPtr, aCount, aVerts);
    int aDataSize = GetAttributeSize(aType.attribute) * aCount * aVerts;
    sPtr += aDataSize;
  }
  if(pLength)
    *pLength = sPtr;
}

RenderObject::~RenderObject()
{
  Clear();
}
