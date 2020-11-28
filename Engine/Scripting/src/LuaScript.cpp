#include "Scene/ScriptableEntity.h"
#include "Scripting/LuaScript.h"
#include "Scripting/Lua/LuaTransform.h"
#include "Scripting/Lua/LuaTag.h"
#include "Scripting/Lua/LuaEntity.h"
#include "Scripting/Lua/LuaInput.h"
#include "Scripting/Lua/LuaRigidbody.h"
#include "Scripting/Lua/LuaAudioSource.h"
#include "Scripting/Lua/LuaVector.h"
#include "Scripting/Lua/LuaCollider.h"
#include "Scripting/Lua/LuaEntity.h"
#include "Core/Log.h"
#include  <type_traits>
#include "Physics/Collider.h"



static int Log(lua_State* L)
{
    luaL_checkstring(L, -1);
    const char* msg = lua_tostring(L, -1);
    LOG_INFO(msg);
    return 0;
}

void LuaScript::Start()
{
    L = luaL_newstate();
    luaL_openlibs(L); //opens all standard Lua libraries

    // Setup logs
    lua_pushcfunction(L, Log);
    lua_setglobal(L, "Log");

    /* for objects managed by lua stack(gc and all that)
    char* transformComponetMetaTableName = "TransformComponetMetaTable";
    {luaL_newmetatable(L, transformComponetMetaTableName);
        lua_pushstring(L, "__index");
        lua_pushcfunction(L, LuaTransfrom::LuaTransformTableIndex);//indexing method for the Transfrom table above
    }lua_settable(L, -3); //sets the (meta)table and pop above
    */
    lua_newtable(L);
    {
        lua_pushcfunction(L, LuaVector::LuaVec2);
        lua_setfield(L, -2, "New");
    }
    lua_setglobal(L, "Vec2");

    lua_newtable(L);
    {
        lua_pushcfunction(L, LuaVector::LuaVec3);
        lua_setfield(L, -2, "New");
    }
    lua_setglobal(L, "Vec3");

    lua_newtable(L);
    {
        lua_pushcfunction(L, LuaVector::LuaVec4);
        lua_setfield(L, -2, "New");
    }
    lua_setglobal(L, "Vec4");

    char* vec2MetaTableName = "LuaVec2MetaTable";
    luaL_newmetatable(L, vec2MetaTableName);
    lua_pushstring(L, "__add");
    lua_pushcfunction(L, LuaVector::LuaVec2Add);
    lua_settable(L, -3);
    lua_pushstring(L, "__sub");
    lua_pushcfunction(L, LuaVector::LuaVec2Sub);
    lua_settable(L, -3);

    char* vec3MetaTableName = "LuaVec3MetaTable";
    luaL_newmetatable(L, vec3MetaTableName);
    lua_pushstring(L, "__add");
    lua_pushcfunction(L, LuaVector::LuaVec3Add);
    lua_settable(L, -3);
    lua_pushstring(L, "__sub");
    lua_pushcfunction(L, LuaVector::LuaVec3Sub);
    lua_settable(L, -3);

    char* vec4MetaTableName = "LuaVec4MetaTable";
    luaL_newmetatable(L, vec4MetaTableName);
    lua_pushstring(L, "__add");
    lua_pushcfunction(L, LuaVector::LuaVec4Add);
    lua_settable(L, -3);
    lua_pushstring(L, "__sub");
    lua_pushcfunction(L, LuaVector::LuaVec4Sub);
    lua_settable(L, -3); //sets the (meta)table and pop above

    LuaPushComponetTable<TransformComponent>();
    LuaPushComponetTable<RigidbodyComponent>();
    LuaPushComponetTable<AudioSource>();
    LuaPushComponetTable<Colliders>();

    lua_newtable(L);
    {
        lua_pushlightuserdata(L, this);
        lua_pushcclosure(L, LuaEntity::Instantiate, 1);
        lua_setfield(L, -2, "Instantiate");
    }
    lua_setglobal(L, "Entity"); //name the table entity

    lua_newtable(L);
    {
        LuaPushCFunctionWithTag(LuaTag::GetTag,"GetTag");
    }
    lua_setglobal(L, "Tag");//name the table Tag

    lua_newtable(L);
    {
        luaL_setfuncs(L, LuaInput::inputLib, 0);
    }
    lua_setglobal(L, "Input");//name the table Input

    luaL_dofile(L, filepath.c_str());
    lua_pushcfunction(L, LuaScriptCallBack::LuaCallback);
    lua_getglobal(L, "Start");
    if (lua_pcall(L, 0, 0, -2) != LUA_OK)
    {
        LOG_ERROR("Cannot run Start() within {0}. Error: {1}", filepath, lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

void LuaScript::Update(float dt)
{
    lua_pushnumber(L, dt);
    lua_setglobal(L, "deltatime");
    luaL_dofile(L, filepath.c_str());
    lua_pushcfunction(L, LuaScriptCallBack::LuaCallback);
    lua_getglobal(L, "Update");
    if (lua_pcall(L, 0, 0, -2) != LUA_OK)
    {
        LOG_ERROR("Cannot run Update() within {0}. Error: {1}", filepath, lua_tostring(L, -1));
        lua_pop(L, 1);
    }
#ifdef  _DEBUG 
    luaL_checktype(L, -1, LUA_TFUNCTION);
    lua_pop(L, 1); //pop the LuaCallback function
#else
    lua_pop(L, 1);
#endif //  _DEBUG 
}

void LuaScript::Destroy()
{
    lua_close(L);
}

void LuaScript::OnTriggerEnter(Entity other)
{
    luaL_dofile(L, filepath.c_str());
    lua_pushcfunction(L, LuaScriptCallBack::LuaCallback);
    lua_getglobal(L, "OnTriggerEnter");
    LuaPushEntity(other);
    if (lua_pcall(L, 1, 0, -3) != LUA_OK)
    {
        LOG_ERROR("Cannot run OnTriggerEnter() within {0}. Error: {1}", filepath, lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

void LuaScript::LuaPushCFunctionWithEntity(const lua_CFunction& f, const char* name)
{
    lua_pushlightuserdata(L, &entity);
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithRigidbody(Entity entity, const lua_CFunction& f, const char* name)
{
    SharedPtr<Rigidbody> rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;
    lua_pushlightuserdata(L, rigidbody.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}


void LuaScript::LuaPushCFunctionWithRigidbody(const lua_CFunction& f, const char* name)
{
    SharedPtr<Rigidbody> rigidbody = GetComponent<RigidbodyComponent>().rigidbody;
    lua_pushlightuserdata(L, rigidbody.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithTag(const lua_CFunction& f, const char* name)
{
    const char* tag = GetComponent<TagComponent>().tag.c_str(); lua_pushstring(L, tag);
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushCFunctionWithAudioSource(const lua_CFunction& f, const char* name)
{
    SharedPtr<AudioSource> audioSource = GetComponent<AudioComponent>().audioSource;
    lua_pushlightuserdata(L, audioSource.get());
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name);
}

void LuaScript::LuaPushEntity(Entity entity)
{
    lua_newtable(L);

    if (entity.HasComponent<TagComponent>())
    {
        lua_pushstring(L, entity.GetComponent<TagComponent>().tag.c_str());
        lua_setfield(L, -2, "tag");
    }

    if (entity.HasComponent<TransformComponent>())
    {
        lua_newtable(L);
        {
            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::SetEntityPosition, 1);
            lua_setfield(L, -2, "SetPosition");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().euler));
            lua_pushcclosure(L, LuaTransfrom::SetEntityRotation, 1);
            lua_setfield(L, -2, "SetRotation");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().scale));
            lua_pushcclosure(L, LuaTransfrom::SetEntityScale, 1);
            lua_setfield(L, -2, "SetScale");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::Translate, 1);
            lua_setfield(L, -2, "Translate");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::GetEntityPosition, 1);
            lua_setfield(L, -2, "GetPosition");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().rotation));
            lua_pushcclosure(L, LuaTransfrom::GetEntityRotation, 1);
            lua_setfield(L, -2, "GetRotation");

            lua_pushlightuserdata(L, &(entity.GetComponent<TransformComponent>().scale));
            lua_pushcclosure(L, LuaTransfrom::GetEntityScale, 1);
            lua_setfield(L, -2, "GetScale");
        }
        lua_setfield(L, -2, "Transform");
    }

    if (entity.HasComponent<RigidbodyComponent>())
    {
        lua_newtable(L);
        {
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::AddVelocity, "AddVelocity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetVelocity, "GetVelocity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetVelocity, "SetVelocity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetMass, "GetMass");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetMass, "SetMass");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetDrag, "GetDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetDrag, "SetDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::GetAngularDrag, "GetAngularDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::SetAngularDrag, "SetAngularDrag");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::UseGravity, "UseGravity");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::FreezePosition, "FreezePosition");
            LuaPushCFunctionWithRigidbody(entity, LuaRigidbody::FreezeRotation, "FreezeRotation");
        }
        lua_setfield(L, -2, "Rigidbody");
    }

    if (entity.HasComponent<AudioComponent>())
    {

    }

    //if (entity.HasComponent<CameraComponent>())
    //{
    //}
    //if (entity.HasComponent<MeshComponent>())
    //{
    //}
    //if (entity.HasComponent<SpriteComponent>())
    //{
    //}
    //if (entity.HasComponent<LightComponent>())
    //{
    //}
    //if (entity.HasComponent<Colliders>())
    //{
    //}
    //if (entity.HasComponent<TextComponent>())
    //{
    //}
    //if (entity.HasComponent<ScriptComponent>())
    //{
    //}
}

template <class T>
void LuaScript::LuaPushComponetTable()
{
    if (HasComponent<T>())
    {
        if (std::is_same<T, TransformComponent>::value)
        {
            lua_newtable(L);
            //luaL_setfuncs(L, LuaTransfrom::transformLib, 0);

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::SetEntityPosition, 1);
            lua_setfield(L, -2, "SetPosition");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().euler));
            lua_pushcclosure(L, LuaTransfrom::SetEntityRotation, 1);
            lua_setfield(L, -2, "SetRotation");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().scale));
            lua_pushcclosure(L, LuaTransfrom::SetEntityScale, 1);
            lua_setfield(L, -2, "SetScale");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::Translate, 1);
            lua_setfield(L, -2, "Translate");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::GetEntityPosition, 1);
            lua_setfield(L, -2, "GetPosition");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().rotation));
            lua_pushcclosure(L, LuaTransfrom::GetEntityRotation, 1);
            lua_setfield(L, -2, "GetRotation");

            lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().scale));
            lua_pushcclosure(L, LuaTransfrom::GetEntityScale, 1);
            lua_setfield(L, -2, "GetScale");
            lua_setglobal(L, "Transform");
        }
        else if (std::is_same<T, RigidbodyComponent>::value)
        {
            lua_newtable(L);
            LuaPushCFunctionWithRigidbody(LuaRigidbody::AddVelocity, "AddVelocity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetVelocity, "GetVelocity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetVelocity, "SetVelocity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetMass, "GetMass");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetMass, "SetMass");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetDrag, "GetDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetDrag, "SetDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::GetAngularDrag, "GetAngularDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::SetAngularDrag, "SetAngularDrag");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::UseGravity, "UseGravity");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::FreezePosition, "FreezePosition");
            LuaPushCFunctionWithRigidbody(LuaRigidbody::FreezeRotation, "FreezeRotation");
            lua_setglobal(L, "Rigidbody");
        }
        else if (std::is_same<T, AudioComponent>::value)
        {
            lua_newtable(L);
            LuaPushCFunctionWithAudioSource(LuaAudioSource::Play, "Play");
            LuaPushCFunctionWithAudioSource(LuaAudioSource::Pause, "Pause");
            LuaPushCFunctionWithAudioSource(LuaAudioSource::Stop, "Stop");
            lua_setglobal(L, "AudioSource");
        }
        else if (std::is_same<T, BoxCollider>::value)
        {
            lua_newtable(L);
            lua_pushlightuserdata(L, &(GetComponent<BoxCollider>().center));
            lua_pushcclosure(L, LuaCollider::Box::GetCenter, 1);
            lua_setfield(L, -2, "GetCenter");
            lua_setglobal(L, "BoxCollider");
        }
        else if (std::is_same<T, SphereCollider>::value)
        {
            lua_newtable(L);
            lua_pushlightuserdata(L, &(GetComponent<SphereCollider>().center));
            lua_pushcclosure(L, LuaCollider::Sphere::GetCenter, 1);
            lua_setfield(L, -2, "GetCenter");
            lua_setglobal(L, "SphereCollider");
        }
        else if (std::is_same<T, CapsuleCollider>::value)
        {
            lua_newtable(L);
            lua_pushlightuserdata(L, &(GetComponent<CapsuleCollider>().center));
            lua_pushcclosure(L, LuaCollider::Capsule::GetCenter, 1);
            lua_setfield(L, -2, "GetCenter");
            lua_setglobal(L, "CapsuleCollider");
        }
        else
        {
            CORE_WARN("{0} was found, but Lua Table for was not created", typeid(T).name());
        }
    }
    else
    {
        CORE_WARN("Missing {0} component", typeid(T).name());
    }
}

int LuaScriptCallBack::LuaCallback(lua_State* L)
{
    const char* message = lua_tostring(L, 1);
    if (message)
    {
        //replace and push an error message that includes stack trace
        luaL_traceback(L, L, message, 1); 
    }
    return 1;
}
