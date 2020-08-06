#include "Scene/Scene.h"
#include "Core/Random.h"
#include <imgui.h>

#include "Renderer/Renderer.h"

Scene::Scene()
{
    /*
       Scene should be initialized with the following:
       1) Camera
       2) Directional light
    */

    uboLights = UniformBuffer::Create();
    BufferLayout uboLayout =
    {
        { ShaderDataType::FLOAT4, "pos" },
        { ShaderDataType::FLOAT4, "color" },
        { ShaderDataType::FLOAT4, "attenFactors" }
    };

    /*
     25 is currently the max number of lights specified within the shader,
     but we pass 26 since it's a bit messy otherwise to set in the bufferlayout.
     Data is stored as the following in the shader:

     Lights[25];
     uint numLights; //which comes after the array

    --------------------------

    In terms of memory layout, it looks like:
    Lights[0]
    Lights[1]
    ...
    ...
    Lights[24]
    uint numLights

     */

    uboLights->SetLayout(uboLayout, 1, 26);

    auto& mainCamera = CreateEntity("Camera");
    mainCamera.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, 5.0f);

    auto& directionalLight = CreateEntity("Directional Light");
    directionalLight.AddComponent<LightComponent>();
    directionalLight.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 3.0f, 0.0f);
}

void Scene::Init()
{

}

void Scene::OnUpdate(float dt)
{

}

// Render scene from perspective of editor camera
void Scene::OnRenderEditor(float dt, const Camera& editorCamera)
{
    // Send light info to our UBO
    SubmitLights();

    // Only render objects that have mesh components
    auto group = registry.group<MeshComponent>(entt::get<TransformComponent>);
    for (auto entity : group)
    {
        auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
        if (meshComponent.mesh)
        {
            meshComponent.mesh->OnUpdate(dt);

            Renderer::SubmitMesh(meshComponent, transformComponent);
        }
    }

}

// Render scene from perspective of game camera
void Scene::OnRenderRuntime(float dt)
{
    // Send light info to our UBO
    SubmitLights();

    // Only render objects that have mesh components
    auto group = registry.group<MeshComponent>(entt::get<TransformComponent>);
    for (auto entity : group)
    {
        auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
        if (meshComponent.mesh)
        {
            meshComponent.mesh->OnUpdate(dt);

            Renderer::SubmitMesh(meshComponent, transformComponent);
        }
    }
}

void Scene::SubmitLights()
{
    auto group = registry.group<LightComponent>(entt::get<TransformComponent>);
    uint32_t numLights = 0;

    uboLights->Bind();

    for (auto entity : group)
    {
        auto [transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

        glm::vec4 position = glm::vec4(transformComponent.position, 0.0f);
        glm::vec4 attenFactors =
            glm::vec4(
                lightComponent.constant,
                lightComponent.linear,
                lightComponent.quadratic,
                0.0f // padding
            );

        // position
        uboLights->SetData((void*)glm::value_ptr(position), (3 * sizeof(glm::vec4) * numLights), sizeof(glm::vec4));
        // color
        uboLights->SetData((void*)glm::value_ptr(lightComponent.color), (3 * sizeof(glm::vec4) * numLights) + sizeof(glm::vec4), sizeof(glm::vec4));
        // attenuation factors
        uboLights->SetData((void*)glm::value_ptr(attenFactors), (3 * sizeof(glm::vec4) * numLights) + 2*sizeof(glm::vec4), sizeof(glm::vec4));

        numLights++;
    }

    // Pass number of lights within the scene
    // 25 is max number of lights
    uboLights->SetData(
        &numLights,
        25 * 3 * sizeof(glm::vec4),
        sizeof(uint32_t)
    );

    uboLights->Unbind();
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

    // Random ID for access in hashmap
    auto ID = Random::UINT64T();

    // Default components all entities should have
    entity.AddComponent<IDComponent>(ID);
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();

    entityMap[ID] = entity;

    return entity;
}

void Scene::RemoveEntity(entt::entity entity)
{
    registry.destroy(entity);
}


