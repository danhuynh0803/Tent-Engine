#pragma once

#include <cstdarg>
#include <entt.hpp>

#include "Core/Base.h"
#include "Scene/Scene.h"
#include "Scene/Component.h"

class Entity
{
public:
    Entity() = default;
    Entity(entt::entity handle, Scene* _scene);
    ~Entity() = default;

    template <typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        return scene->registry.template emplace<T>(entityHandle, std::forward<Args>(args)...);
    }

    template <typename T>
    T& GetComponent()
    {
        if (HasComponent<T>())
            return scene->registry.template get<T>(entityHandle);
        else
            LOG_ERROR("Component not found");
    }

    template <typename T>
    bool HasComponent()
    {
        return scene->registry.template has<T>(entityHandle);
    }

    template <typename T>
    void RemoveComponent()
    {
        scene->registry.template remove<T>(entityHandle);
    }

    operator bool() const { return entityHandle != entt::null; }
    operator entt::entity() const { return entityHandle; }

private:
    friend class Scene;
    friend class ScriptableEntity;

    entt::entity entityHandle { entt::null };

    // Scene that this entity belongs to
    Scene* scene = nullptr;
};
