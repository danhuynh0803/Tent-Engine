#pragma once

#include <lua.hpp>
#include <functional>
#include "Scene/Component.h"

//static const struct luaL_reg {
//    char* methodName;
//    std::function<int(lua_State*)> method;
//} transformLib[] =
//{
//    {"New", NewTransform},
//    {"SetPosition", SetPosition},
//    {"SetEuler", SetRotation},
//    {"SetScale", SetScale},
//
//    {"GetPosition", GetPosition},
//    {"GetEuler", GetRotation},
//    {"GetScale", GetScale}
//};

static int NewTransform(lua_State* L)
{
    void* transformComponent = lua_newuserdata(L, sizeof(TransformComponent));
    new (transformComponent) TransformComponent();
    luaL_getmetatable(L, "TransformComponentMetaTable");
    assert(lua_istable(L, -1));
    lua_setmetatable(L, -2);
    return 1;
}

static int SetPosition(lua_State* L)
{
    TransformComponent* tc = (TransformComponent*)lua_touserdata(L, -4);
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    tc->position = glm::vec3(x, y, z);
    return 0;
}

static int SetRotation(lua_State* L)
{
    return 0;
}

static int SetScale(lua_State* L)
{
    return 0;
}

static int GetPosition(lua_State* L)
{
    return 0;
}

static int GetRotation(lua_State* L)
{
    return 0;
}

static int GetScale(lua_State* L)
{
    return 0;
}

static int ResetTransform(lua_State* L)
{
    TransformComponent* tc = (TransformComponent*)lua_touserdata(L, -1);
    //call tc destructor
    return 0;
}
