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

const char *kDefaultLuaEntryFilePath = "./lua/yapre.lua";
lua_State *mainLuaState = nullptr;

lua_State *GetMainLuaState() {
  if (!mainLuaState) {
    mainLuaState = luaL_newstate();
    luaL_openlibs(mainLuaState);
  }
  return mainLuaState;
}

std::vector<std::string> GetV(std::vector<std::string> v) {
  for (auto i : v) {
    std::cout << i << std::endl;
  }
  return v;
}

std::unordered_map<std::string, std::string>
GetM(std::unordered_map<std::string, std::string> m) {
  for (auto i : m) {
    std::cout << i.first << ", " << i.second << std::endl;
  }
  return m;
}

std::unordered_map<std::string, std::tuple<int, double, std::string>>
GetTT(std::unordered_map<std::string, std::tuple<int, double, std::string>> t) {
  return t;
}

std::tuple<int, double, std::string>
GetT(std::tuple<int, double, std::string> t) {
  return t;
}

class Object {
private:
  int x = 11;

public:
  Object(int x) : x(x) {}
  Object() {}
  std::vector<std::string> Print(std::vector<std::string> v) {
    for (auto i : v) {
      std::cout << i << std::endl;
    }
    std::cout << x << x << std::endl;
    return v;
  }
  void t() {}
};

bool Init() {
  // todo: test
  GStateModule{"yapre"}.Define("GetV", GetV);
  GStateModule{"yapre"}.Define("GetM", GetM);
  GStateModule{"yapre"}.Define("GetT", GetT);
  Object obj(1);
  auto a = std::function([&obj]() { obj.t(); });
  GStateModule{"yapre"}.Define("OF", a);

  LuaClass<Object>(GetMainLuaState(), "yapre", "Object")
      .Member("a", &Object::Print)
      .Ctor<>("f0")
      .Ctor<int>("f1")
      .Define();

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
