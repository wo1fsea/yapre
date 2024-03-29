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

template <typename T> struct LuaClassMetaName { static std::string name; };
template <typename T> std::string LuaClassMetaName<T>::name;

template <typename T, typename Enable = void> struct StateVar {
  static inline void Put(lua_State *l, T value);
  static inline T Get(lua_State *l, int index);
};

template <typename T> struct StateVar<T *> {
  static inline void Put(lua_State *l, T *value) {
    *static_cast<T **>(lua_newuserdata(l, sizeof(T *))) = value;
  }
  static inline T *Get(lua_State *l, int index) {
    return *static_cast<T **>(
        luaL_checkudata(l, index, LuaClassMetaName<T>::name.c_str()));
  }
};

struct StateVarT {
  template <typename T> static inline void Put(lua_State *l, T value) {
    StateVar<T>::Put(l, value);
  }
};

struct GStateVarT {
  template <typename T> static inline void Put(T value) {
    StateVarT::Put(GetMainLuaState(), value);
  }
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
  static inline void Put(lua_State *l, T value) { lua_pushinteger(l, value); }
  static inline T Get(lua_State *l, int index) {
    return (T)luaL_checknumber(l, index);
  }
};

template <typename T> struct StateVar<T, std::enable_if_t<std::is_floating_point<T>::value>> {
  static inline T Put(lua_State *l, T value) {
    lua_pushnumber(l, value);
  }
  static inline T Get(lua_State *l, int index) {
    return (T)luaL_checknumber(l, index);
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

  template <size_t idx> inline static void _Put(lua_State *l, TupleType value) {
    if constexpr (idx < sizeof...(Targs)) {
      StateVar<typename GetElement<idx>::type>::Put(l, std::get<idx>(value));
      lua_rawseti(l, -2, idx + 1);
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
    index = index < 0? top_of_stack + index + 1: index;
    if (index <= top_of_stack) {
      for (lua_Integer i = 1; i <= sizeof...(Targs); ++i) {
        lua_geti(l, index, i);
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
    index = index < 0? top_of_stack + index + 1: index;
    if (index <= top_of_stack) {
      for (lua_Integer i = 1; lua_geti(l, index, i) != LUA_TNIL; ++i) {
        T item = StateVar<T>::Get(l, -1);
        v.emplace_back(item);
        lua_settop(l, top_of_stack);
      }
    }
    lua_settop(l, top_of_stack);
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
    MapType m;
    int top_of_stack = lua_gettop(l);
    index = index < 0? top_of_stack + index + 1: index;
    if (index <= top_of_stack) {
      lua_pushnil(l);
      while (lua_next(l, index) != 0) {
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

static const char *StateVarStdFuncionMeta = "STATE_VAR_STD_FUNCTION_META";

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

    lua_settop(l, sizeof...(Targs));

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

    lua_settop(l, sizeof...(Targs));

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
  lua_State *const l;
  const std::string name;

  StateModule(lua_State *l_, const std::string &name_) : l(l_), name(name_) {}

  template <typename T>
  const StateModule &Define(const std::string &key, T value) const {
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

template <typename T> struct _LuaClassMemFuncHelper {
  static inline void Put(lua_State *l, T);
};

template <typename R, typename... Targs>
struct _LuaClassMemFuncHelper<R (*)(Targs...)> {
  static inline std::function<R(Targs...)> _GenMemCallFunc(R (*mem)(Targs...)) {
    return [mem](Targs... args) { return (*mem)(args...); };
  }

  static inline void Put(lua_State *l, R (*mem)(Targs...)) {
    StateVarT::Put(l, _GenMemCallFunc(mem));
  }
};

template <typename T, typename R, typename... Targs>
struct _LuaClassMemFuncHelper<R (T::*)(Targs...)> {
  static inline std::function<R(T *, Targs...)>
  _GenMemCallFunc(R (T::*mem)(Targs...)) {
    return [mem](T *self, Targs... args) {
      //  todo: check self
      return (self->*mem)(args...);
    };
  }

  static inline void Put(lua_State *l, R (T::*mem)(Targs...)) {
    StateVarT::Put(l, _GenMemCallFunc(mem));
  }
};

struct LuaClassMemFuncHelper {
  template <typename T> static inline void Put(lua_State *l, T mem) {
    _LuaClassMemFuncHelper<T>::Put(l, mem);
  }
};

template <typename T, typename... Targs> struct LuaClassCtorFuncHelper {
  template <std::size_t... Is>
  static int _Call(std::index_sequence<Is...> const &, lua_State *l) {

    lua_settop(l, sizeof...(Targs));

    *static_cast<T **>(lua_newuserdata(l, sizeof(T *))) =
        new T(StateVar<std::remove_cv_t<std::remove_reference_t<Targs>>>::Get(
            l, (int)(-sizeof...(Targs) + Is))...);

    if (luaL_getmetatable(l, LuaClassMetaName<T>::name.c_str())) {
      lua_setmetatable(l, -2);
    }
    return 1;
  }

  static int Call(lua_State *l) {
    return _Call(std::make_index_sequence<sizeof...(Targs)>{}, l);
  }
};

template <typename T> struct LuaClass {
  lua_State *const l;
  const std::string name;

  static int _ObjectFree(lua_State *l) {
    delete *static_cast<T **>(
        luaL_checkudata(l, 1, LuaClassMetaName<T>::name.c_str()));
    return 0;
  }

  LuaClass(lua_State *l_, const std::string &module_name,
           const std::string &class_name)
      : l(l_), name(class_name) {

    if (LuaClassMetaName<T>::name == "") {
      LuaClassMetaName<T>::name = "CPP_OBJECT_META[" + name + "]";
    }

    // moudule
    lua_getglobal(l, module_name.c_str());
    if (lua_isnil(l, -1)) {
      lua_pop(l, 1);
      lua_newtable(l);
    }

    // meta table
    if (luaL_newmetatable(l, LuaClassMetaName<T>::name.c_str())) {
      static const luaL_Reg __index[] = {{"__gc", _ObjectFree},
                                         {nullptr, nullptr}};
      luaL_setfuncs(l, __index, 0);
      lua_pushvalue(l, -1);
      lua_setfield(l, -2, "__index");
    }

    lua_setfield(l, -2, name.c_str());
    lua_setglobal(l, module_name.c_str());
    DumpStack(l);
  }

  template <typename F> LuaClass<T> &Member(const std::string &name, F mem) {
    if (luaL_getmetatable(l, LuaClassMetaName<T>::name.c_str())) {
      _LuaClassMemFuncHelper<F>::Put(l, mem);
      lua_setfield(l, -2, name.c_str());
      lua_pop(l, 1);
    }

    return *this;
  }

  template <typename... Targs>
  LuaClass<T> &Ctor(const std::string &name = "new") {
    using CtorFuncHelperType = LuaClassCtorFuncHelper<T, Targs...>;
    if (luaL_getmetatable(l, LuaClassMetaName<T>::name.c_str())) {
      lua_pushcfunction(l, CtorFuncHelperType::Call);
      lua_setfield(l, -2, name.c_str());

      lua_pop(l, 1);
    }
    return *this;
  }
};

template <typename T> struct GLuaClass : LuaClass<T> {
  GLuaClass(const std::string &module_name, const std::string &class_name)
      : LuaClass<T>(GetMainLuaState(), module_name, class_name) {}
};
} // namespace lua
} // namespace yapre
