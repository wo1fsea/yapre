#pragma once

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include <string>
#include <string_view>

namespace yapre {
namespace lua {

extern lua_State *mainLuaState;

template <typename T> struct StateVar {
  static inline void Put(lua_State *l, T value);
  static inline T Get(lua_State *l, int index);
};

template <> struct StateVar<void> {
  static inline void Put(lua_State *l) { lua_pushnil(l); }
};

template <> struct StateVar<bool> {
  static inline void Put(lua_State *l, bool value) {
    lua_pushboolean(l, value);
  }
  static inline bool Get(lua_State *l, int index) {
    return lua_toboolean(l, index);
  }
};

template <> struct StateVar<int> {
  static inline void Put(lua_State *l, int value) { lua_pushinteger(l, value); }
  static inline int Get(lua_State *l, int index) {
    return luaL_checkinteger(l, index);
  }
};

template <> struct StateVar<float> {
  static inline void Put(lua_State *l, float value) {
    lua_pushnumber(l, value);
  }
  static inline float Get(lua_State *l, int index) {
    return (float)luaL_checknumber(l, index);
  }
};

template <> struct StateVar<double> {
  static inline void Put(lua_State *l, double value) {
    lua_pushnumber(l, value);
  }
  static inline double Get(lua_State *l, int index) {
    return (double)luaL_checknumber(l, index);
  }
};

template <> struct StateVar<std::string> {
  static inline void Put(lua_State *l, std::string value) {
    lua_pushstring(l, value.c_str());
  }
  static inline std::string Get(lua_State *l, int index) {
    return luaL_checkstring(l, index);
  }
};

template <size_t args_size, typename R, typename... Targs> struct _StateCall {};

template <typename R, typename... Targs> struct StateCall {
  static inline R Call(lua_State *l, Targs... args) {
    return _StateCall<sizeof...(Targs), R, Targs...>::Call(l, args...);
  }
};

template <size_t args_size, typename R, typename T, typename... Targs>
struct _StateCall<args_size, R, T, Targs...> {
  static inline R Call(lua_State *l, T t, Targs... args) {
    StateVar<T>::Put(t);
    return _StateCall<sizeof...(Targs), R, Targs...>::Call(l, args...);
  }
};

template <size_t args_size, typename R> struct _StateCall<args_size, R> {
  static inline R Call(lua_State *l) {
    // todo check pcall
    lua_pcall(l, args_size, 1, 0);
    R result = StateVar<R>::Get(l, -1);
    lua_pop(l, 1);
    return result;
  }
};

template <size_t args_size> struct _StateCall<args_size, void> {
  static inline void Call(lua_State *l) {
    // todo check pcall
    lua_pcall(l, args_size, 0, 0);
  }
};

template <typename R, typename... Targs> struct GStateCall {
  static inline R Call(Targs... args) {
    return StateCall<R, Targs...>::Call(mainLuaState, args...);
  }
};

template <typename R, typename... Targs> struct StateFunc {
  std::string functionName;
  R Call(lua_State *l, Targs... args) {
    lua_getglobal(l, functionName.c_str());
    return StateCall<R, Targs...>::Call(l, args...);
  }
};

template <typename R, typename... Targs> struct GStateFunc {
  std::string functionName;
  R Call(Targs... args) {
    lua_getglobal(mainLuaState, functionName.c_str());
    return StateCall<R, Targs...>::Call(mainLuaState, args...);
  }
};

template <typename R> struct StateCall<R, void> {
  static inline R Call(lua_State *l) { return _StateCall<0, R>::Call(l); }
};

template <typename R> struct StateFunc<R, void> {
  std::string functionName;
  R Call(lua_State *l) {
    lua_getglobal(l, functionName.c_str());
    return StateCall<R>::Call(l);
  }
};

template <typename R> struct GStateFunc<R, void> {
  std::string functionName;
  R Call() {
    lua_getglobal(mainLuaState, functionName.c_str());
    return StateCall<R>::Call(mainLuaState);
  }
};

} // namespace lua
} // namespace yapre
