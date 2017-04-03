#include "File.h"
#include "stdio.h"
#include "stdint.h"

void* File::ReadFile(const char *a_filePath, int64_t *a_fileLength, bool a_textMode)
{
  FILE *pFile;
  fopen_s(&pFile, a_filePath, a_textMode ? "r" : "rb");
  if (pFile != nullptr)
  {
    char *buffer;
    int64_t len;
    _fseeki64(pFile, 0, SEEK_END);
    len = _ftelli64(pFile);
    _fseeki64(pFile, 0, SEEK_SET);
    buffer = new char[len + (a_textMode ? 1 : 0)];
    int64_t readLength = fread_s(buffer, len, 1, len, pFile);
    if (a_fileLength)
      *a_fileLength = readLength;
    if(a_textMode)
      buffer[readLength] = 0;
    fclose(pFile);
    return buffer;
  }
  *a_fileLength = 0;
  return nullptr;
}

bool File::WriteFile(const char* a_filePath, void* a_buffer, int64_t a_dataLength, bool a_textMode)
{
  FILE* file;
  fopen_s(&file, a_filePath, a_textMode ? "w" : "wb");
  if (file != nullptr)
  {
    fwrite(a_buffer, a_dataLength, 1, file);
    fclose(file);
    return true;
  }
  return false;
}

int64_t File::FileSize(const char* a_filePath)
{
  FILE *pFile;
  fopen_s(&pFile, a_filePath, "rb");
  if (pFile != nullptr)
  {
    char *buffer;
    int64_t len;
    _fseeki64(pFile, 0, SEEK_END);
    len = _ftelli64(pFile);
    _fseeki64(pFile, 0, SEEK_SET);
    fclose(pFile);
    return len;
  }
  return 0;
}

bool File::FileExists(const char* fileName)
{
  if (!fileName)
    return false;
  if (fileName[0] == NULL)
    return false;
  FILE* file;
  fopen_s(&file, fileName, "rb");
  if (file)
  {
    fclose(file);
    return true;
  }
  return false;
}

bool File::DelFile(const char *a_filePath)
{
  if (!FileExists(a_filePath))
    return false;
  remove(a_filePath);
  return true;
}
