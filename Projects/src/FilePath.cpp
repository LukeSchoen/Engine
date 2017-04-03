#include "FilePath.h"
#include "Maths.h"

FilePath::FilePath(const char *filePath)
{
  path = filePath;
}

std::string FilePath::GetName()
{
  int lf = path.rfind('//', path.npos);
  int lb = path.rfind('\\', path.npos);
  return path.substr(Max(lf, lb), path.npos);
}

std::string FilePath::GetNameNoExt()
{
  int lf = path.rfind('//', path.npos);
  int lb = path.rfind('\\', path.npos);
  int ld = path.rfind('.', path.npos);
  int ls = Max(lf, lb);
  return path.substr(ls, ld - ls);
}

std::string FilePath::GetExtension()
{
  int ld = path.rfind('.', path.npos);
  return path.substr(ld, path.npos);
}

std::string FilePath::GetFolder()
{
  int lf = path.rfind('//', path.npos);
  int lb = path.rfind('\\', path.npos);
  int ld = path.rfind('.', path.npos);
  int ls = Max(lf, lb);
  return path.substr(0, ls);
}

