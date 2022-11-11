//提供 C++ 方法的映射能力
#pragma once
#include "lua.hpp"

using namespace std;
class LuaAPI
{
private:
    /* data */
public:
    // C++ 方法注册到Lua虚拟机
    static int Register(lua_State *luaState);
    static int NewService(lua_State *luaState);
    static int KillService(lua_State *luaState);
    static int Send(lua_State *luaState);
    static int Listen(lua_State *luaState);
    static int Close(lua_State *luaState);
    static int Write(lua_State *luaState);
};
