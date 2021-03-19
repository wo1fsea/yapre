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

int t(int i, int j) { return i+j; }
void *testFuc = (void*)t;

bool Init() {
  mainLuaState = luaL_newstate();
  luaL_openlibs(mainLuaState);
  luaL_dofile(mainLuaState, kDefaultLuaEntryFilePath);

  GStateFunc<bool, int, int, int(*)(int,int)>{"Init"}.Call(1,2,t);
  return true;
}

void Deinit() {
  GStateFunc<void>{"Deinit"}.Call();
  lua_close(mainLuaState);
}

void Update() { GStateFunc<void>{"Update"}.Call(); }

} // namespace lua
} // namespace yapre
