#pragma once
#include <lua.hpp>
#include "Scene/ScriptableEntity.h"

namespace LuaScriptCallBack {
    int LuaCallback(lua_State*);
}

class LuaScript : public ScriptableEntity
{

public:
    ~LuaScript()
    {
        lua_close(L);
    }

    void Start();
    void Update(float);
    void Destroy();

    void OnTriggerEnter(Entity);
    void OnTriggerStay(Entity);
    void OnTriggerExit(Entity);

    void LuaPushEntity(Entity, lua_State*);
private:
    lua_State* L;
    void LuaPushEntity(Entity);
    void LuaPushCFunctionWithEntity(const lua_CFunction&, const char*);
    void LuaPushCFunctionWithRigidbody(Entity, const lua_CFunction&, const char*);
    void LuaPushCFunctionWithRigidbody(const lua_CFunction&, const char*);
    void LuaPushCFunctionWithTag(const lua_CFunction&, const char*);
    void LuaPushCFunctionWithAudioSource(const lua_CFunction&, const char*);
    template<typename T>
    void LuaPushComponetTable();
};
