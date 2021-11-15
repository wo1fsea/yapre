#pragma once

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace yapre {
namespace lua {

lua_State *GetMainLuaState();

static void DumpStack(lua_State *l) {
  int top = lua_gettop(l);
  printf("DumpStack\n");
  for (int i = 1; i <= top; i++) {
    printf("%d\t%s\t", i, luaL_typename(l, i));
    switch (lua_type(l, i)) {
    case LUA_TNUMBER:
      printf("%g\n", lua_tonumber(l, i));
      break;
    case LUA_TSTRING:
      printf("%s\n", lua_tostring(l, i));
      break;
    case LUA_TBOOLEAN:
      printf("%s\n", (lua_toboolean(l, i) ? "true" : "false"));
      break;
    case LUA_TNIL:
      printf("%s\n", "nil");
      break;
    default:
      printf("%p\n", lua_topointer(l, i));
      break;
    }
  }
}

static void GDumpStack() { DumpStack(GetMainLuaState()); }

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

template <typename... Targs> struct StateVar<std::tuple<Targs...>> {
  using TupleType = std::tuple<Targs...>;
  template <size_t idx> using GetElement = std::tuple_element<idx, TupleType>;

  template <size_t idx> static void _Put(lua_State *l, TupleType value) {
    StateVar<typename GetElement<idx>::Type>::Put(l, std::get<idx>(value));
    lua_rawseti(l, -2, idx + 1);
    if (idx < sizeof...(Targs)) {
      _Put<idx + 1>(l, value);
    }
  }

  static inline void Put(lua_State *l, TupleType value) {
    lua_newtable(l);
    _Put<0>(l, value);
  }

  static inline TupleType Get(lua_State *l, int index) {
    return _Get(std::make_index_sequence<sizeof...(Targs)>{}, l, index);
  }

  template <std::size_t... Is>
  static inline TupleType _Get(std::index_sequence<Is...> const &, lua_State *l,
                               int index) {
    TupleType t;
    int top_of_stack = lua_gettop(l);
    if (top_of_stack != 0) {
      for (lua_Integer i = 1; i <= sizeof...(Targs); ++i) {
        lua_geti(l, top_of_stack, i);
      }
      t = std::make_tuple(
          StateVar<Targs>::Get(l, (int)(-sizeof...(Targs) + Is))...);
    }

    lua_settop(l, top_of_stack);
    return t;
  }
};

template <typename T> struct StateVar<std::vector<T>> {
  using VectorType = std::vector<T>;
  static inline void Put(lua_State *l, VectorType value) {
    lua_newtable(l);
    int idx = 1;
    for (auto &item : value) {
      StateVar<T>::Put(l, item);
      lua_rawseti(l, -2, idx++);
    }
  }

  static inline VectorType Get(lua_State *l, int index) {
    VectorType v;
    int top_of_stack = lua_gettop(l);
    if (top_of_stack != 0) {
      for (lua_Integer i = 1; lua_geti(l, top_of_stack, i) != LUA_TNIL; ++i) {
        T item = StateVar<T>::Get(l, -1);
        v.emplace_back(item);
        lua_settop(l, top_of_stack);
      }
    }
    return v;
  }
};

template <typename T> struct StateVar<std::unordered_map<std::string, T>> {
  using MapType = std::unordered_map<std::string, T>;
  static inline void Put(lua_State *l, MapType value) {
    lua_newtable(l);
    int idx = 1;
    for (auto &item : value) {
      StateVar<T>::Put(l, item.second);
      lua_setfield(l, -2, item.first.c_str());
    }
  }
  static inline MapType Get(lua_State *l, int index) {
    int top_of_stack = lua_gettop(l);
    MapType m;

    if (top_of_stack != 0) {
      lua_pushnil(l);
      while (lua_next(l, top_of_stack) != 0) {
        auto v = StateVar<T>::Get(l, -1);
        std::string k = StateVar<std::string>::Get(l, -2);
        m.emplace(k, v);
        lua_pop(l, 1);
      }
    }
    lua_settop(l, top_of_stack);
    return m;
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

static const char *StateVarStdFuncionMeta = "StateVarStdFuncionMeta";

template <typename R, typename... Targs> struct _CFuncWrapper;
template <typename R, typename... Targs>
struct StateVar<std::function<R(Targs...)>> {
  using FuncType = std::function<R(Targs...)>;

  static int __gc(lua_State *l) {
    delete *static_cast<FuncType **>(
        luaL_checkudata(l, 1, StateVarStdFuncionMeta));
    return 0;
  }

  static inline void Put(lua_State *l, const FuncType &value) {
    void *user_data_ptr = lua_newuserdata(l, sizeof(FuncType *));
    *static_cast<FuncType **>(user_data_ptr) = new FuncType(value);

    if (luaL_newmetatable(l, StateVarStdFuncionMeta)) {
      static const luaL_Reg __index[] = {{"__gc", __gc}, {nullptr, nullptr}};
      luaL_setfuncs(l, __index, 0);
      lua_pushvalue(l, -1);
      lua_setfield(l, -2, "__index");
    }
    lua_setmetatable(l, -2);

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
    if (lua_pcall(l, args_size, 1, 0) != 0) {
      std::cout << lua_tostring(l, -1) << std::endl;
    }
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
    if (lua_pcall(l, args_size, 0, 0) != 0) {
      std::cout << lua_tostring(l, -1) << std::endl;
      lua_pop(l, 1);
    }
  }
};

template <typename R, typename... Targs>
LuaFuncVar<R, Targs...> GetGolbalFunc(lua_State *l, const std::string &name) {
  lua_getglobal(l, name.c_str());
  auto fun_var = StateVar<R(Targs...)>::Get(l, -1);
  lua_pop(l, 1);
  return fun_var;
}

template <typename R, typename... Targs>
LuaFuncVar<R, Targs...> GGetGolbalFunc(const std::string &name) {
  auto l = GetMainLuaState();
  lua_getglobal(l, name.c_str());
  auto fun_var = StateVar<R(Targs...)>::Get(l, -1);
  lua_pop(l, 1);
  return fun_var;
}

template <typename R, typename... Targs> struct _CFuncWrapper {
  using FuncType = std::function<R(Targs...)>;
  template <std::size_t... Is>
  static int _Call(std::index_sequence<Is...> const &, lua_State *l) {
    auto func =
        reinterpret_cast<FuncType **>(lua_touserdata(l, lua_upvalueindex(1)));
    R result = (**func)(
        StateVar<std::remove_cv_t<std::remove_reference_t<Targs>>>::Get(
            l, (int)(-sizeof...(Targs) + Is))...);
    lua_settop(l, 0);
    StateVar<R>::Put(l, result);
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
        reinterpret_cast<FuncType **>(lua_touserdata(l, lua_upvalueindex(1)));
    (**func)(StateVar<std::remove_cv_t<std::remove_reference_t<Targs>>>::Get(
        l, (int)(-sizeof...(Targs) + Is))...);
    lua_settop(l, 0);
    return 0;
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
