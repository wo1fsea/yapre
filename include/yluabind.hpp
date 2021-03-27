#pragma once

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include <functional>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

namespace yapre {
namespace lua {

lua_State *GetMainLuaState();

template <typename T, typename Enable = void> struct StateVar {
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

template <typename T>
struct StateVar<T, std::enable_if_t<std::is_integral<T>::value>> {
  static inline void Put(lua_State *l, int value) { lua_pushinteger(l, value); }
  static inline int Get(lua_State *l, int index) {
    return (T)luaL_checkinteger(l, index);
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
  static inline void Put(lua_State *l, const std::string &value) {
    lua_pushstring(l, value.c_str());
  }
  static inline std::string Get(lua_State *l, int index) {
    return luaL_checkstring(l, index);
  }
};

template <> struct StateVar<char const *> {
  static inline void Put(lua_State *l, char const *value) {
    lua_pushstring(l, value);
  }
  static inline char const *Get(lua_State *l, int index) {
    return luaL_checkstring(l, index);
  }
};

template <typename R> struct StateCall;
template <typename R, typename... Targs> class LuaFuncVar {
  int ref_;
  lua_State *l_;

public:
  LuaFuncVar(lua_State *l) : ref_(LUA_REFNIL), l_(l) {}
  LuaFuncVar(lua_State *l, int index) : ref_(LUA_REFNIL), l_(l) {
    lua_pushvalue(l_, index);
    ref_ = luaL_ref(l_, LUA_REGISTRYINDEX);
  }

  LuaFuncVar(const LuaFuncVar<R, Targs...> &other) {
    l_ = other.l_;
    lua_rawgeti(l_, LUA_REGISTRYINDEX, other.ref_);
    ref_ = luaL_ref(l_, LUA_REGISTRYINDEX);
  }
  const LuaFuncVar<R, Targs...> &
  operator=(const LuaFuncVar<R, Targs...> &other) {
    l_ = other.l_;
    lua_rawgeti(l_, LUA_REGISTRYINDEX, other.ref_);
    ref_ = luaL_ref(l_, LUA_REGISTRYINDEX);
    return *this;
  }
  virtual ~LuaFuncVar() { luaL_unref(l_, LUA_REGISTRYINDEX, ref_); }

  R operator()(Targs... args) {
    lua_rawgeti(l_, LUA_REGISTRYINDEX, ref_);
    return StateCall<R>::Call(l_, args...);
  }
};

template <typename R, typename... Targs> struct _CFuncWrapper;
template <typename R, typename... Targs>
struct StateVar<std::function<R(Targs...)>> {
  static inline void Put(lua_State *l,
                         const std::function<R(Targs...)> &value) {
    void *user_data_ptr = lua_newuserdata(l, sizeof(value));
    auto user_data_func_ptr =
        new (user_data_ptr) std::function<R(Targs...)>(value);
    lua_pushcclosure(l, _CFuncWrapper<R, Targs...>::Call, 1);
  }
  static inline LuaFuncVar<R, Targs...> Get(lua_State *l, int index) {
    return LuaFuncVar<R, Targs...>(l, index);
  }
};

template <typename R, typename... Targs>
struct StateVar<R (*)(Targs...)> : StateVar<std::function<R(Targs...)>> {};
template <typename R, typename... Targs>
struct StateVar<R(Targs...)> : StateVar<std::function<R(Targs...)>> {};

template <size_t args_size, typename R> struct _StateCall {
  template <typename T, typename... Targs>
  static inline R Call(lua_State *l, T t, Targs... args) {
    StateVar<std::remove_cv_t<std::remove_reference_t<T>>>::Put(l, t);
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
  template <typename T, typename... Targs>
  static inline void Call(lua_State *l, T t, Targs... args) {
    StateVar<std::remove_cv_t<std::remove_reference_t<T>>>::Put(l, t);
    return _StateCall<args_size, void>::Call(l, args...);
  }

  static inline void Call(lua_State *l) {
    // todo check pcall
    lua_pcall(l, args_size, 0, 0);
  }
};

template <typename R, typename... Targs>
LuaFuncVar<R, Targs...> GetGolbalFunc(lua_State *l, const std::string &name) {
  lua_getglobal(l, name.c_str());
  auto fun_var = StateVar<R(Targs...)>::Get(l, -1);
  return fun_var;
}

template <typename R, typename... Targs>
LuaFuncVar<R, Targs...> GGetGolbalFunc(const std::string &name) {
  auto l = GetMainLuaState();
  lua_getglobal(l, name.c_str());
  auto fun_var = StateVar<R(Targs...)>::Get(l, -1);
  return fun_var;
}

template <typename R, typename... Targs> struct _CFuncWrapper {
  using FuncType = std::function<R(Targs...)>;
  template <std::size_t... Is>
  static int _Call(std::index_sequence<Is...> const &, lua_State *l) {
    auto func =
        reinterpret_cast<FuncType *>(lua_touserdata(l, lua_upvalueindex(1)));
    StateVar<R>::Put(
        l,
        (*func)(StateVar<std::remove_cv_t<std::remove_reference_t<Targs>>>::Get(
            l, (int)(-sizeof...(Targs) + Is))...));
    return 1;
  }
  static int Call(lua_State *l) {
    return _Call(std::make_index_sequence<sizeof...(Targs)>{}, l);
  }
};

template <typename... Targs> struct _CFuncWrapper<void, Targs...> {
  using FuncType = std::function<void(Targs...)>;
  template <std::size_t... Is>
  static int _Call(std::index_sequence<Is...> const &, lua_State *l) {
    auto func =
        reinterpret_cast<FuncType *>(lua_touserdata(l, lua_upvalueindex(1)));
    (*func)(StateVar<std::remove_cv_t<std::remove_reference_t<Targs>>>::Get(
        l, ((uint32_t)Is - sizeof...(Targs)))...);
    return 1;
  }
  static int Call(lua_State *l) {
    return _Call(std::make_index_sequence<sizeof...(Targs)>{}, l);
  }
};

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
