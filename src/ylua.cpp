#include "ylua.h"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include <string_view>

namespace yapre {
    namespace lua {


        lua_State *luaState = nullptr;

        void LoadLuaFile(const std::string& luaFilePath)
        {
            luaL_dofile(luaState, luaFilePath.c_str());
        }

        bool CallLuaInit()
        {
            lua_getglobal(luaState, "Init");
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
            lua_close(luaState);
        }

        void Update() 
        {
            lua_getglobal(luaState, "Update");
            lua_pcall(luaState, 0, 0, 0);
        }

    } // namespace lua
} // namespace yapre
