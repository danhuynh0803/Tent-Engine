#pragma once

#include <map>
#include <utility>
#include <entt.hpp>

#include "Core/Base.h"
#include "Scene/Camera.h"
#include "Scene/Skybox.h"

// TODO fix this miss of circular dependencies
// Getting difficult to get correct ordering of includes
class Entity;
class SceneManager;
class SceneRenderer;

class Scene
{
public:
    Scene(bool isNewScene = true);
    void Init();
    void OnEvent(Event& e);

    void DeepCopy(const SharedPtr<Scene>& other);
    void OnStart();
    void OnStop();
    void OnUpdate(float dt);
    void OnRender(float dt, const Camera& camera);

    template <typename T, typename... Args>
    auto GetAllEntitiesWith() { return registry.view<T, Args...>(); }

    //auto GetAllEntitiesWithTag(const std::string& tag) { return registry.view<T>(); }
    Entity CreateEntity(const std::string& name, bool isRootEntity = true);
    Entity CreateEntity(const std::string& name, uint64_t ID, bool isRootEntity = true);
    Entity DuplicateEntity(const Entity& entity);
    void RemoveEntity(Entity entity);
    void Clear() { registry.clear(); }

    const std::map<uint64_t, Entity> GetEntityMap() { return entityMap; }

public:
    SharedPtr<Skybox> skybox;
private:
    friend class Entity;
    friend class Skybox;
    friend class SceneRenderer;
    friend class SceneManager;

    std::string name = "Untitled";
    //entt::entity sceneEntity;
    entt::registry registry;
    std::map<uint64_t, Entity> entityMap;
};