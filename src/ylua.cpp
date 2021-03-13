#include "ylua.h"
extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}
#include <stdio.h>
#include <stdlib.h>

namespace yapre {
namespace lua {

static const luaL_Reg lualibs[] = {{"base", luaopen_base}, {NULL, NULL}};
static void openlualibs(lua_State *l) {
  const luaL_Reg *lib;

  for (lib = lualibs; lib->func != NULL; lib++) {
    lib->func(l);
    lua_settop(l, 0);
  }
}

bool to_stop = false;
bool Init() {

  lua_State *l;
  l = luaL_newstate();
  openlualibs(l);

  luaL_dofile(l, "data/lua/test.lua");

  lua_close(l);
  // return (result && status == LUA_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
  return true;
}

void Deinit() {}

void Update() {}
} // namespace lua
} // namespace yapre
