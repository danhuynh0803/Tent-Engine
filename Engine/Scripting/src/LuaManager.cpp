#include "Scripting/LuaManager.h"
#include "Scripting/LuaUtility.h"

//LuaData LuaManager::data;
lua_State* LuaManager::L;
LuaManager::JsonObject LuaManager::LuaGlobal;

void LuaManager::Init()
{
    if (L) return;
    L = luaL_newstate();
    luaL_openlibs(L);
    lua_newtable(L);
    lua_setglobal(L, LUA_MANAGER_GLOBAL);
    LuaGlobal = JsonObject({});
}

void LuaManager::Shutdown()
{
    if (!L) return;
    LuaGlobal = JsonObject({});
    lua_close(L);
}

void LuaManager::OnStart()
{
    Init();
}

void LuaManager::OnStop()
{
    Shutdown();
}

//void LuaManager::SetEventCallback(const LuaEventCallbackFn& callback)
//{
//	data.EventCallback = callback;
//}

void LuaManager::SetGlobalLuaNumber(const char* name, const lua_Number& number)
{
    lua_pushnumber(L, number);
    lua_setglobal(L, name);
}

void LuaManager::SetGlobalLuaInteger(const char* name, const lua_Integer& integer)
{
    lua_pushinteger(L, integer);
    lua_setglobal(L, name);
}

void LuaManager::SetGlobalLuaString(const char* name, const char* string)
{
    lua_pushstring(L, string);
    lua_setglobal(L, name);
}

void LuaManager::SetGlobalLuaBoolean(const char* name, const bool& boolean)
{
    lua_pushboolean(L, boolean);
    lua_setglobal(L, name);
}

bool LuaManager::SetGlobalLuaTable(lua_State* from, const char* name)
{
    //check name here to prevent bad assignment
    if (!lua_istable(from, -1)) return false;
    lua_newtable(L);
    if (!LuaUtility::TransferTable(from, L))
    {
        lua_pop(L, 1);
        return false;
    }
    lua_setglobal(L, name);
    return true;
}

bool LuaManager::SetGlobalLuaTableNumber(const char* table, const char* name, const lua_Number& number)
{
    lua_getglobal(L, table);
    lua_pushnumber(L, number);
    lua_setfield(L, -2, name);
    lua_setglobal(L, table);
    if (strcmp(table, LUA_MANAGER_GLOBAL) == 0)
    {
        LuaGlobal = SerializeLuaGlobalTable();
    }
    return true;
}

bool LuaManager::SetGlobalLuaTableInteger(const char* table, const char* name, const lua_Integer& integer)
{
    lua_getglobal(L, table);
    lua_pushinteger(L, integer);
    lua_setfield(L, -2, name);
    lua_setglobal(L, table);
    if (strcmp(table, LUA_MANAGER_GLOBAL) == 0)
    {
        LuaGlobal = SerializeLuaGlobalTable();
    }
    return true;
}

bool LuaManager::SetGlobalLuaTableString(const char* table, const char* name, const char* string)
{
    lua_getglobal(L, table);
    lua_pushstring(L, string);
    lua_setfield(L, -2, name);
    lua_setglobal(L, table);
    if (strcmp(table, LUA_MANAGER_GLOBAL) == 0)
    {
        LuaGlobal = SerializeLuaGlobalTable();
    }
    return true;
}

bool LuaManager::SetGlobalLuaTableBoolean(const char* table, const char* name, const bool& boolean)
{
    lua_getglobal(L, table);
    lua_pushboolean(L, boolean);
    lua_setfield(L, -2, name);
    lua_setglobal(L, table);
    if (strcmp(table, LUA_MANAGER_GLOBAL) == 0)
    {
        LuaGlobal = SerializeLuaGlobalTable();
    }
    return true;
}

bool LuaManager::SetGlobalLuaTableTable(lua_State* from, const char* table, const char* name)
{
    if (!lua_istable(from, -1)) return false;
    lua_getglobal(L, table);
    lua_newtable(L);
    if (!LuaUtility::TransferTable(from, L))
    {
        lua_pop(L, 2);
        return false;
    }
    lua_setfield(L, -2, name);
    lua_setglobal(L, table);
    if (strcmp(table, LUA_MANAGER_GLOBAL) == 0)
    {
        LuaGlobal = SerializeLuaGlobalTable();
    }
    return true;
}

bool LuaManager::GetGlobalLuaNumber(lua_State* to, const char* name)
{
    lua_getglobal(L, name);
    if(lua_isnumber(L,-1))
    {
        lua_pushnumber(to, lua_tonumber(L, -1));
        lua_pop(L, 1);
        return true;
    }
    lua_pop(L, 1);
    return false;
}

bool LuaManager::GetGlobalLuaInteger(lua_State* to, const char* name)
{
    lua_getglobal(L, name);
    if (lua_isinteger(L, -1))
    {
        lua_pushinteger(to, lua_tointeger(L, -1));
        lua_pop(L, 1);
        return true;
    }
    lua_pop(L, 1);
    return false;
}

bool LuaManager::GetGlobalLuaString(lua_State* to, const char* name)
{
    lua_getglobal(L, name);
    if (lua_isstring(L, -1))
    {
        lua_pushstring(to, lua_tostring(L, -1));
        lua_pop(L, -1);
        return true;
    }
    lua_pop(L, 1);
    return false;
}

bool LuaManager::GetGlobalLuaBoolean(lua_State* to ,const char* name)
{
    lua_getglobal(L, name);
    if (lua_isboolean(L, -1))
    {
        lua_pushboolean(to, lua_toboolean(L, -1));
        lua_pop(L, 1);
        return true;
    }
    lua_pop(L, 1);
    return false;
}

bool LuaManager::GetGlobalLuaTable(lua_State* to, const char* name)
{
    lua_getglobal(L, name);
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }
    lua_newtable(to);
    if (!LuaUtility::TransferTable(L, to))
    {
        lua_pop(to,1);
        lua_pop(L, 1);
        return false;
    }
    lua_pop(L, 1);//pop the very first table from the getglobal
    return true;
}

bool LuaManager::GetGlobalLuaTableNumber(lua_State* to, const char* table, const char* name)
{
    lua_getglobal(L, table);
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }
    lua_getfield(L, -1, name);
    if (lua_isnumber(L, -1))
    {
        lua_pushnumber(to, lua_tonumber(L, -1));
        lua_pop(L, 2);
        return true;
    }
    lua_pop(L, 2);
    return false;
}

bool LuaManager::GetGlobalLuaTableInteger(lua_State* to, const char* table, const char* name)
{
    lua_getglobal(L, table);
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }
    lua_getfield(L, -1, name);
    if (lua_isinteger(L, -1))
    {
        lua_pushinteger(to, lua_tointeger(L, -1));
        lua_pop(L, 2);
        return true;
    }
    lua_pop(L, 2);
    return false;
}

bool LuaManager::GetGlobalLuaTableString(lua_State* to, const char* table, const char* name)
{
    lua_getglobal(L, table);
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }
    lua_getfield(L, -1, name);
    if (lua_isstring(L, -1))
    {
        lua_pushstring(to, lua_tostring(L, -1));
        lua_pop(L, 2);
        return true;
    }
    lua_pop(L, 2);
    return false;
}

bool LuaManager::GetGlobalLuaTableBoolean(lua_State* to, const char* table, const char* name)
{
    lua_getglobal(L, table);
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }
    lua_getfield(L, -1, name);
    if (lua_isboolean(L, -1))
    {
        lua_pushboolean(to, lua_toboolean(L, -1));
        lua_pop(L, 2);
        return true;
    }
    lua_pop(L, 2);
    return false;
}

bool LuaManager::GetGlobalLuaTableTable(lua_State* to, const char* table, const char* name)
{
    lua_getglobal(L, table);
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }
    lua_getfield(L, -1, name);
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 2);
        return false;
    }
    lua_newtable(to);
    if (!LuaUtility::TransferTable(L, to))
    {
        lua_pop(to, 1);
        lua_pop(L, 2);
        return false;
    }
    lua_pop(L, 2);
    return true;
}

LuaManager::JsonObject LuaManager::SerializeLuaGlobalTable()
{
    lua_getglobal(L, LUA_MANAGER_GLOBAL);
    JsonObject json;
    LuaUtility::SerializeLuaTable(L, json);
    lua_pop(L, 1);
    return json;
}

LuaManager::JsonObject LuaManager::SerializeLuaGlobalTable(const char* name)
{
    lua_getglobal(L, name);
    JsonObject json;
    LuaUtility::SerializeLuaTable(L, json);
    lua_pop(L, 1);
    return json;
}

const LuaManager::JsonObject& LuaManager::GetLuaGlobal()
{
    return LuaGlobal;
}

void LuaManager::Find(const char* name)
{
    //find the all value with key and the name and return a table 
}