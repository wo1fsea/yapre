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

bool Init() {
  mainLuaState = luaL_newstate();
  luaL_openlibs(mainLuaState);
  luaL_dofile(mainLuaState, kDefaultLuaEntryFilePath);

  return GStateFunc<bool, void>{"Init"}.Call();
}

void Deinit() {
  GStateFunc<void>{"Deinit"}.Call();
  lua_close(mainLuaState);
}

void Update() { GStateFunc<void>{"Update"}.Call(); }

} // namespace lua
} // namespace yapre
