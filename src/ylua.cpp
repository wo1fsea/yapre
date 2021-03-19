#include "ylua.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "yluabind.hpp"

namespace yapre {
namespace lua {

const char *kDefaultLuaEntryFilePath = "data/lua/yapre.lua";
lua_State *mainLuaState = nullptr;

lua_State *GetMainLuaState() {
  if (!mainLuaState){
  mainLuaState = luaL_newstate();
  luaL_openlibs(mainLuaState);
  }
  return mainLuaState;
}

bool Init() {
  luaL_dofile(GetMainLuaState(), kDefaultLuaEntryFilePath);
  GStateFunc<bool>{"Init"}.Call();
  return true;
}

void Deinit() {
  GStateFunc<void>{"Deinit"}.Call();
  lua_close(mainLuaState);
}

void Update() { GStateFunc<void>{"Update"}.Call(); }

} // namespace lua
} // namespace yapre
