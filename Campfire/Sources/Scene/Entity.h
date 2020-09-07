#pragma once

#include <cstdarg>
#include <entt.hpp>

#include "Core/Base.h"
#include "Scene/Scene.h"
#include "Scene/Component.h"

struct RelationshipComponent
{
    std::size_t numChildren = 0;
    entt::entity first{ entt::null };
    entt::entity prev{ entt::null };
    entt::entity next{ entt::null };
    entt::entity parent{ entt::null };
};

class Entity
{
public:
    Entity() = default;
    Entity(entt::entity handle, Scene* _scene);
    ~Entity() = default;

    void AddChild(entt::entity childHandle)
    {
        auto& comp = GetComponent<RelationshipComponent>();

        if (comp.numChildren == 0)
        {
            comp.first = childHandle;
            comp.parent = entityHandle;
            comp.numChildren++;
            return;
        }

        auto& curr = comp.first;
        auto& prev = comp.prev;
        // Iterate to the end of the children list
        for (std::size_t i {}; i < comp.numChildren-1; ++i)
        {
            prev = curr;
            curr = scene->registry.get<RelationshipComponent>(curr).next;
        }

        auto& currRelation = scene->registry.get<RelationshipComponent>(curr);
        currRelation.next = childHandle;
        auto& nextRelation = scene->registry.get<RelationshipComponent>(currRelation.next);
        nextRelation.prev = curr;
        nextRelation.parent = entityHandle;
        comp.numChildren++;
    }

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
