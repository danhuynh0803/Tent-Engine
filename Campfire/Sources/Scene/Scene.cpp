#include "Scene/Scene.h"

#include <imgui.h>

Scene::Scene()
{
    /*
       Scene should be initialized with the following:
       1) Camera
       2) Directional light
    */

    auto mainCamera = CreateEntity("camera");
    mainCamera.AddComponent<TransformComponent>();

}

void Scene::Init()
{

}

void Scene::OnUpdate(float timestep)
{

}

void Scene::OnEvent(Event& e)
{

}

void Scene::OnImGuiRender()
{
}

void Scene::SetSkybox(SharedPtr<TextureCube> skyboxTex)
{
    //skybox->Load()
}

Entity Scene::CreateEntity(const std::string& name)
{
    auto entity = Entity(registry.create(), this);
    entity.AddComponent<TagComponent>(name);

    // TODO figure out a identifier system later
    entityMap[0] = entity;

    return entity;
}

void Scene::RemoveEntity(entt::entity entity)
{
    registry.destroy(entity);
}


