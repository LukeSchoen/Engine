#ifndef Lua_h__
#define Lua_h__

class Lua
{
public:
  Lua();

  bool RunText(const char *text);
  bool RunFile(const char *file);

  template<typename T>
  void ExposeFunction(const char *functionName, T&& cFunction);

private:
  void *pLua;
};

#endif // Lua_h__
