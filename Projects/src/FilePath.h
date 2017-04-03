#ifndef FilePath_h__
#define FilePath_h__
#include <string>

struct FilePath
{
  FilePath(const char *text);

  std::string GetName();
  std::string GetNameNoExt();
  std::string GetExtension();
  std::string GetFolder();

  std::string path;
};

#endif // FilePath_h__
