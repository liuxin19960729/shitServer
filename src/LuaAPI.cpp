#include "LuaAPI.hpp"
#include <cstdint>
#include "ShitNet.hpp"
#include <unistd.h>
#include <string.h>

/**
 * lua 数据类型
 * nil  boolean number string function userdata thread table
 *
 * 引用类型:string function userdata thread table
 *
 *
 * int LuaAPI::XXXX(lua_State*)
 * int 返回值代表的是返回值的个数
 *
 */

int LuaAPI::Register(lua_State *luaState)
{
    // note:最后一个元素必须是nullptr
    static luaL_Reg cOrCppLibs[] = {
        {"NewService", NewService}, //函数名 函数指针
        {"KillService", KillService},
        {"Send", Send},
        {nullptr, nullptr}};
    luaL_newlib(luaState, cOrCppLibs);  //在栈中创建一张新的表
    lua_setglobal(luaState, "ShitNet"); //将栈顶元素放入全局空间 并且命名
}

int LuaAPI::NewService(lua_State *luaState)
{
    // lua_gettop() 返回栈顶元素的索引(参数的个数)
    int argNum = lua_gettop(luaState);
    //参数1 不是 string  lua_isstring 判断栈中的元素
    // index + 栈底->栈顶  - 相反
    if (argNum != 1 || !!lua_isinteger(luaState, 1))
    {
        lua_pushinteger(luaState, -1);
        return 1;
    }

    size_t len = 0;
    //获取luaString参数 len 参数代表字符串的长度
    // 该字符串由lua虚拟机管理
    const char *type = lua_tolstring(luaState, 1, &len);
    char *newStr = new char[len + 1]; //+'\0'
    newStr[len] = '\0';
    memcpy(newStr, type, len);
    auto t = make_shared<string>(newStr);
    auto id = ShitNet::Inst()->NewService(t);
    lua_pushinteger(luaState, id); //返回 lua 方法返回 id
    return 1;
}
int LuaAPI::KillService(lua_State *luaState)
{
    int argNum = lua_gettop(luaState);
    if (argNum != 1 || !lua_isinteger(luaState, 1))
        return 0;
    int id = lua_tointeger(luaState, 1);
    ShitNet::Inst()->Killservice(id);
    return 0;
}
int LuaAPI::Send(lua_State *luaState)
{
    int argNum = lua_gettop(luaState);
    if (argNum != 3)
    {
        cout << "Send fail ,args num!=3 error:" << argNum << endl;
        return 0;
    }
    if (!lua_isinteger(luaState, 1))
    {
        cout << "Send fail ,arg1 不是 interger" << endl;
        return 0;
    }
    auto source = lua_tointeger(luaState, 1);
    if (!lua_isinteger(luaState, 2))
    {
        cout << "Send fail ,arg2 不是 interger" << endl;
        return 0;
    }
    auto toId = lua_tointeger(luaState, 2);

    if (!lua_isstring(luaState, 3))
    {
        cout << "Send fail ,arg2 不是 string" << endl;
        return 0;
    }
    size_t len = 0;
    const char *type = lua_tolstring(luaState, 3, &len);
    char *newStr = new char[len]; //+'\0'
    memcpy(newStr, type, len);
    auto msg = make_shared<ServiceMsg>();
    msg->type = BaseMsg::Type::SERVICE;
    msg->source = source;
    msg->buf = shared_ptr<char>(newStr);
    msg->size = len;
    ShitNet::Inst()->Send(toId, msg);
    return 0;
}
int LuaAPI::Listen(lua_State *luaState)
{
}
int LuaAPI::Close(lua_State *luaState)
{
}
int LuaAPI::Write(lua_State *luaState)
{
}