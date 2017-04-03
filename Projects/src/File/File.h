#ifndef File_h__
#define File_h__

#include "stdint.h"
#include <stdio.h>
#include <string>

struct File
{
  static void *ReadFile(const char *a_filePath, int64_t *a_fileLength = nullptr, bool textMode = false);
  static bool WriteFile(const char *fileName, void* buffer, int64_t dataLength, bool textMode = false);
  static int64_t FileSize(const char *a_filePath);
  static bool FileExists(const char *a_filePath);
  static bool DelFile(const char *a_filePath);
};
#endif // File_h__
