#include "ylua.h"
#include <functional>
#include <iostream>
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
  int result = luaL_dofile(GetMainLuaState(), kDefaultLuaEntryFilePath);
  if (result != 0) {
    auto s = lua_tostring(GetMainLuaState(), -1);
    std::cout << s << std::endl;
    return false;
  }

  return GGetGolbalFunc<bool>("Init")();
}

void Deinit() {
  GGetGolbalFunc<void>("Deinit")();
  lua_close(mainLuaState);
}

void Update(int delta_ms) { 
    GGetGolbalFunc<void, int>("Update")(delta_ms); 
}

} // namespace lua
} // namespace yapre
