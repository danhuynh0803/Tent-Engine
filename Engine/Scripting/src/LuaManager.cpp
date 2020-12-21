#include "Scripting/LuaManager.h"
#include <string>
#include "Core/Log.h"
#include <cstdlib>

LuaData LuaManager::data;
lua_State* LuaManager::L;

void LuaManager::Init()
{
    L = luaL_newstate();
    luaL_openlibs(L);
}

void LuaManager::Shutdown()
{
    lua_close(L);
}

void LuaManager::SetEventCallback(const LuaEventCallbackFn& callback)
{
	data.EventCallback = callback;
}

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

bool LuaManager::SetGlobalLuaTable(const char* name, lua_State* L2)
{
    if (!lua_istable(L, -1)) return false;
    lua_newtable(L);
    LuaUtility::TransferTable(L2, L);
    lua_setglobal(L, name);
    return true;
}

bool LuaManager::GetGlobalLuaNumber(const char* name, lua_Number& number)
{
    lua_getglobal(L, name);
    if(lua_isnumber(L,-1))
    {
        lua_pop(L, 1);
        number = lua_tonumber(L, -1);
        return true;
    }
    return false;
}

bool LuaManager::GetGlobalLuaInteger(const char* name, lua_Integer& value)
{
    lua_getglobal(L, name);
    if (lua_isnumber(L, -1))
    {
        lua_pop(L, 1);
        value = lua_tointeger(L, -1);
        return true;
    }
    return false;
}

bool LuaManager::GetGlobalLuaString(const char* name, const char* value)
{
    lua_getglobal(L, name);
    if (lua_isstring(L, -1))
    {
        lua_pop(L, 1);
        value = lua_tostring(L, -1);
        return true;
    }
    return false;
}

bool LuaManager::GetGlobalLuaBoolean(const char* name, bool& value)
{
    lua_getglobal(L, name);
    if (lua_isboolean(L, -1))
    {
        lua_pop(L, 1);
        value = lua_toboolean(L, -1);
        return true;
    }
    return false;
}

bool LuaManager::GetGlobalLuaTable(const char* name, lua_State* L2)
{
    lua_getglobal(L, name);
    if (!lua_istable(L, -1)) return false;
    lua_newtable(L2);
    LuaUtility::TransferTable(L, L2);
    return true;
}

bool LuaUtility::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void LuaUtility::TransferTable(lua_State* L1, lua_State* L2)
{
    if (!lua_istable(L1, -1)) return;
    lua_pushnil(L1);
    while (lua_next(L1, -2) != 0)
    {
        if (lua_isstring(L1,-2))
        {
            if (lua_isinteger(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_pushinteger(L2, lua_tointeger(L1, -1));
                lua_settable(L2, -3);
            }
            else if (lua_isnumber(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_pushnumber(L2, lua_tonumber(L1, -1));
                lua_settable(L2, -3);
            }
            else if (lua_isstring(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_pushstring(L2, lua_tostring(L1, -1));
                lua_settable(L2, -3);
            }
            else if (lua_isboolean(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_pushboolean(L2, lua_toboolean(L1, -1));
                lua_settable(L2, -3);
            }
            else if (lua_istable(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_newtable(L2);
                TransferTable(L1, L2);
                lua_settable(L2, -3);
            }
        }
        else if (lua_isnumber(L1, -2))
        {
            if (lua_isinteger(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_pushinteger(L2, lua_tointeger(L1, -1));
                lua_settable(L2, -3);

            }
            else if (lua_isnumber(L1, -1))
            {
                lua_pushstring(L2, std::to_string(lua_tonumber(L1, -2)).c_str());
                lua_pushnumber(L2, lua_tonumber(L1, -1));
                lua_settable(L2, -3);
            }
            else if (lua_isstring(L1, -1))
            {
                lua_pushstring(L2, std::to_string(lua_tonumber(L1, -2)).c_str());
                lua_pushstring(L2, lua_tostring(L1, -1));
                lua_settable(L2, -3);
            }
            else if (lua_isboolean(L1, -1))
            {
                lua_pushstring(L2, std::to_string(lua_tonumber(L1, -2)).c_str());
                lua_pushboolean(L2, lua_toboolean(L1, -1));
                lua_settable(L2, -3);
            }
            else if (lua_istable(L1, -1))
            {
                lua_pushstring(L2, std::to_string(lua_tonumber(L1, -2)).c_str());
                lua_newtable(L2);
                TransferTable(L1, L2);
                lua_settable(L2, -3);
            }
        }
        lua_pop(L1, 1);
    }
}

JsonObject LuaUtility::SerializeLuaTable(lua_State* L, JsonObject& json)
{
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            JsonObject subJson;
            switch (lua_type(L, -1))//value type
            {
                case LUA_TNONE:
                    break;
                case LUA_TNIL:
                    switch (lua_type(L, -2))//key type
                    {
                        case LUA_TNUMBER:
                            json[std::to_string(lua_tointeger(L, -2))] = nullptr;
                            break;
                        case LUA_TSTRING:
                            json[lua_tostring(L, -2)] = nullptr;
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                case LUA_TBOOLEAN:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            json[std::to_string(lua_tonumber(L, -2))] = lua_toboolean(L,-1);
                            break;
                        case LUA_TSTRING:
                            json[lua_tostring(L, -2)] = lua_toboolean(L, -1);
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                case LUA_TNUMBER:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            if (lua_isinteger(L, -1))
                            {
                                json[std::to_string(lua_tointeger(L, -2))] = lua_tointeger(L, -1);
                            }
                            else
                            {
                                json[std::to_string(lua_tointeger(L, -2))] = lua_tonumber(L, -1);
                            }
                            break;
                        case LUA_TSTRING:
                            json[lua_tostring(L, -2)] = lua_tonumber(L, -1);
                            break;
                        default:
                            break;
                    }
                    break;
                case LUA_TSTRING:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            json[std::to_string(lua_tointeger(L, -2))] = lua_tostring(L, -1);
                            break;
                        case LUA_TSTRING:
                            json[lua_tostring(L, -2)] = lua_tostring(L, -1);
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                case LUA_TTABLE:
                    switch (lua_type(L, -2))
                    {
                        case LUA_TNUMBER:
                            json[std::to_string(lua_tointeger(L, -2))] = SerializeLuaTable(L, subJson);
                            break;
                        case LUA_TSTRING:
                            json[lua_tostring(L, -2)] = SerializeLuaTable(L, subJson);
                            break;
                        default:
                            //other type of key
                            break;
                    }
                    break;
                default:
                    break;
            }
            lua_pop(L, 1);
        }
    }
	return json;
}

void LuaUtility::DeseralizeLuaTable(lua_State* L, JsonObject jsonObject)
{
	//push a table back to the target lua state
    lua_newtable(L);
    for (auto& [key, value] : jsonObject.items()) 
    {
        if (is_number(key))
        {
            if (value.is_null())
            {
                lua_pushnil(L);
                lua_pushinteger(L, std::atoi(key.c_str()));
                lua_rawset(L, -3);
            }
            else
            if (value.is_boolean())
            {
                lua_pushboolean(L, (bool)value);//or json[key]
                lua_pushinteger(L, std::atoi(key.c_str()));
                lua_rawset(L, -3);
            }
            else
            if (value.is_number_float())
            {
                lua_pushnumber(L, (double)value);
                lua_pushinteger(L, std::atoi(key.c_str()));
                lua_rawset(L, -3);
            }
            else
            if (value.is_number_integer())
            {
                lua_pushinteger(L, (lua_Integer)value);
                lua_pushinteger(L, std::atoi(key.c_str()));
                lua_rawset(L, -3);
            }
            else
            if (value.is_object())
            {
                DeseralizeLuaTable(L, value);
                lua_pushinteger(L, std::atoi(key.c_str()));
                lua_rawset(L, -3);
            }
        }
        else
        {
            if (value.is_null())
            {
                lua_pushnil(L);
                lua_pushstring(L, key.c_str());
                lua_rawset(L, -3);
            }
            else
            if (value.is_boolean())
            {
                lua_pushboolean(L, (bool)value);
                lua_pushstring(L, key.c_str());
                lua_rawset(L, -3);
            }
            else
            if (value.is_number_float())
            {
                lua_pushnumber(L, (double)value);
                lua_pushstring(L, key.c_str());
                lua_rawset(L, -3);
            }
            else
            if (value.is_number_integer())
            {
                lua_pushinteger(L, (lua_Integer)value);
                lua_pushstring(L, key.c_str());
                lua_rawset(L, -3);
            }
            else
            if (value.is_object())
            {
                DeseralizeLuaTable(L, (JsonObject)value);
                lua_pushstring(L, key.c_str());
                lua_rawset(L, -3);
            }
        }
    }
}

void LuaUtility::ParseLuaTableOnTop(lua_State *L, const char* tabs)
{
    if (lua_istable(L, -1))
    {
        std::stringstream tabStream;
        LOG_INFO("{0}{1}", tabs, "{");
        tabStream << tabs << "\t";
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            switch (lua_type(L, -1))//value type
            {
            case LUA_TNONE:
                CORE_INFO("What is the case for it to be NONE?");
                break;
            case LUA_TNIL:
                switch (lua_type(L, -2))//key type
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]: nil", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]: nil", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TBOOLEAN:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2), lua_toboolean(L, -1));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2), lua_toboolean(L, -1));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TLIGHTUSERDATA:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TNUMBER:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2), lua_tonumber(L, -1));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2), lua_tonumber(L, -1));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TSTRING:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]: \"{3}\"", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2), lua_tostring(L, -1));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]: \"{3}\"", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2), lua_tostring(L, -1));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TTABLE:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    ParseLuaTableOnTop(L,tabStream.str().c_str());
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    //LuaParseTableOnTop(tabStream.str().c_str());
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TFUNCTION:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TUSERDATA:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TTHREAD:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            default:
                CORE_INFO("What is the case for this? Something went wrong :(");
                break;
            }
            lua_pop(L, 1);
        }
        LOG_INFO("{0}{1}", tabs, "}");
    }
}

int LuaUtility::Log(lua_State* L)
{
    luaL_checkstring(L, -1);
    const char* msg = lua_tostring(L, -1);
    LOG_INFO(msg);
    return 0;
}