#include "ylua.h"
#include <functional>
#include <iostream>
#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "debugger_lua.h"
}

#include "yluabind.hpp"

namespace yapre {
namespace lua {

const char *kDefaultLuaEntryFilePath = "./lua/yapre.lua";
lua_State *mainLuaState = nullptr;

lua_State *GetMainLuaState() {
  if (!mainLuaState) {
    mainLuaState = luaL_newstate();
    luaL_openlibs(mainLuaState);
    dbg_setup(mainLuaState, "debugger", "dbg", NULL, NULL);
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
  mainLuaState = nullptr;
}

void Update(int delta_ms) { GGetGolbalFunc<void, int>("Update")(delta_ms); }

std::string DoString(const std::string &input) {
  std::string eval_string = "return " + input;
  std::string result = "nil";
  if (mainLuaState) {
    // int on = lua_gettop(mainLuaState);
    int r = luaL_dostring(mainLuaState, eval_string.c_str());
    if (r != 0) {
      lua_pop(mainLuaState, 1);
      luaL_dostring(mainLuaState, input.c_str());
    }

    int n = lua_gettop(mainLuaState);

    if (n > 0) {
      luaL_checkstack(mainLuaState, LUA_MINSTACK, "too many results");
      lua_getglobal(mainLuaState, "tostring");
      lua_insert(mainLuaState, 1);
      lua_pcall(mainLuaState, n, 1, 0);
      result = StateVar<std::string>::Get(mainLuaState, -1);
      lua_pop(mainLuaState, 1);
    }
  }

  return result;
}

} // namespace lua
} // namespace yapre
