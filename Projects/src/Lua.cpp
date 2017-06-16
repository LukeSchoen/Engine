#include "Lua.h"
#include "..\..\3rdParty\Lua\include\sol.hpp"

bool _handleLuaCall(sol::protected_function_result callResult)
{
  if (!callResult.valid())
  {
    sol::error e = callResult;
    printf(e.what());
    return false;
  }
  return true;
}

Lua::Lua()
{
  sol::state *rlua = new sol::state;
  rlua->open_libraries();
  pLua = rlua;
}

bool Lua::RunText(const char *text)
{
  return _handleLuaCall((*(sol::state*)pLua).script(text, sol::simple_on_error));
}

bool Lua::RunFile(const char *file)
{
  return _handleLuaCall((*(sol::state*)pLua).script_file(file, sol::simple_on_error));
}

template<typename T>
void Lua::ExposeFunction(const char *functionName, T&& cFunction)
{
  (*(sol::state*)pLua).set_function(functionName, cFunction);
}

