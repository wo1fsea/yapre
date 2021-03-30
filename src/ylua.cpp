#include "ylua.h"
#include <functional>
#include <string>

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
  if (!mainLuaState) {
    mainLuaState = luaL_newstate();
    luaL_openlibs(mainLuaState);
  }
  return mainLuaState;
}

bool Init() {
  luaL_dofile(GetMainLuaState(), kDefaultLuaEntryFilePath);
  return GGetGolbalFunc<bool>("Init")();
  return true;
}

void Deinit() {
  GGetGolbalFunc<void>("Deinit")();
  lua_close(mainLuaState);
}

void Update() {
  GGetGolbalFunc<void>("Update")();
}

} // namespace lua
} // namespace yapre
