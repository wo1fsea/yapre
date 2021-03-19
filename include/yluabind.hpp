#pragma once

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace yapre {
namespace lua {

lua_State *GetMainLuaState();

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

template <> struct StateVar<const std::string &> {
  static inline void Put(lua_State *l, const std::string &value) {
    lua_pushstring(l, value.c_str());
  }
  static inline const std::string Get(lua_State *l, int index) {
    return luaL_checkstring(l, index);
  }
};

template <typename R, typename... Targs>
lua_CFunction MakeCFuncStateVar(R (*func)(Targs...));
template <typename R, typename... Targs> struct StateVar<R (*)(Targs...)> {
  static inline void Put(lua_State *l, R (*value)(Targs...)) {
    lua_pushlightuserdata(l, (void *)value);
    lua_pushcclosure(l, MakeCFuncStateVar(value), 1);
  }
};

template <size_t args_size, typename R> struct _StateCall {
  template <typename T, typename... Targs>
  static inline R Call(lua_State *l, T t, Targs... args) {
    StateVar<T>::Put(l, t);
    return _StateCall<args_size, R>::Call(l, args...);
  }
  static inline R Call(lua_State *l) {
    // todo check pcall
    lua_pcall(l, args_size, 1, 0);
    R result = StateVar<R>::Get(l, -1);
    lua_pop(l, 1);
    return result;
  }
};
template <typename R> struct StateCall {
  template <typename... Targs>
  static inline R Call(lua_State *l, Targs... args) {
    return _StateCall<sizeof...(Targs), R>::Call(l, args...);
  }
};
template <size_t args_size> struct _StateCall<args_size, void> {
  static inline void Call(lua_State *l) {
    // todo check pcall
    lua_pcall(l, args_size, 0, 0);
  }
};

template <typename R> struct GStateCall {
  template <typename... Targs> static inline R Call(Targs... args) {
    return StateCall<R>::Call(GetMainLuaState(), args...);
  }
};

template <typename R> struct StateFunc {
  std::string functionName;
  template <typename... Targs> R Call(lua_State *l, Targs... args) {
    lua_getglobal(l, functionName.c_str());
    return StateCall<R>::Call(l, args...);
  }
};

template <typename R> struct GStateFunc {
  std::string functionName;
  template <typename... Targs> R Call(Targs... args) {
    lua_getglobal(GetMainLuaState(), functionName.c_str());
    return StateCall<R>::Call(GetMainLuaState(), args...);
  }
};

template <typename R, typename... Targs> struct _CFuncWrapper {
  using CFuncType = R (*)(Targs...);
  template <std::size_t... Is>
  static int _Call(std::index_sequence<Is...> const &, lua_State *l) {
    CFuncType func = (CFuncType)lua_touserdata(l, lua_upvalueindex(1));
    StateVar<R>::Put(
        l, func(StateVar<Targs>::Get(l, (int)(-sizeof...(Targs) + Is))...));
    return 1;
  }
  static int Call(lua_State *l) {
    return _Call(std::make_index_sequence<sizeof...(Targs)>{}, l);
  }
};

template <typename... Targs> struct _CFuncWrapper<void, Targs...> {
  using CFuncType = void (*)(Targs...);
  template <std::size_t... Is>
  static int _Call(std::index_sequence<Is...> const &, lua_State *l) {
    CFuncType func = (CFuncType)lua_touserdata(l, lua_upvalueindex(1));
    func(StateVar<Targs>::Get(l, (int)(-sizeof...(Targs) + Is))...);
    return 1;
  }
  static int Call(lua_State *l) {
    return _Call(std::make_index_sequence<sizeof...(Targs)>{}, l);
  }
};

template <typename R, typename... Targs>
lua_CFunction MakeCFuncStateVar(R (*func)(Targs...)) {
  return _CFuncWrapper<R, Targs...>::Call;
}

struct StateModule {
  lua_State *l;
  const std::string name;

  StateModule(lua_State *l_, const std::string &name_) : l(l_), name(name_) {}

  template <typename T> StateModule Define(const std::string &key, T value) {
    lua_getglobal(l, name.c_str());
    if (lua_isnil(l, -1)) {
      lua_pop(l, 1);
      lua_newtable(l);
    }
    StateVar<T>::Put(l, value);
    lua_setfield(l, -2, key.c_str());
    lua_setglobal(l, name.c_str());
    return *this;
  }
};

struct GStateModule : public StateModule {
  GStateModule(const std::string &name)
      : StateModule(GetMainLuaState(), name) {}
};

} // namespace lua
} // namespace yapre
