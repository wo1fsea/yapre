#include "ylua.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include <string_view>
#include <string>

namespace yapre {
    namespace lua {

        lua_State *luaState = nullptr;
        
        template <typename T> 
        struct State{
            static inline void Put(T value);
            static inline T Get(int index);
        };
        
        template <> 
        struct State<void>{
            static inline void Put(){ lua_pushnil(luaState); }
        };

        template <> 
        struct State<bool>{
            static inline void Put(bool value){ luaL_checkinteger(luaState, value); }
            static inline bool Get(int index){ return luaL_checkinteger(luaState, index) == 0; }
        };

        template <> 
        struct State<int>{
            static inline void Put(int value){ lua_pushinteger(luaState, value); }
            static inline int Get(int index){ return luaL_checkinteger(luaState, index); }
        };

        template <> 
        struct State<float>{
            static inline void Put(float value){ lua_pushnumber(luaState, value); }
            static inline float Get(int index){ return (float)luaL_checknumber(luaState, index); }
        };

        template <> 
        struct State<double>{
            static inline void Put(double value){ lua_pushnumber(luaState, value); }
            static inline double Get(int index){ return (double)luaL_checknumber(luaState, index); }
        };

        template <> 
            struct State<std::string>{
                static inline void Put(std::string value){ lua_pushstring(luaState, value.c_str());  }
                static inline std::string Get(int index){ return luaL_checkstring(luaState, index); }        
            };
        
        template <typename R, typename... Targs> 
        struct StateFunc
        {
            static R Call(Targs... args)
            {
                return _StateFunc<sizeof...(Targs), R, Targs...>::Call(args...);
            }
        };

        template <size_t args_size, typename R, typename... Targs> 
        struct _StateFunc{};
        
        template <size_t args_size, typename R, typename T, typename... Targs> 
        struct _StateFunc<args_size, R, T, Targs...>
        {
            static R Call(T t, Targs... args)
            {
                State<T>::Put(t);
                return _StateFunc<sizeof...(Targs), R, Targs...>::Call(args...);
            }
        };       
               
        template <size_t args_size, typename R> 
        struct _StateFunc<args_size, R>
        {
            static R Call()
            {
                lua_pcall(luaState, args_size, 1, 0);
                return State<R>::Get(-1);
            }
        };
/*
        template <typename... Targs> 
        struct StateFunc<void, typename... Targs>
        {
            static void Call(Targs... args)
            {
                 
            }
        }; 
*/
        void LoadLuaFile(const std::string& luaFilePath)
        {
            luaL_dofile(luaState, luaFilePath.c_str());
        }

        bool CallLuaInit()
        {
            lua_getglobal(luaState, "Init");
            StateFunc<bool, int>::Call(1);
            lua_pcall(luaState, 0, 1, 0);
            bool result = (bool)lua_toboolean(luaState, -1);
            lua_pop(luaState, 1);

            return result;
        }

        bool Init() {
            luaState = luaL_newstate();
            luaL_openlibs(luaState);

            LoadLuaFile("data/lua/yapre.lua");

            return CallLuaInit();
        }

        void Deinit() 
        {
            lua_getglobal(luaState, "Deinit");
            lua_pcall(luaState, 0, 0, 0);
            lua_close(luaState);
        }

        void Update() 
        {
            lua_getglobal(luaState, "Update");
            lua_pcall(luaState, 0, 0, 0);
        }

    } // namespace lua
} // namespace yapre
