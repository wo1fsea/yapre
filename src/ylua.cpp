#include "ylua.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "yluabind.hpp"

namespace yapre {
namespace lua {

lua_State *mainLuaState = nullptr;

void LoadLuaFile(const std::string &luaFilePath) {
  luaL_dofile(mainLuaState, luaFilePath.c_str());
}

bool Init() {
  mainLuaState = luaL_newstate();
  luaL_openlibs(mainLuaState);
  LoadLuaFile("data/lua/yapre.lua");

  return GStateFunc<bool, void>{"Init"}.Call();
}

void Deinit() {
  GStateFunc<void>{"Deinit"}.Call();
  lua_close(mainLuaState);
}

void Update() { GStateFunc<void, void>{"Update"}.Call(); }

} // namespace lua
} // namespace yapre
